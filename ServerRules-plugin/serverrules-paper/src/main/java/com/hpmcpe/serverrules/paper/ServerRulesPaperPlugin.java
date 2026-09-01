package com.hpmcpe.serverrules.paper;

import com.hpmcpe.serverrules.api.ServerRulesAPI;
import com.hpmcpe.serverrules.core.config.RulesLoader;
import com.hpmcpe.serverrules.core.manager.DefaultAcceptanceManager;
import com.hpmcpe.serverrules.core.manager.DefaultCategoryManager;
import com.hpmcpe.serverrules.core.manager.DefaultRuleManager;
import com.hpmcpe.serverrules.core.storage.JsonPlayerStorage;
import com.hpmcpe.serverrules.core.util.SRLogger;
import com.hpmcpe.serverrules.paper.command.ServerRulesCommand;
import com.hpmcpe.serverrules.paper.integration.FloodgateIntegration;
import com.hpmcpe.serverrules.paper.integration.GeyserIntegration;
import com.hpmcpe.serverrules.paper.integration.PlaceholderAPIExpansion;
import com.hpmcpe.serverrules.paper.listener.PlayerJoinListener;
import com.hpmcpe.serverrules.paper.scheduler.AnnouncementTask;
import org.bukkit.Bukkit;
import org.bukkit.command.PluginCommand;
import org.bukkit.plugin.java.JavaPlugin;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * Main Paper plugin entry point for ServerRules.
 *
 * <p>Startup sequence:</p>
 * <ol>
 *   <li>Copy default configs</li>
 *   <li>Load rules.yml → managers</li>
 *   <li>Load player acceptance data</li>
 *   <li>Register commands and listeners</li>
 *   <li>Detect optional integrations</li>
 *   <li>Register public API</li>
 * </ol>
 */
public final class ServerRulesPaperPlugin extends JavaPlugin {

    private static ServerRulesPaperPlugin instance;

    private DefaultRuleManager ruleManager;
    private DefaultCategoryManager categoryManager;
    private DefaultAcceptanceManager acceptanceManager;
    private PaperConfigManager configManager;
    private AnnouncementTask announcementTask;

    private int currentRulesVersion = 1;

    // Optional integration flags
    private boolean placeholderApiHooked = false;
    private boolean geyserDetected = false;
    private boolean floodgateDetected = false;

    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------

    @Override
    public void onEnable() {
        instance = this;
        SRLogger.setLogger(getLogger());

        printBanner();

        // 1. Save default config files
        saveDefaultConfigs();

        // 2. Initialize config manager
        configManager = new PaperConfigManager(this);
        configManager.reload();

        // 3. Initialize storage
        Path dataFolder = getDataFolder().toPath().resolve("data");
        JsonPlayerStorage storage = new JsonPlayerStorage(dataFolder);

        // 4. Initialize managers
        ruleManager = new DefaultRuleManager(null);
        categoryManager = new DefaultCategoryManager(null);
        acceptanceManager = new DefaultAcceptanceManager(
                () -> currentRulesVersion,
                storage
        );

        // 5. Load rules
        loadRules();

        // 6. Load player data
        acceptanceManager.load();

        // 7. Register public API
        ServerRulesAPI api = ServerRulesAPI.create(
                ruleManager, categoryManager, acceptanceManager, currentRulesVersion);
        ServerRulesAPI.register(api);

        // 8. Register commands
        registerCommands();

        // 9. Register listeners
        Bukkit.getPluginManager().registerEvents(new PlayerJoinListener(this), this);

        // 10. Start announcement task
        startAnnouncementTask();

        // 11. Detect optional integrations
        detectIntegrations();

        SRLogger.info("ServerRules v" + getDescription().getVersion() + " successfully enabled!");
    }

    @Override
    public void onDisable() {
        if (acceptanceManager != null) {
            acceptanceManager.save();
        }
        if (announcementTask != null) {
            announcementTask.cancel();
        }
        ServerRulesAPI.unregister();
        SRLogger.info("ServerRules disabled.");
        instance = null;
    }

