package com.hpmcpe.serverrules.core.util;

import java.util.logging.Logger;

/**
 * Centralized logger wrapper that prefixes all messages with {@code [ServerRules]}.
 */
public final class SRLogger {

    private static Logger logger = Logger.getLogger("ServerRules");

    private SRLogger() {}

    /**
     * Set the backing logger (called by the platform plugin on startup).
     *
     * @param backingLogger the platform logger to delegate to
     */
    public static void setLogger(Logger backingLogger) {
        logger = backingLogger;
    }

    public static void info(String message) {
        logger.info(message);
    }

    public static void warning(String message) {
        logger.warning(message);
    }

    public static void severe(String message) {
        logger.severe(message);
    }

    public static void severe(String message, Throwable throwable) {
        logger.severe(message);
        logger.severe("Cause: " + throwable.getMessage());
    }

    public static void debug(String message) {
        // Debug is only printed when debug mode is enabled
        if (DebugFlag.ENABLED) {
            logger.info("[DEBUG] " + message);
        }
    }

    /**
     * Simple static debug flag that can be toggled at runtime.
     */
    public static final class DebugFlag {
        public static volatile boolean ENABLED = false;
        private DebugFlag() {}
    }
}
