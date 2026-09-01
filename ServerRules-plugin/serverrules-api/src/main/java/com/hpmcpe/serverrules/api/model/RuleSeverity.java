package com.hpmcpe.serverrules.api.model;

/**
 * Represents the severity level of a rule.
 * This is informational only — no automatic punishment is applied.
 */
public enum RuleSeverity {

    /** Minor infractions. */
    LOW,

    /** Standard infractions. */
    MEDIUM,

    /** Serious infractions. */
    HIGH,

    /** Severe infractions that may result in a permanent ban. */
    CRITICAL;

    /**
     * Parse a severity from a string, case-insensitive.
     *
     * @param value the string to parse
     * @return the matching severity, or {@link #MEDIUM} as fallback
     */
    public static RuleSeverity fromString(String value) {
        if (value == null) return MEDIUM;
        try {
            return valueOf(value.toUpperCase());
        } catch (IllegalArgumentException e) {
            return MEDIUM;
        }
    }
}
