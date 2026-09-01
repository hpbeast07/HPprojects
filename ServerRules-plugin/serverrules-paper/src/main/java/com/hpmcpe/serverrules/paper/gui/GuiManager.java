package com.hpmcpe.serverrules.paper.gui;

import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.api.model.RuleCategory;
import com.hpmcpe.serverrules.core.util.ColorUtil;
import com.hpmcpe.serverrules.paper.ServerRulesPaperPlugin;
import org.bukkit.Bukkit;
import org.bukkit.Material;
import org.bukkit.entity.Player;
import org.bukkit.event.EventHandler;
import org.bukkit.event.Listener;
import org.bukkit.event.inventory.InventoryClickEvent;
import org.bukkit.event.inventory.InventoryCloseEvent;
import org.bukkit.inventory.Inventory;
import org.bukkit.inventory.ItemStack;
import org.bukkit.inventory.meta.ItemMeta;

import java.util.*;

/**
 * Manages the rules inventory GUI.
 *
 * <h3>GUI flow:</h3>
 * <ol>
 *   <li>Category selection screen</li>
 *   <li>Rule list screen (paginated)</li>
 *   <li>Accept button on category screen if acceptance is enabled</li>
 * </ol>
 *
 * <p>The GUI auto-registers itself as a Bukkit event listener.</p>
 */
public class GuiManager implements Listener {

    // GUI session tracking — player UUID → current state
    private final Map<UUID, GuiSession> sessions = new HashMap<>();
    private final ServerRulesPaperPlugin plugin;

    public GuiManager(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
        Bukkit.getPluginManager().registerEvents(this, plugin);
    }

    // -------------------------------------------------------------------------
    // Open methods
    // -------------------------------------------------------------------------

    /** Open the category selection screen for a player. */
    public void openCategoryGui(Player player) {
        List<RuleCategory> categories = plugin.getCategoryManager().getEnabledCategories();
        String title = ColorUtil.colorize(plugin.getConfigManager().getGuiConfig()
                .getString("gui.category-title", "&6&lServer Rules"));

        // Size: fill rows as needed (multiple of 9, max 54)
        int rows = Math.min(6, (int) Math.ceil((categories.size() + 2) / 9.0) + 1);
        rows = Math.max(rows, 2);
        int size = rows * 9;

        Inventory inv = Bukkit.createInventory(null, size, title);

        // Fill background
        ItemStack filler = makeItem(Material.GRAY_STAINED_GLASS_PANE, " ", null);
        for (int i = 0; i < size; i++) inv.setItem(i, filler);

        // Category buttons
        int slot = 10; // start one row in, one col offset
        for (RuleCategory category : categories) {
            Material mat = parseMaterial(category.getIcon(), Material.BOOK);
            List<String> lore = new ArrayList<>();
            if (!category.getDescription().isEmpty()) {
                lore.add(ColorUtil.colorize("&7" + category.getDescription()));
            }
            lore.add("");
            lore.add(ColorUtil.colorize("&eClick to view rules"));
            ItemStack item = makeItem(mat, ColorUtil.colorize(category.getDisplayName()), lore);
            inv.setItem(slot, item);
            slot++;
            if (slot % 9 == 8) slot += 2; // skip border columns
            if (slot >= size - 9) break;
        }

        // Accept button (bottom row center)
        if (plugin.getConfigManager().getMainConfig().getBoolean("acceptance.enabled", true)) {
            boolean accepted = plugin.getAcceptanceManager().hasAccepted(player.getUniqueId());
            Material acceptMat = accepted ? Material.LIME_WOOL : Material.RED_WOOL;
            String acceptName = accepted
                    ? ColorUtil.colorize("&a✔ Rules Accepted")
                    : ColorUtil.colorize("&c✘ Click to Accept Rules");
            ItemStack acceptBtn = makeItem(acceptMat, acceptName, List.of(
                    ColorUtil.colorize("&7Version: &f" + plugin.getCurrentRulesVersion())
            ));
            inv.setItem(size - 5, acceptBtn);
        }

        // Close button
        inv.setItem(size - 1, makeItem(Material.BARRIER,
                ColorUtil.colorize("&cClose"), null));

        // View All button
        inv.setItem(size - 9, makeItem(Material.COMPASS,
                ColorUtil.colorize("&eView All Rules"), List.of(
                        ColorUtil.colorize("&7Shows all rules in chat"))));

        sessions.put(player.getUniqueId(), new GuiSession(GuiState.CATEGORY, null, 0));
        player.openInventory(inv);
    }

    /** Open the rules list for a specific category. */
    public void openRulesGui(Player player, String categoryId, int page) {
        List<Rule> rules = plugin.getRuleManager().getEnabledRulesByCategory(categoryId);
        RuleCategory category = plugin.getCategoryManager().getCategory(categoryId).orElse(null);
        if (category == null) return;

        String title = ColorUtil.colorize("&6Rules: " +
                ColorUtil.colorize(category.getDisplayName()) + " &8[" + (page + 1) + "]");
        Inventory inv = Bukkit.createInventory(null, 54, title);

        // Filler
        ItemStack filler = makeItem(Material.GRAY_STAINED_GLASS_PANE, " ", null);
        for (int i = 0; i < 54; i++) inv.setItem(i, filler);

        // Rules — 28 per page (slots 10-16, 19-25, 28-34, 37-43)
        int[] ruleSlots = buildRuleSlots();
        int startIndex = page * ruleSlots.length;
        for (int i = 0; i < ruleSlots.length && (startIndex + i) < rules.size(); i++) {
            Rule rule = rules.get(startIndex + i);
            List<String> lore = new ArrayList<>();
            if (!rule.getDescription().isEmpty()) {
                lore.add(ColorUtil.colorize("&7" + rule.getDescription()));
                lore.add("");
            }
            lore.add(ColorUtil.colorize("&8ID: " + rule.getId()));
            lore.add(ColorUtil.colorize("&8Severity: " + rule.getSeverity().name()));
            ItemStack item = makeItem(Material.PAPER,
                    ColorUtil.colorize("&f" + rule.getText()), lore);
            inv.setItem(ruleSlots[i], item);
        }

        // Navigation
        int totalPages = (int) Math.ceil((double) rules.size() / ruleSlots.length);
        if (page > 0) {
            inv.setItem(45, makeItem(Material.ARROW,
                    ColorUtil.colorize("&ePrevious Page"), null));
        }
        if (page < totalPages - 1) {
            inv.setItem(53, makeItem(Material.ARROW,
                    ColorUtil.colorize("&eNext Page"), null));
        }

        // Back button
        inv.setItem(49, makeItem(Material.DARK_OAK_DOOR,
                ColorUtil.colorize("&7← Back to Categories"), null));

        sessions.put(player.getUniqueId(), new GuiSession(GuiState.RULES, categoryId, page));
        player.openInventory(inv);
    }

