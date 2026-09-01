package com.hpmcpe.serverrules.paper.listener;

import com.hpmcpe.serverrules.core.util.ColorUtil;
import com.hpmcpe.serverrules.paper.ServerRulesPaperPlugin;
import org.bukkit.Bukkit;
import org.bukkit.entity.Player;
import org.bukkit.event.EventHandler;
import org.bukkit.event.EventPriority;
import org.bukkit.event.Listener;
import org.bukkit.event.player.PlayerJoinEvent;

import java.util.List;

/**
 * Handles player join events to display rule reminders or require rule acceptance.
 */
public class PlayerJoinListener implements Listener {

    private final ServerRulesPaperPlugin plugin;

    public PlayerJoinListener(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
    }

    @EventHandler(priority = EventPriority.HIGH)
    public void onPlayerJoin(PlayerJoinEvent event) {
        Player player = event.getPlayer();

        // Delay message slightly so client is fully connected and chat is visible
        Bukkit.getScheduler().runTaskLater(plugin, () -> {
            if (!player.isOnline()) return;

            boolean acceptanceEnabled = plugin.getConfigManager().getMainConfig().getBoolean("acceptance.enabled", true);
            boolean remindJoin = plugin.getConfigManager().getMainConfig().getBoolean("join-reminder.enabled", true);
            boolean remindUnaccepted = plugin.getConfigManager().getMainConfig().getBoolean("acceptance.remind-unaccepted-players", true);

            boolean hasAccepted = plugin.getAcceptanceManager().hasAccepted(player.getUniqueId());

            if (remindJoin) {
                sendJoinReminder(player);
            }

            if (acceptanceEnabled && remindUnaccepted && !hasAccepted) {
                player.sendMessage(ColorUtil.colorize("&c&l[!] &eYou have not accepted the current server rules (v"
                        + plugin.getCurrentRulesVersion() + "). Type &a/rules accept &eto accept."));
            }
        }, 20L); // 1 second delay
    }

    private void sendJoinReminder(Player player) {
        List<String> lines = plugin.getConfigManager().getMessageList("join-reminder.message");
        if (lines.isEmpty()) {
            player.sendMessage(ColorUtil.colorize("&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
            player.sendMessage(ColorUtil.colorize("       &6&lSERVER RULES"));
            player.sendMessage(ColorUtil.colorize("&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
            player.sendMessage(ColorUtil.colorize("&fPlease make sure you follow the server rules."));
            player.sendMessage(ColorUtil.colorize("&7Type &e/rules &7to view them in full."));
            player.sendMessage(ColorUtil.colorize("&7━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"));
        } else {
            for (String line : lines) {
                player.sendMessage(line);
            }
        }
    }
}