    // -------------------------------------------------------------------------
    // Public access
    // -------------------------------------------------------------------------

    public static ServerRulesPaperPlugin getInstance() { return instance; }
    public DefaultRuleManager getRuleManager() { return ruleManager; }
    public DefaultCategoryManager getCategoryManager() { return categoryManager; }
    public DefaultAcceptanceManager getAcceptanceManager() { return acceptanceManager; }
    public PaperConfigManager getConfigManager() { return configManager; }
    public int getCurrentRulesVersion() { return currentRulesVersion; }
    public boolean isGeyserDetected() { return geyserDetected; }
    public boolean isFloodgateDetected() { return floodgateDetected; }

    // -------------------------------------------------------------------------
    // Reload
    // -------------------------------------------------------------------------

    /**
     * Perform a full plugin reload.
     */
    public void fullReload() {
        configManager.reload();
        loadRules();
        acceptanceManager.load();

        if (announcementTask != null) {
            announcementTask.cancel();
        }
        startAnnouncementTask();

        SRLogger.info("ServerRules reloaded.");
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    private void loadRules() {
        Path rulesFile = getDataFolder().toPath().resolve("rules.yml");
        RulesLoader loader = new RulesLoader(rulesFile);
        RulesLoader.LoadResult result = loader.load();

        currentRulesVersion = result.getRulesVersion();
        categoryManager.loadCategories(result.getCategories());
        ruleManager.loadRules(result.getRules());
    }

    private void registerCommands() {
        ServerRulesCommand handler = new ServerRulesCommand(this);

        for (String alias : new String[]{"rules", "serverrules", "serverrule", "srules"}) {
            PluginCommand cmd = getCommand(alias);
            if (cmd != null) {
                cmd.setExecutor(handler);
                cmd.setTabCompleter(handler);
            }
        }
    }

    private void saveDefaultConfigs() {
        saveResourceIfAbsent("config.yml");
        saveResourceIfAbsent("rules.yml");
        saveResourceIfAbsent("messages.yml");
        saveResourceIfAbsent("gui.yml");
        saveResourceIfAbsent("holograms.yml");
    }

    private void saveResourceIfAbsent(String name) {
        File target = new File(getDataFolder(), name);
        if (!target.exists()) {
            saveResource(name, false);
        }
    }

    private void startAnnouncementTask() {
        if (!configManager.getMainConfig().getBoolean("announcements.enabled", false)) return;

        long intervalTicks = configManager.getMainConfig().getLong("announcements.interval", 300L) * 20L;
        announcementTask = new AnnouncementTask(this);
        announcementTask.runTaskTimer(this, intervalTicks, intervalTicks);
    }

    private void detectIntegrations() {
        SRLogger.info("Platform: Paper");

        // PlaceholderAPI
        if (Bukkit.getPluginManager().getPlugin("PlaceholderAPI") != null) {
            try {
                new PlaceholderAPIExpansion(this).register();
                placeholderApiHooked = true;
                SRLogger.info("PlaceholderAPI: Hooked");
            } catch (Exception e) {
                SRLogger.warning("PlaceholderAPI: Hook failed — " + e.getMessage());
            }
        } else {
            SRLogger.info("PlaceholderAPI: Not installed (optional)");
        }

        // Geyser
        geyserDetected = GeyserIntegration.isAvailable();
        SRLogger.info("Geyser: " + (geyserDetected ? "Detected" : "Not installed"));

        // Floodgate
        floodgateDetected = FloodgateIntegration.isAvailable();
        SRLogger.info("Floodgate: " + (floodgateDetected ? "Detected" : "Not installed"));

        // DecentHolograms (logged only)
        boolean decentHolo = Bukkit.getPluginManager().getPlugin("DecentHolograms") != null;
        SRLogger.info("DecentHolograms: " + (decentHolo ? "Detected" : "Not installed"));
    }

    private void printBanner() {
        SRLogger.info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        SRLogger.info("  ServerRules — Rules & Regulations  ");
        SRLogger.info("  Version: " + getDescription().getVersion() + "          ");
        SRLogger.info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    }
}
