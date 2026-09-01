package com.hpmcpe.serverrules.core.manager;

import com.hpmcpe.serverrules.api.manager.RuleManager;
import com.hpmcpe.serverrules.api.model.Rule;
import com.hpmcpe.serverrules.core.util.SRLogger;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

/**
 * Default thread-safe implementation of {@link RuleManager}.
 * Rules are stored in an in-memory map, indexed by their ID.
 */
public class DefaultRuleManager implements RuleManager {

    private final Map<String, Rule> rulesById = new ConcurrentHashMap<>();
    private final Runnable onSaveCallback;

    /**
     * @param onSaveCallback invoked when {@link #save()} is called,
     *                       allowing the platform adapter to persist data
     */
    public DefaultRuleManager(Runnable onSaveCallback) {
        this.onSaveCallback = onSaveCallback != null ? onSaveCallback : () -> {};
    }

    /** Load an initial set of rules (called after config reload). */
    public void loadRules(List<Rule> rules) {
        rulesById.clear();
        for (Rule rule : rules) {
            if (rulesById.containsKey(rule.getId())) {
                SRLogger.warning("Duplicate rule ID '" + rule.getId() + "' detected during load — skipping.");
            } else {
                rulesById.put(rule.getId(), rule);
            }
        }
    }

    @Override
    public Optional<Rule> getRule(String id) {
        if (id == null) return Optional.empty();
        return Optional.ofNullable(rulesById.get(id));
    }

    @Override
    public List<Rule> getAllRules() {
        return rulesById.values().stream()
                .sorted(Comparator.comparingInt(Rule::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public List<Rule> getEnabledRules() {
        return rulesById.values().stream()
                .filter(Rule::isEnabled)
                .sorted(Comparator.comparingInt(Rule::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public List<Rule> getRulesByCategory(String categoryId) {
        if (categoryId == null) return List.of();
        return rulesById.values().stream()
                .filter(r -> categoryId.equals(r.getCategoryId()))
                .sorted(Comparator.comparingInt(Rule::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public List<Rule> getEnabledRulesByCategory(String categoryId) {
        if (categoryId == null) return List.of();
        return rulesById.values().stream()
                .filter(r -> r.isEnabled() && categoryId.equals(r.getCategoryId()))
                .sorted(Comparator.comparingInt(Rule::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public List<Rule> searchRules(String keyword) {
        if (keyword == null || keyword.isBlank()) return List.of();
        String lower = keyword.toLowerCase(Locale.ROOT);
        return rulesById.values().stream()
                .filter(r -> r.getText().toLowerCase(Locale.ROOT).contains(lower)
                          || r.getDescription().toLowerCase(Locale.ROOT).contains(lower)
                          || r.getId().toLowerCase(Locale.ROOT).contains(lower))
                .sorted(Comparator.comparingInt(Rule::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public void addRule(Rule rule) {
        Objects.requireNonNull(rule, "rule must not be null");
        if (rulesById.containsKey(rule.getId())) {
            throw new IllegalArgumentException("Rule ID '" + rule.getId() + "' already exists.");
        }
        rulesById.put(rule.getId(), rule);
    }

    @Override
    public boolean removeRule(String id) {
        if (id == null) return false;
        return rulesById.remove(id) != null;
    }

    @Override
    public void updateRule(Rule rule) {
        Objects.requireNonNull(rule, "rule must not be null");
        if (!rulesById.containsKey(rule.getId())) {
            throw new IllegalArgumentException("No rule with ID '" + rule.getId() + "' exists.");
        }
        rulesById.put(rule.getId(), rule);
    }

    @Override
    public boolean existsRule(String id) {
        return id != null && rulesById.containsKey(id);
    }

    @Override
    public int getTotalRuleCount() { return rulesById.size(); }

    @Override
    public int getEnabledRuleCount() {
        return (int) rulesById.values().stream().filter(Rule::isEnabled).count();
    }

    @Override
    public void reload() {
        // Reload is driven externally by the platform plugin
        // (which re-runs the RulesLoader and calls loadRules)
    }

    @Override
    public void save() {
        onSaveCallback.run();
    }
}
