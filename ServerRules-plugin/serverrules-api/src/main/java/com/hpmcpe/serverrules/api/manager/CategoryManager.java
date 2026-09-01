package com.hpmcpe.serverrules.api.manager;

import com.hpmcpe.serverrules.api.model.RuleCategory;

import java.util.List;
import java.util.Optional;

/**
 * Manages {@link RuleCategory} instances.
 */
public interface CategoryManager {

    /**
     * Retrieve a category by ID.
     *
     * @param id the category ID
     * @return Optional containing the category, or empty
     */
    Optional<RuleCategory> getCategory(String id);

    /**
     * Get all categories sorted by their order value.
     *
     * @return unmodifiable list of all categories
     */
    List<RuleCategory> getAllCategories();

    /**
     * Get only enabled categories.
     *
     * @return unmodifiable list of enabled categories
     */
    List<RuleCategory> getEnabledCategories();

    /**
     * Add a new category.
     *
     * @param category the category to add
     * @throws IllegalArgumentException if the ID already exists
     */
    void addCategory(RuleCategory category);

    /**
     * Remove a category by ID.
     *
     * @param id the category ID
     * @return true if found and removed
     */
    boolean removeCategory(String id);

    /**
     * Update an existing category.
     *
     * @param category the updated category
     */
    void updateCategory(RuleCategory category);

    /**
     * Check whether a category ID exists.
     *
     * @param id the category ID
     * @return true if the category exists
     */
    boolean existsCategory(String id);

    /** @return total number of categories */
    int getCategoryCount();

    /** Reload categories from configuration. */
    void reload();

    /** Save categories to configuration. */
    void save();
}
