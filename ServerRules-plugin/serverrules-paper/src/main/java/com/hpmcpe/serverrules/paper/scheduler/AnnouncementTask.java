package com.hpmcpe.serverrules.paper.scheduler;

import com.hpmcpe.serverrules.core.util.ColorUtil;
import com.hpmcpe.serverrules.paper.ServerRulesPaperPlugin;
import org.bukkit.Bukkit;
import org.bukkit.scheduler.BukkitRunnable;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Broadcasts rule announcement messages on a configurable interval.
 *
 * <p>Supports two modes (configured via {@code announcements.mode}):</p>
 * <ul>
 *   <li>{@code sequential} — messages rotate in order</li>
 *   <li>{@code random}     — messages are chosen at random</li>
 * </ul>
 *
 * <p>PlaceholderAPI placeholders in messages are resolved if PAPI is present.</p>
 */
public class AnnouncementTask extends BukkitRunnable {

    private final ServerRulesPaperPlugin plugin;
    private final AtomicInteger index = new AtomicInteger(0);

    public AnnouncementTask(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
    }

    @Override
    public void run() {
        List<String> messages = plugin.getConfigManager().getMainConfig()
                .getStringList("announcements.messages");

        if (messages == null || messages.isEmpty()) return;

        String mode = plugin.getConfigManager().getMainConfig()
                .getString("announcements.mode", "sequential");

        String raw;
        if ("random".equalsIgnoreCase(mode)) {
            int idx = (int) (Math.random() * messages.size());
            raw = messages.get(idx);
        } else {
            int current = index.getAndUpdate(i -> (i + 1) % messages.size());
            raw = messages.get(current);
        }

        String message = ColorUtil.colorize(raw);

        // If PlaceholderAPI is available, resolve global placeholders
        // (no specific player — server-wide announcement)
        try {
            if (Bukkit.getPluginManager().getPlugin("PlaceholderAPI") != null) {
                message = me.clip.placeholderapi.PlaceholderAPI.setPlaceholders(null, message);
            }
        } catch (Exception ignored) {}

        Bukkit.broadcastMessage(message);
    }
}
