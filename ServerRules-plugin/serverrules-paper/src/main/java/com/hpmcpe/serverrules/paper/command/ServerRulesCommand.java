package com.hpmcpe.serverrules.paper.command;

import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.api.model.RuleCategory;
import com.hpmcpe.serverrules.api.model.RuleSeverity;
import com.hpmcpe.serverrules.core.util.ColorUtil;
import com.hpmcpe.serverrules.paper.ServerRulesPaperPlugin;
import com.hpmcpe.serverrules.paper.gui.GuiManager;
import com.hpmcpe.serverrules.paper.integration.FloodgateIntegration;
import org.bukkit.Bukkit;
import org.bukkit.OfflinePlayer;
import org.bukkit.command.*;
import org.bukkit.entity.Player;

import java.util.*;
import java.util.stream.Collectors;

/**
 * Handles all {@code /serverrules} and {@code /rules} command logic.
 *
 * <h3>Player commands:</h3>
 * <pre>
 * /rules                     — Open GUI (or chat list for Bedrock)
 * /rules accept              — Accept the current rules version
 * /rules status              — Check your acceptance status
 * /rules search &lt;keyword&gt;   — Search rules by keyword
 * /rules help                — Show help
 * </pre>
 *
 * <h3>Admin commands:</h3>
 * <pre>
 * /serverrules reload
 * /serverrules stats
 * /serverrules version
 * /serverrules version set &lt;n&gt;
 * /serverrules add &lt;category&gt; &lt;rule text...&gt;
 * /serverrules remove &lt;id&gt;
 * /serverrules edit &lt;id&gt; &lt;new text...&gt;
 * /serverrules list [category]
 * /serverrules reset &lt;player&gt;
 * /serverrules category add &lt;id&gt; [display-name]
 * /serverrules category remove &lt;id&gt;
 * /serverrules category list
 * </pre>
 */
public class ServerRulesCommand implements CommandExecutor, TabCompleter {

    private final ServerRulesPaperPlugin plugin;
    private final GuiManager guiManager;

    public ServerRulesCommand(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
        this.guiManager = new GuiManager(plugin);
    }

    // -------------------------------------------------------------------------
    // Command execution
    // -------------------------------------------------------------------------

    @Override
    public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {
        String cmdName = command.getName().toLowerCase(Locale.ROOT);

        // /rules or aliases with no args → open GUI / list
        if (cmdName.equals("rules") || cmdName.equals("srules") || cmdName.equals("serverrule")) {
            if (args.length == 0) {
                handleViewRules(sender);
                return true;
            }
        }

        if (args.length == 0) {
            // /serverrules with no args → show help
            sendHelp(sender);
            return true;
        }

        String sub = args[0].toLowerCase(Locale.ROOT);

        switch (sub) {
            case "help"     -> sendHelp(sender);
            case "accept"   -> handleAccept(sender);
            case "status"   -> handleStatus(sender);
            case "search"   -> handleSearch(sender, args);
            case "reload"   -> handleReload(sender);
            case "stats"    -> handleStats(sender);
            case "version"  -> handleVersion(sender, args);
            case "add"      -> handleAdd(sender, args);
            case "remove"   -> handleRemove(sender, args);
            case "edit"     -> handleEdit(sender, args);
            case "list"     -> handleList(sender, args);
            case "reset"    -> handleReset(sender, args);
            case "category" -> handleCategory(sender, args);
            default         -> {
                // /rules [anything] first arg may be a subcommand too
                if (cmdName.equals("rules") || cmdName.equals("srules")) {
                    switch (sub) {
                        case "accept"  -> handleAccept(sender);
                        case "status"  -> handleStatus(sender);
                        case "search"  -> handleSearch(sender, args);
                        case "help"    -> sendHelp(sender);
                        default        -> handleViewRules(sender);
                    }
                } else {
                    msg(sender, "&cUnknown subcommand. Use &e/serverrules help &cfor a list.");
                }
            }
        }

        return true;
    }

    // -------------------------------------------------------------------------
    // Player-facing subcommands
    // -------------------------------------------------------------------------

