package com.hpmcpe.serverrules.paper.integration;

import com.hpmcpe.serverrules.api.model.RuleCategory;
import com.hpmcpe.serverrules.paper.ServerRulesPaperPlugin;
import me.clip.placeholderapi.expansion.PlaceholderExpansion;
import org.bukkit.OfflinePlayer;
import org.jetbrains.annotations.NotNull;

/**
 * PlaceholderAPI expansion providing ServerRules placeholders.
 *
 * <p>Available placeholders:</p>
 * <pre>
 * %serverrules_total%          — Total rule count (enabled + disabled)
 * %serverrules_enabled%        — Enabled rule count only
 * %serverrules_categories%     — Total category count
 * %serverrules_version%        — Current rules version number
 * %serverrules_player_status%  — "Accepted" or "Pending" for the requesting player
 * %serverrules_player_accepted% — "true" or "false"
 * %serverrules_accepted_count% — Number of players who accepted current version
 * %serverrules_category_<id>%  — Rule count for a specific category (e.g. %serverrules_category_chat%)
 * </pre>
 *
 * <p>This class is only loaded when PlaceholderAPI is present. It is registered
 * in {@link ServerRulesPaperPlugin} after checking for the PAPI plugin.</p>
 */
public class PlaceholderAPIExpansion extends PlaceholderExpansion {

    private final ServerRulesPaperPlugin plugin;

    public PlaceholderAPIExpansion(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
    }

    @Override
    public @NotNull String getIdentifier() {
        return "serverrules";
    }

    @Override
    public @NotNull String getAuthor() {
        return String.join(", ", plugin.getDescription().getAuthors());
    }

    @Override
    public @NotNull String getVersion() {
        return plugin.getDescription().getVersion();
    }

    @Override
    public boolean persist() {
        return true; // do not unregister when PAPI reloads
    }

    @Override
    public String onRequest(OfflinePlayer player, @NotNull String params) {
        String p = params.toLowerCase();

        return switch (p) {
            case "total"         -> String.valueOf(plugin.getRuleManager().getTotalRuleCount());
            case "enabled"       -> String.valueOf(plugin.getRuleManager().getEnabledRuleCount());
            case "categories"    -> String.valueOf(plugin.getCategoryManager().getCategoryCount());
            case "version"       -> String.valueOf(plugin.getCurrentRulesVersion());
            case "accepted_count" -> String.valueOf(plugin.getAcceptanceManager().getAcceptedCount());
            case "player_status" -> {
                if (player == null) yield "Unknown";
                boolean accepted = plugin.getAcceptanceManager().hasAccepted(player.getUniqueId());
                yield accepted ? "Accepted" : "Pending";
            }
            case "player_accepted" -> {
                if (player == null) yield "false";
                yield String.valueOf(plugin.getAcceptanceManager().hasAccepted(player.getUniqueId()));
            }
            default -> {
                // %serverrules_category_<id>%
                if (p.startsWith("category_")) {
                    String catId = p.substring("category_".length());
                    int count = plugin.getRuleManager().getEnabledRulesByCategory(catId).size();
                    yield String.valueOf(count);
                }
                yield null; // unknown placeholder
            }
        };
    }
}
