package com.hpmcpe.serverrules.api.model;

import java.util.List;
import java.util.Objects;

/**
 * Represents a category that groups related {@link Rule}s together.
 *
 * <p>Example YAML configuration:</p>
 * <pre>
 * categories:
 *   general:
 *     display-name: "&6General"
 *     description: "Basic rules everyone must follow."
 *     icon: "BOOK"
 *     icon-data: 0
 *     order: 1
 *     enabled: true
 * </pre>
 */
public final class RuleCategory {

    private final String id;
    private final String displayName;
    private final String description;
    private final String icon;
    private final int iconData;
    private final int order;
    private final boolean enabled;

    private RuleCategory(Builder builder) {
        this.id = builder.id;
        this.displayName = builder.displayName;
        this.description = builder.description;
        this.icon = builder.icon;
        this.iconData = builder.iconData;
        this.order = builder.order;
        this.enabled = builder.enabled;
    }

    /** @return unique category identifier (e.g. "general") */
    public String getId() { return id; }

    /** @return color-coded display name */
    public String getDisplayName() { return displayName; }

    /** @return short description of this category */
    public String getDescription() { return description; }

    /** @return material name for GUI icon */
    public String getIcon() { return icon; }

    /** @return custom model data for the GUI icon */
    public int getIconData() { return iconData; }

    /** @return sort order in the category list */
    public int getOrder() { return order; }

    /** @return whether this category is visible */
    public boolean isEnabled() { return enabled; }

    public Builder toBuilder() {
        return new Builder(id)
                .displayName(displayName)
                .description(description)
                .icon(icon)
                .iconData(iconData)
                .order(order)
                .enabled(enabled);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof RuleCategory that)) return false;
        return Objects.equals(id, that.id);
    }

    @Override
    public int hashCode() { return Objects.hash(id); }

    @Override
    public String toString() { return "RuleCategory{id='" + id + "'}"; }

    // -------------------------------------------------------------------------
    // Builder
    // -------------------------------------------------------------------------

    public static final class Builder {
        private final String id;
        private String displayName;
        private String description = "";
        private String icon = "BOOK";
        private int iconData = 0;
        private int order = 0;
        private boolean enabled = true;

        public Builder(String id) {
            if (id == null || id.isBlank()) throw new IllegalArgumentException("Category id must not be blank");
            this.id = id;
            this.displayName = id; // default display name is the id
        }

        public Builder displayName(String name) { this.displayName = name == null ? id : name; return this; }
        public Builder description(String desc) { this.description = desc == null ? "" : desc; return this; }
        public Builder icon(String icon) { this.icon = icon == null ? "BOOK" : icon; return this; }
        public Builder iconData(int data) { this.iconData = data; return this; }
        public Builder order(int order) { this.order = order; return this; }
        public Builder enabled(boolean enabled) { this.enabled = enabled; return this; }

        public RuleCategory build() { return new RuleCategory(this); }
    }
}
