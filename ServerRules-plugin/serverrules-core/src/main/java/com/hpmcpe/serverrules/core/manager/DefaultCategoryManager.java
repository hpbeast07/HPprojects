package com.hpmcpe.serverrules.core.manager;

import com.hpmcpe.serverrules.api.manager.CategoryManager;
import com.hpmcpe.serverrules.api.model.RuleCategory;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

/**
 * Default thread-safe implementation of {@link CategoryManager}.
 */
public class DefaultCategoryManager implements CategoryManager {

    private final Map<String, RuleCategory> categoriesById = new ConcurrentHashMap<>();
    private final Runnable onSaveCallback;

    public DefaultCategoryManager(Runnable onSaveCallback) {
        this.onSaveCallback = onSaveCallback != null ? onSaveCallback : () -> {};
    }

    /** Load an initial set of categories (called after config reload). */
    public void loadCategories(List<RuleCategory> categories) {
        categoriesById.clear();
        for (RuleCategory cat : categories) {
            categoriesById.put(cat.getId(), cat);
        }
    }

    @Override
    public Optional<RuleCategory> getCategory(String id) {
        if (id == null) return Optional.empty();
        return Optional.ofNullable(categoriesById.get(id));
    }

    @Override
    public List<RuleCategory> getAllCategories() {
        return categoriesById.values().stream()
                .sorted(Comparator.comparingInt(RuleCategory::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public List<RuleCategory> getEnabledCategories() {
        return categoriesById.values().stream()
                .filter(RuleCategory::isEnabled)
                .sorted(Comparator.comparingInt(RuleCategory::getOrder))
                .collect(Collectors.toUnmodifiableList());
    }

    @Override
    public void addCategory(RuleCategory category) {
        Objects.requireNonNull(category, "category must not be null");
        if (categoriesById.containsKey(category.getId())) {
            throw new IllegalArgumentException("Category ID '" + category.getId() + "' already exists.");
        }
        categoriesById.put(category.getId(), category);
    }

    @Override
    public boolean removeCategory(String id) {
        if (id == null) return false;
        return categoriesById.remove(id) != null;
    }

    @Override
    public void updateCategory(RuleCategory category) {
        Objects.requireNonNull(category, "category must not be null");
        if (!categoriesById.containsKey(category.getId())) {
            throw new IllegalArgumentException("No category with ID '" + category.getId() + "' exists.");
        }
        categoriesById.put(category.getId(), category);
    }

    @Override
    public boolean existsCategory(String id) {
        return id != null && categoriesById.containsKey(id);
    }

    @Override
    public int getCategoryCount() { return categoriesById.size(); }

    @Override
    public void reload() {}

    @Override
    public void save() { onSaveCallback.run(); }
}
