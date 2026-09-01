package com.hpmcpe.serverrules.paper.integration;

import org.bukkit.Bukkit;

import java.util.UUID;

/**
 * Detects Floodgate and identifies Bedrock players.
 *
 * <p>Floodgate is the companion to Geyser that runs on the backend server.
 * It allows identifying which players are Bedrock clients.</p>
 *
 * <p>All methods are safe to call even if Floodgate is not installed.</p>
 */
public final class FloodgateIntegration {

    private static final boolean FLOODGATE_PRESENT;

    static {
        boolean found = false;
        try {
            Class.forName("org.geysermc.floodgate.api.FloodgateApi");
            found = Bukkit.getPluginManager().getPlugin("floodgate") != null
                 || Bukkit.getPluginManager().getPlugin("Floodgate") != null;
        } catch (ClassNotFoundException ignored) {}
        FLOODGATE_PRESENT = found;
    }

    private FloodgateIntegration() {}

    /**
     * @return true if Floodgate is installed and its API is available
     */
    public static boolean isAvailable() {
        return FLOODGATE_PRESENT;
    }

    /**
     * Check whether a player UUID belongs to a Bedrock client.
     *
     * <p>Safe to call even if Floodgate is not installed — returns false.</p>
     *
     * @param uuid the player UUID
     * @return true if the player is a Bedrock client connected through Floodgate
     */
    public static boolean isBedrockPlayer(UUID uuid) {
        if (!FLOODGATE_PRESENT) return false;
        try {
            return org.geysermc.floodgate.api.FloodgateApi.getInstance().isFloodgatePlayer(uuid);
        } catch (Exception e) {
            return false;
        }
    }
}
