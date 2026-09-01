package com.hpmcpe.serverrules.api.model;

import java.util.Objects;

/**
 * Represents a single immutable server rule.
 *
 * <p>Rules are identified by a unique string ID and belong to a category.
 * They carry display text, an optional description, a severity, and an enabled flag.</p>
 *
 * <p>Use {@link Builder} to construct instances:</p>
 * <pre>{@code
 * Rule rule = new Rule.Builder("general-1", "general")
 *     .text("Respect all players.")
 *     .severity(RuleSeverity.HIGH)
 *     .build();
 * }</pre>
 */
public final class Rule {

    private final String id;
    private final String categoryId;
    private final String text;
    private final String description;
    private final RuleSeverity severity;
    private final boolean enabled;
    private final int order;

    private Rule(Builder builder) {
        this.id = builder.id;
        this.categoryId = builder.categoryId;
        this.text = builder.text;
        this.description = builder.description;
        this.severity = builder.severity;
        this.enabled = builder.enabled;
        this.order = builder.order;
    }

    /** @return the unique rule identifier */
    public String getId() { return id; }

    /** @return the category this rule belongs to */
    public String getCategoryId() { return categoryId; }

    /** @return the display text of the rule */
    public String getText() { return text; }

    /** @return the optional extended description, or empty string */
    public String getDescription() { return description; }

    /** @return the informational severity of this rule */
    public RuleSeverity getSeverity() { return severity; }

    /** @return whether this rule is currently active */
    public boolean isEnabled() { return enabled; }

    /** @return the sort order within the category */
    public int getOrder() { return order; }

    /**
     * Create a new builder copying this rule's values.
     *
     * @return a mutable builder pre-populated with this rule's fields
     */
    public Builder toBuilder() {
        return new Builder(id, categoryId)
                .text(text)
                .description(description)
                .severity(severity)
                .enabled(enabled)
                .order(order);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof Rule rule)) return false;
        return Objects.equals(id, rule.id);
    }

    @Override
    public int hashCode() { return Objects.hash(id); }

    @Override
    public String toString() {
        return "Rule{id='" + id + "', category='" + categoryId + "', enabled=" + enabled + "}";
    }

    // -------------------------------------------------------------------------
    // Builder
    // -------------------------------------------------------------------------

    public static final class Builder {
        private final String id;
        private final String categoryId;
        private String text = "";
        private String description = "";
        private RuleSeverity severity = RuleSeverity.MEDIUM;
        private boolean enabled = true;
        private int order = 0;

        public Builder(String id, String categoryId) {
            if (id == null || id.isBlank()) throw new IllegalArgumentException("Rule id must not be blank");
            if (categoryId == null || categoryId.isBlank()) throw new IllegalArgumentException("Category id must not be blank");
            this.id = id;
            this.categoryId = categoryId;
        }

        public Builder text(String text) { this.text = text == null ? "" : text; return this; }
        public Builder description(String description) { this.description = description == null ? "" : description; return this; }
        public Builder severity(RuleSeverity severity) { this.severity = severity == null ? RuleSeverity.MEDIUM : severity; return this; }
        public Builder enabled(boolean enabled) { this.enabled = enabled; return this; }
        public Builder order(int order) { this.order = order; return this; }

        public Rule build() {
            if (text.isBlank()) throw new IllegalStateException("Rule text must not be blank for id: " + id);
            return new Rule(this);
        }
    }
}