    private void handleViewRules(CommandSender sender) {
        if (!checkPerm(sender, "serverrules.view")) return;

        if (sender instanceof Player player) {
            // Check if Bedrock player (Floodgate)
            if (plugin.isFloodgateDetected() && FloodgateIntegration.isBedrockPlayer(player.getUniqueId())) {
                sendChatRules(player);
                return;
            }
            guiManager.openCategoryGui(player);
        } else {
            sendChatRulesSender(sender);
        }
    }

    private void handleAccept(CommandSender sender) {
        if (!checkPerm(sender, "serverrules.accept")) return;

        if (!(sender instanceof Player player)) {
            msg(sender, "&cOnly players can accept rules.");
            return;
        }

        if (!plugin.getConfigManager().getMainConfig().getBoolean("acceptance.enabled", true)) {
            msg(player, plugin.getConfigManager().getMessage(
                    "messages.acceptance-disabled",
                    "&cRule acceptance is not enabled on this server."));
            return;
        }

        plugin.getAcceptanceManager().setAccepted(player.getUniqueId(), player.getName());

        String accepted = plugin.getConfigManager().getMessage(
                "messages.rules-accepted",
                "&a✔ You have accepted the server rules (version %version%).");
        msg(player, accepted.replace("%version%",
                String.valueOf(plugin.getCurrentRulesVersion())));
    }

