package com.hpmcpe.serverrules.api.manager;

import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.api.model.RuleSeverity;

import java.util.List;
import java.util.Optional;

/**
 * Manages all {@link Rule} instances for the server.
 *
 * <p>Provides CRUD operations, searching, and category-scoped retrieval.
 * All operations are synchronous and thread-safe.</p>
 *
 * <h3>Example usage:</h3>
 * <pre>{@code
 * RuleManager manager = ServerRulesAPI.getRuleManager();
 * List<Rule> chatRules = manager.getRulesByCategory("chat");
 * Optional<Rule> rule = manager.getRule("chat-no-spam");
 * }</pre>
 */
public interface RuleManager {

    /**
     * Retrieve a rule by its unique ID.
     *
     * @param id the rule ID (case-sensitive)
     * @return an Optional containing the rule, or empty if not found
     */
    Optional<Rule> getRule(String id);

    /**
     * Get all currently loaded rules, sorted by category order then rule order.
     *
     * @return unmodifiable list of all enabled and disabled rules
     */
    List<Rule> getAllRules();

    /**
     * Get all enabled rules.
     *
     * @return unmodifiable list of enabled rules
     */
    List<Rule> getEnabledRules();

    /**
     * Get all rules belonging to a specific category.
     *
     * @param categoryId the category identifier
     * @return unmodifiable list of rules in the category (enabled + disabled)
     */
    List<Rule> getRulesByCategory(String categoryId);

    /**
     * Get only enabled rules belonging to a specific category.
     *
     * @param categoryId the category identifier
     * @return unmodifiable list of enabled rules in the category
     */
    List<Rule> getEnabledRulesByCategory(String categoryId);

    /**
     * Search rules by keyword (case-insensitive substring match on text and description).
     *
     * @param keyword the search term
     * @return list of matching rules
     */
    List<Rule> searchRules(String keyword);

    /**
     * Add a new rule. The rule ID must be unique.
     *
     * @param rule the rule to add
     * @throws IllegalArgumentException if the ID already exists or is invalid
     */
    void addRule(Rule rule);

    /**
     * Remove a rule by ID.
     *
     * @param id the rule ID to remove
     * @return true if the rule was found and removed
     */
    boolean removeRule(String id);

    /**
     * Update an existing rule. The rule must already exist.
     *
     * @param rule the updated rule (matched by ID)
     * @throws IllegalArgumentException if no rule with that ID exists
     */
    void updateRule(Rule rule);

    /**
     * Check whether a rule ID already exists.
     *
     * @param id the rule ID
     * @return true if a rule with that ID exists
     */
    boolean existsRule(String id);

    /**
     * Get the total count of all rules (enabled + disabled).
     *
     * @return total rule count
     */
    int getTotalRuleCount();

    /**
     * Get the count of enabled rules only.
     *
     * @return enabled rule count
     */
    int getEnabledRuleCount();

    /**
     * Reload all rules from configuration files.
     */
    void reload();

    /**
     * Save all rules to configuration files.
     */
    void save();
}
