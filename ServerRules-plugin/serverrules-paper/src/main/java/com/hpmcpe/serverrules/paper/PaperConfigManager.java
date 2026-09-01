package com.hpmcpe.serverrules.paper;

import com.hpmcpe.serverrules.core.config.YamlConfig;
import com.hpmcpe.serverrules.core.util.ColorUtil;
import com.hpmcpe.serverrules.core.util.SRLogger;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;

/**
 * Loads and caches all plugin configuration files.
 *
 * <p>Files loaded:</p>
 * <ul>
 *   <li>{@code config.yml} — general settings</li>
 *   <li>{@code messages.yml} — player-facing messages</li>
 *   <li>{@code gui.yml} — GUI layout configuration</li>
 *   <li>{@code holograms.yml} — hologram settings</li>
 * </ul>
 *
 * <p>Use {@link #getMessage(String, String)} to get color-translated messages.</p>
 */
public class PaperConfigManager {

    private final ServerRulesPaperPlugin plugin;

    private YamlConfig mainConfig;
    private YamlConfig messages;
    private YamlConfig guiConfig;
    private YamlConfig hologramConfig;

    public PaperConfigManager(ServerRulesPaperPlugin plugin) {
        this.plugin = plugin;
    }

    /** Reload all configuration files from disk. */
    public void reload() {
        Path dir = plugin.getDataFolder().toPath();
        mainConfig    = load(dir.resolve("config.yml"), "config.yml");
        messages      = load(dir.resolve("messages.yml"), "messages.yml");
        guiConfig     = load(dir.resolve("gui.yml"), "gui.yml");
        hologramConfig = load(dir.resolve("holograms.yml"), "holograms.yml");
    }

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------

    public YamlConfig getMainConfig()     { return mainConfig; }
    public YamlConfig getGuiConfig()      { return guiConfig; }
    public YamlConfig getHologramConfig() { return hologramConfig; }

    /**
     * Get a color-translated message from messages.yml.
     *
     * @param key          dot-notation key
     * @param defaultValue fallback text if key is missing
     * @return colorized message string
     */
    public String getMessage(String key, String defaultValue) {
        String raw = messages.getString(key, defaultValue);
        return ColorUtil.colorize(raw);
    }

    /**
     * Get a list of color-translated messages.
     *
     * @param key dot-notation key
     * @return list of colorized strings
     */
    public List<String> getMessageList(String key) {
        return messages.getStringList(key).stream()
                .map(ColorUtil::colorize)
                .toList();
    }

    /**
     * Get a color-translated value from config.yml.
     *
     * @param key          dot-notation key
     * @param defaultValue fallback
     * @return colorized string
     */
    public String getConfigString(String key, String defaultValue) {
        return ColorUtil.colorize(mainConfig.getString(key, defaultValue));
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    private YamlConfig load(Path file, String resourceName) {
        if (!file.toFile().exists()) {
            SRLogger.warning("Config file not found: " + file + " — using empty config.");
            return YamlConfig.loadFromStream(null);
        }
        try {
            return YamlConfig.load(file);
        } catch (IOException e) {
            SRLogger.severe("Failed to load " + resourceName + ": " + e.getMessage(), e);
            return YamlConfig.loadFromStream(null);
        }
    }
}