    // -------------------------------------------------------------------------
    // Event handling
    // -------------------------------------------------------------------------

    @EventHandler
    public void onInventoryClick(InventoryClickEvent event) {
        if (!(event.getWhoClicked() instanceof Player player)) return;
        GuiSession session = sessions.get(player.getUniqueId());
        if (session == null) return;

        event.setCancelled(true);
        if (event.getCurrentItem() == null || event.getCurrentItem().getType() == Material.AIR) return;

        Inventory inv = event.getInventory();
        int size = inv.getSize();
        int slot = event.getRawSlot();
        ItemStack clicked = event.getCurrentItem();

        if (session.state == GuiState.CATEGORY) {
            // Close button
            if (clicked.getType() == Material.BARRIER) {
                player.closeInventory();
                return;
            }
            // Accept button
            if (clicked.getType() == Material.RED_WOOL || clicked.getType() == Material.LIME_WOOL) {
                handleAcceptClick(player, clicked.getType() == Material.LIME_WOOL);
                return;
            }
            // View All
            if (clicked.getType() == Material.COMPASS) {
                player.closeInventory();
                // Send chat list
                Bukkit.getScheduler().runTaskLater(plugin, () -> {
                    // Re-use the command's chat sender
                    plugin.getServer().dispatchCommand(player, "rules list");
                }, 1L);
                return;
            }
            // Category button — find which category was clicked
            List<RuleCategory> cats = plugin.getCategoryManager().getEnabledCategories();
            int catIndex = getCategoryIndexFromSlot(slot, cats.size(), size);
            if (catIndex >= 0 && catIndex < cats.size()) {
                openRulesGui(player, cats.get(catIndex).getId(), 0);
            }

        } else if (session.state == GuiState.RULES) {
            if (slot == 49 || clicked.getType() == Material.DARK_OAK_DOOR) {
                openCategoryGui(player);
            } else if (slot == 45) {
                openRulesGui(player, session.categoryId, session.page - 1);
            } else if (slot == 53) {
                openRulesGui(player, session.categoryId, session.page + 1);
            }
        }
    }

    @EventHandler
    public void onInventoryClose(InventoryCloseEvent event) {
        if (event.getPlayer() instanceof Player player) {
            sessions.remove(player.getUniqueId());
        }
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    private void handleAcceptClick(Player player, boolean alreadyAccepted) {
        if (alreadyAccepted) {
            player.sendMessage(ColorUtil.colorize("&7You have already accepted the rules."));
            return;
        }
        plugin.getAcceptanceManager().setAccepted(player.getUniqueId(), player.getName());
        String msg = plugin.getConfigManager().getMessage(
                "messages.rules-accepted",
                "&a✔ You have accepted the server rules (version %version%).");
        player.sendMessage(msg.replace("%version%",
                String.valueOf(plugin.getCurrentRulesVersion())));
        player.closeInventory();
    }

    private int getCategoryIndexFromSlot(int slot, int catCount, int invSize) {
        // Categories start at slot 10, skip every 8th and 9th column border
        List<Integer> catSlots = new ArrayList<>();
        int s = 10;
        for (int i = 0; i < catCount; i++) {
            catSlots.add(s);
            s++;
            if (s % 9 == 8) s += 2;
            if (s >= invSize - 9) break;
        }
        return catSlots.indexOf(slot);
    }

    private int[] buildRuleSlots() {
        // 4 rows × 7 cols = 28 rule slots
        int[] slots = new int[28];
        int idx = 0;
        for (int row = 0; row < 4; row++) {
            for (int col = 1; col <= 7; col++) {
                slots[idx++] = row * 9 + col;
            }
        }
        return slots;
    }

    private ItemStack makeItem(Material material, String name, List<String> lore) {
        ItemStack item = new ItemStack(material);
        ItemMeta meta = item.getItemMeta();
        if (meta != null) {
            meta.setDisplayName(name == null ? " " : name);
            if (lore != null && !lore.isEmpty()) meta.setLore(lore);
            item.setItemMeta(meta);
        }
        return item;
    }

    private Material parseMaterial(String name, Material fallback) {
        if (name == null || name.isBlank()) return fallback;
        try {
            return Material.valueOf(name.toUpperCase(Locale.ROOT));
        } catch (IllegalArgumentException e) {
            return fallback;
        }
    }

    // -------------------------------------------------------------------------
    // Session
    // -------------------------------------------------------------------------

    private enum GuiState { CATEGORY, RULES }

    private record GuiSession(GuiState state, String categoryId, int page) {}
}
