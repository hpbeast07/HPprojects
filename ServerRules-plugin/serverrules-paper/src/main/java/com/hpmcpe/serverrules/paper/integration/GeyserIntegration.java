package com.hpmcpe.serverrules.paper.integration;

import org.bukkit.Bukkit;

/**
 * Detects and interacts with the Geyser proxy (Bedrock support).
 *
 * <p>Geyser is detected at class level to avoid hard-depending on it.
 * All methods are safe to call even if Geyser is not installed.</p>
 */
public final class GeyserIntegration {

    private static final boolean GEYSER_PRESENT;

    static {
        boolean found = false;
        try {
            Class.forName("org.geysermc.geyser.api.GeyserApi");
            found = Bukkit.getPluginManager().getPlugin("Geyser-Spigot") != null
                 || Bukkit.getPluginManager().getPlugin("Geyser-Paper") != null
                 || Bukkit.getPluginManager().getPlugin("Geyser") != null;
        } catch (ClassNotFoundException ignored) {}
        GEYSER_PRESENT = found;
    }

    private GeyserIntegration() {}

    /**
     * @return true if Geyser is installed and its API is available
     */
    public static boolean isAvailable() {
        return GEYSER_PRESENT;
    }
}