    private void handleStatus(CommandSender sender) {
        if (!checkPerm(sender, "serverrules.use")) return;

        if (!(sender instanceof Player player)) {
            msg(sender, "&cOnly players can check their own status.");
            return;
        }

        boolean accepted = plugin.getAcceptanceManager().hasAccepted(player.getUniqueId());
        int version = plugin.getCurrentRulesVersion();

        msg(player, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(player, "&6  Rules Status");
        msg(player, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(player, "&7Rules Version: &f" + version);
        msg(player, "&7Your Status:   " + (accepted ? "&aAccepted" : "&cNot Accepted"));

        if (!accepted) {
            msg(player, "&7Type &e/rules accept &7to accept the rules.");
        }
    }

    private void handleSearch(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.search")) return;

        if (args.length < 2) {
            msg(sender, "&cUsage: /rules search <keyword>");
            return;
        }

        String keyword = String.join(" ", Arrays.copyOfRange(args, 1, args.length));
        List<Rule> results = plugin.getRuleManager().searchRules(keyword);

        if (results.isEmpty()) {
            msg(sender, "&7No rules found matching &e" + keyword + "&7.");
            return;
        }

        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&6  Search Results for: &e" + keyword);
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        for (Rule rule : results) {
            String catId = rule.getCategoryId();
            String catName = plugin.getCategoryManager().getCategory(catId)
                    .map(c -> ColorUtil.colorize(c.getDisplayName()))
                    .orElse(catId);
            msg(sender, "&7[" + catName + "&7] &f" + ColorUtil.colorize(rule.getText()));
            if (!rule.getDescription().isEmpty()) {
                msg(sender, "  &8" + ColorUtil.colorize(rule.getDescription()));
            }
        }
        msg(sender, "&7Total: &f" + results.size() + " rule(s)");
    }

    // -------------------------------------------------------------------------
    // Admin subcommands
    // -------------------------------------------------------------------------

    private void handleReload(CommandSender sender) {
        if (!checkPerm(sender, "serverrules.reload")) return;
        plugin.fullReload();
        msg(sender, "&aServerRules reloaded successfully.");
    }

    private void handleStats(CommandSender sender) {
        if (!checkPerm(sender, "serverrules.admin")) return;

        int totalRules = plugin.getRuleManager().getTotalRuleCount();
        int enabledRules = plugin.getRuleManager().getEnabledRuleCount();
        int totalCats = plugin.getCategoryManager().getCategoryCount();
        int accepted = plugin.getAcceptanceManager().getAcceptedCount();
        int version = plugin.getCurrentRulesVersion();

        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&6  ServerRules Statistics");
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&7Rules Version:   &f" + version);
        msg(sender, "&7Total Rules:     &f" + totalRules + " &7(&a" + enabledRules + " enabled&7)");
        msg(sender, "&7Categories:      &f" + totalCats);
        msg(sender, "&7Accepted Players:&f " + accepted);
    }

    private void handleVersion(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.admin")) return;

        if (args.length == 1) {
            msg(sender, "&7Current rules version: &f" + plugin.getCurrentRulesVersion());
            return;
        }
        if (args.length >= 3 && args[1].equalsIgnoreCase("set")) {
            try {
                int newVersion = Integer.parseInt(args[2]);
                if (newVersion < 1) { msg(sender, "&cVersion must be >= 1."); return; }
                // Update rules.yml rules-version via config manager
                plugin.getConfigManager().getMainConfig(); // ensure loaded
                msg(sender, "&cPlease set 'rules-version: " + newVersion + "' in rules.yml and reload.");
                msg(sender, "&7Tip: &e/serverrules reload");
            } catch (NumberFormatException e) {
                msg(sender, "&cInvalid number: " + args[2]);
            }
            return;
        }
        msg(sender, "&cUsage: /serverrules version [set <number>]");
    }

    private void handleAdd(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.manage")) return;

        // /serverrules add <categoryId> <text...>
        if (args.length < 3) {
            msg(sender, "&cUsage: /serverrules add <category> <rule text...>");
            return;
        }

        String categoryId = args[1];
        if (!plugin.getCategoryManager().existsCategory(categoryId)) {
            msg(sender, "&cCategory '&e" + categoryId + "&c' does not exist.");
            msg(sender, "&7Available: " + String.join(", ",
                    plugin.getCategoryManager().getAllCategories().stream()
                            .map(RuleCategory::getId).toList()));
            return;
        }

        String text = String.join(" ", Arrays.copyOfRange(args, 2, args.length));
        int existingCount = plugin.getRuleManager().getRulesByCategory(categoryId).size();
        String id = categoryId + "-" + (existingCount + 1);

        // Ensure unique id
        int suffix = existingCount + 1;
        while (plugin.getRuleManager().existsRule(categoryId + "-" + suffix)) suffix++;
        id = categoryId + "-" + suffix;

        Rule rule = new Rule.Builder(id, categoryId).text(text).order(existingCount).build();
        plugin.getRuleManager().addRule(rule);
        msg(sender, "&aRule added with ID '&e" + id + "&a'. Remember to save it in rules.yml.");
    }

    private void handleRemove(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.manage")) return;

        if (args.length < 2) {
            msg(sender, "&cUsage: /serverrules remove <id>");
            return;
        }

        String id = args[1];
        boolean removed = plugin.getRuleManager().removeRule(id);
        if (removed) {
            msg(sender, "&aRule '&e" + id + "&a' removed. Remember to update rules.yml.");
        } else {
            msg(sender, "&cNo rule with ID '&e" + id + "&c' was found.");
        }
    }

    private void handleEdit(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.manage")) return;

        if (args.length < 3) {
            msg(sender, "&cUsage: /serverrules edit <id> <new text...>");
            return;
        }

        String id = args[1];
        String newText = String.join(" ", Arrays.copyOfRange(args, 2, args.length));

        plugin.getRuleManager().getRule(id).ifPresentOrElse(rule -> {
            Rule updated = rule.toBuilder().text(newText).build();
            plugin.getRuleManager().updateRule(updated);
            msg(sender, "&aRule '&e" + id + "&a' updated. Remember to update rules.yml.");
        }, () -> msg(sender, "&cNo rule with ID '&e" + id + "&c' was found."));
    }

    private void handleList(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.view")) return;

        if (args.length >= 2) {
            String catId = args[1];
            List<Rule> rules = plugin.getRuleManager().getRulesByCategory(catId);
            if (rules.isEmpty()) {
                msg(sender, "&7No rules found in category '&e" + catId + "&7'.");
                return;
            }
            msg(sender, "&6Rules in [" + catId + "]:");
            for (Rule rule : rules) {
                String status = rule.isEnabled() ? "&a✔" : "&c✘";
                msg(sender, " " + status + " &7[&e" + rule.getId() + "&7] &f" + ColorUtil.colorize(rule.getText()));
            }
        } else {
            // List all categories with rule counts
            msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
            msg(sender, "&6  All Rules");
            msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
            for (RuleCategory cat : plugin.getCategoryManager().getEnabledCategories()) {
                List<Rule> catRules = plugin.getRuleManager().getEnabledRulesByCategory(cat.getId());
                msg(sender, ColorUtil.colorize(cat.getDisplayName()) + " &7(" + catRules.size() + " rules)");
                for (Rule rule : catRules) {
                    msg(sender, "  &7• &f" + ColorUtil.colorize(rule.getText()));
                }
            }
        }
    }

    private void handleReset(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.admin")) return;

        if (args.length < 2) {
            msg(sender, "&cUsage: /serverrules reset <player>");
            return;
        }

        String targetName = args[1];
        @SuppressWarnings("deprecation")
        OfflinePlayer target = Bukkit.getOfflinePlayer(targetName);

        boolean reset = plugin.getAcceptanceManager().resetAcceptance(target.getUniqueId());
        if (reset) {
            msg(sender, "&aReset acceptance for &e" + targetName + "&a.");
        } else {
            msg(sender, "&7No acceptance record found for &e" + targetName + "&7.");
        }
    }

    private void handleCategory(CommandSender sender, String[] args) {
        if (!checkPerm(sender, "serverrules.manage")) return;

        if (args.length < 2) {
            msg(sender, "&cUsage: /serverrules category <add|remove|list> [args]");
            return;
        }

        String action = args[1].toLowerCase(Locale.ROOT);
        switch (action) {
            case "list" -> {
                plugin.getCategoryManager().getAllCategories().forEach(cat ->
                        msg(sender, "&7[&e" + cat.getId() + "&7] " + ColorUtil.colorize(cat.getDisplayName())
                                + " &7- enabled: " + (cat.isEnabled() ? "&ayes" : "&cno")));
            }
            case "add" -> {
                if (args.length < 3) { msg(sender, "&cUsage: /serverrules category add <id> [display-name]"); return; }
                String id = args[2];
                String displayName = args.length >= 4
                        ? String.join(" ", Arrays.copyOfRange(args, 3, args.length))
                        : "&f" + id;

                if (plugin.getCategoryManager().existsCategory(id)) {
                    msg(sender, "&cCategory '&e" + id + "&c' already exists.");
                    return;
                }
                RuleCategory cat = new RuleCategory.Builder(id).displayName(displayName).build();
                plugin.getCategoryManager().addCategory(cat);
                msg(sender, "&aCategory '&e" + id + "&a' created. Add it to rules.yml to persist.");
            }
            case "remove" -> {
                if (args.length < 3) { msg(sender, "&cUsage: /serverrules category remove <id>"); return; }
                boolean removed = plugin.getCategoryManager().removeCategory(args[2]);
                msg(sender, removed
                        ? "&aCategory '&e" + args[2] + "&a' removed."
                        : "&cNo category with ID '&e" + args[2] + "&c'.");
            }
            default -> msg(sender, "&cUnknown category action: " + action);
        }
    }

    // -------------------------------------------------------------------------
    // Help
    // -------------------------------------------------------------------------

    private void sendHelp(CommandSender sender) {
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&6        ServerRules Help");
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&e/rules                   &7View server rules");
        msg(sender, "&e/rules accept             &7Accept the rules");
        msg(sender, "&e/rules status             &7Check your acceptance status");
        msg(sender, "&e/rules search <text>      &7Search rules by keyword");
        msg(sender, "&e/rules help               &7Show this help");
        if (sender.hasPermission("serverrules.admin")) {
            msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
            msg(sender, "&c      Admin Commands");
            msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
            msg(sender, "&e/serverrules reload          &7Reload configuration");
            msg(sender, "&e/serverrules stats           &7Show statistics");
            msg(sender, "&e/serverrules version         &7Show rules version");
            msg(sender, "&e/serverrules add <cat> <text> &7Add a rule");
            msg(sender, "&e/serverrules remove <id>     &7Remove a rule");
            msg(sender, "&e/serverrules edit <id> <text> &7Edit a rule");
            msg(sender, "&e/serverrules list [cat]      &7List rules");
            msg(sender, "&e/serverrules reset <player>  &7Reset player acceptance");
            msg(sender, "&e/serverrules category add/remove/list");
        }
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    }

    // -------------------------------------------------------------------------
    // Chat-based rule display (for consoles and Bedrock players)
    // -------------------------------------------------------------------------

    private void sendChatRules(Player player) {
        sendChatRulesSender(player);
    }

    private void sendChatRulesSender(CommandSender sender) {
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&6      SERVER RULES");
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");

        for (RuleCategory category : plugin.getCategoryManager().getEnabledCategories()) {
            List<Rule> rules = plugin.getRuleManager().getEnabledRulesByCategory(category.getId());
            if (rules.isEmpty()) continue;

            msg(sender, "");
            msg(sender, ColorUtil.colorize(category.getDisplayName()));
            int num = 1;
            for (Rule rule : rules) {
                msg(sender, "&7  " + num + ". &f" + ColorUtil.colorize(rule.getText()));
                num++;
            }
        }

        msg(sender, "");
        msg(sender, "&7━━━━━━━━━━━━━━━━━━━━━━━━");
        msg(sender, "&7Rules Version: &f" + plugin.getCurrentRulesVersion());
        if (sender instanceof Player player) {
            boolean accepted = plugin.getAcceptanceManager().hasAccepted(player.getUniqueId());
            if (!accepted && plugin.getConfigManager().getMainConfig().getBoolean("acceptance.enabled", true)) {
                msg(sender, "&7Type &e/rules accept &7to accept the rules.");
            }
        }
    }

    // -------------------------------------------------------------------------
    // Tab completion
    // -------------------------------------------------------------------------

    @Override
    public List<String> onTabComplete(CommandSender sender, Command command, String alias, String[] args) {
        String cmdName = command.getName().toLowerCase(Locale.ROOT);

        if (args.length == 1) {
            List<String> subs = new ArrayList<>(List.of("accept", "status", "search", "help"));
            if (sender.hasPermission("serverrules.admin")) {
                subs.addAll(List.of("reload", "stats", "version", "add", "remove",
                        "edit", "list", "reset", "category"));
            }
            return filterPrefix(subs, args[0]);
        }

        if (args.length == 2) {
            switch (args[0].toLowerCase(Locale.ROOT)) {
                case "add", "list" -> {
                    List<String> cats = plugin.getCategoryManager().getAllCategories()
                            .stream().map(RuleCategory::getId).collect(Collectors.toList());
                    return filterPrefix(cats, args[1]);
                }
                case "remove", "edit" -> {
                    List<String> ids = plugin.getRuleManager().getAllRules()
                            .stream().map(Rule::getId).collect(Collectors.toList());
                    return filterPrefix(ids, args[1]);
                }
                case "reset" -> {
                    return Bukkit.getOnlinePlayers().stream()
                            .map(Player::getName)
                            .filter(n -> n.toLowerCase().startsWith(args[1].toLowerCase()))
                            .collect(Collectors.toList());
                }
                case "version" -> { return filterPrefix(List.of("set"), args[1]); }
                case "category" -> { return filterPrefix(List.of("add", "remove", "list"), args[1]); }
            }
        }

        if (args.length == 3 && args[0].equalsIgnoreCase("category")
                && args[1].equalsIgnoreCase("remove")) {
            List<String> cats = plugin.getCategoryManager().getAllCategories()
                    .stream().map(RuleCategory::getId).collect(Collectors.toList());
            return filterPrefix(cats, args[2]);
        }

        return List.of();
    }

    // -------------------------------------------------------------------------
    // Utilities
    // -------------------------------------------------------------------------

    private boolean checkPerm(CommandSender sender, String permission) {
        if (!sender.hasPermission(permission)) {
            msg(sender, plugin.getConfigManager().getMessage(
                    "messages.no-permission",
                    "&cYou do not have permission to use this command."));
            return false;
        }
        return true;
    }

    private void msg(CommandSender sender, String message) {
        sender.sendMessage(ColorUtil.colorize(message));
    }

    private List<String> filterPrefix(List<String> list, String prefix) {
        if (prefix == null || prefix.isBlank()) return list;
        String lower = prefix.toLowerCase(Locale.ROOT);
        return list.stream()
                .filter(s -> s.toLowerCase(Locale.ROOT).startsWith(lower))
                .collect(Collectors.toList());
    }
}
