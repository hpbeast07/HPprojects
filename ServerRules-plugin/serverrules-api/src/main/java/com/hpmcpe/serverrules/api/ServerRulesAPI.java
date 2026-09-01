package com.hpmcpe.serverrules.api;

import com.hpmcpe.serverrules.api.manager.AcceptanceManager;
import com.hpmcpe.serverrules.api.manager.CategoryManager;
import com.hpmcpe.serverrules.api.manager.RuleManager;

/**
 * Static entry point for the ServerRules public API.
 *
 * <p>Other plugins can use this class to interact with the rules system:</p>
 * <pre>{@code
 * // Check if a player has accepted the rules
 * boolean accepted = ServerRulesAPI.getAcceptanceManager()
 *     .hasAccepted(player.getUniqueId());
 *
 * // Get all rules in the "chat" category
 * List<Rule> chatRules = ServerRulesAPI.getRuleManager()
 *     .getRulesByCategory("chat");
 * }</pre>
 *
 * <p>The API is available after {@code ServerRules} has been enabled.
 * Accessing it before the plugin enables will throw an {@link IllegalStateException}.</p>
 */
public final class ServerRulesAPI {

    private static ServerRulesAPI instance;

    private final RuleManager ruleManager;
    private final CategoryManager categoryManager;
    private final AcceptanceManager acceptanceManager;
    private final int rulesVersion;

    private ServerRulesAPI(RuleManager ruleManager,
                           CategoryManager categoryManager,
                           AcceptanceManager acceptanceManager,
                           int rulesVersion) {
        this.ruleManager = ruleManager;
        this.categoryManager = categoryManager;
        this.acceptanceManager = acceptanceManager;
        this.rulesVersion = rulesVersion;
    }

    // -------------------------------------------------------------------------
    // Registration (called by the plugin implementation, not by other plugins)
    // -------------------------------------------------------------------------

    /**
     * Register the API instance. Called internally by the platform plugin.
     *
     * @param api the API instance to register
     * @throws IllegalStateException if already registered
     */
    public static void register(ServerRulesAPI api) {
        if (instance != null) throw new IllegalStateException("ServerRulesAPI is already registered.");
        instance = api;
    }

    /**
     * Unregister the API instance. Called internally on plugin disable.
     */
    public static void unregister() {
        instance = null;
    }

    /**
     * Create a new API instance with the provided managers.
     */
    public static ServerRulesAPI create(RuleManager ruleManager,
                                        CategoryManager categoryManager,
                                        AcceptanceManager acceptanceManager,
                                        int rulesVersion) {
        return new ServerRulesAPI(ruleManager, categoryManager, acceptanceManager, rulesVersion);
    }

    // -------------------------------------------------------------------------
    // Public access
    // -------------------------------------------------------------------------

    private static ServerRulesAPI getInstance() {
        if (instance == null) throw new IllegalStateException("ServerRulesAPI is not yet available. Is ServerRules enabled?");
        return instance;
    }

    /**
     * Get the rule manager.
     *
     * @return the RuleManager
     * @throws IllegalStateException if the plugin is not enabled
     */
    public static RuleManager getRuleManager() {
        return getInstance().ruleManager;
    }

    /**
     * Get the category manager.
     *
     * @return the CategoryManager
     * @throws IllegalStateException if the plugin is not enabled
     */
    public static CategoryManager getCategoryManager() {
        return getInstance().categoryManager;
    }

    /**
     * Get the acceptance manager.
     *
     * @return the AcceptanceManager
     * @throws IllegalStateException if the plugin is not enabled
     */
    public static AcceptanceManager getAcceptanceManager() {
        return getInstance().acceptanceManager;
    }

    /**
     * Get the current rules version number.
     *
     * @return the current rules version
     */
    public static int getRulesVersion() {
        return getInstance().rulesVersion;
    }

    /**
     * Check whether the API is currently available.
     *
     * @return true if the plugin is enabled and the API is ready
     */
    public static boolean isAvailable() {
        return instance != null;
    }
}
