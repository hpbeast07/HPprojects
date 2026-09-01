package com.hpmcpe.serverrules.core.util;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Central message formatting utility.
 *
 * <p>Supports:</p>
 * <ul>
 *   <li>Legacy {@code &} color codes (e.g. {@code &c}, {@code &l})</li>
 *   <li>Hex colors in {@code &#RRGGBB} format</li>
 *   <li>Hex colors in {@code {#RRGGBB}} format (alternative)</li>
 * </ul>
 *
 * <p>On platforms that support Adventure / MiniMessage, the result string
 * can be passed to the platform adapter for further processing. This class
 * always produces a Bukkit-legacy-format string.</p>
 */
public final class ColorUtil {

    private static final Pattern HEX_PATTERN_AMP   = Pattern.compile("&#([A-Fa-f0-9]{6})");
    private static final Pattern HEX_PATTERN_BRACE = Pattern.compile("\\{#([A-Fa-f0-9]{6})}");
    private static final char    COLOR_CHAR         = '\u00A7'; // §

    private ColorUtil() {}

    /**
     * Translate a string containing {@code &} codes and hex colors into
     * a string with the Minecraft section-sign ({@code §}) color codes.
     *
     * @param input the raw input string, may be {@code null}
     * @return the colorized string, or empty string if input was {@code null}
     */
    public static String colorize(String input) {
        if (input == null || input.isEmpty()) return "";

        // Translate &#RRGGBB → §x§R§R§G§G§B§B
        input = translateHex(input, HEX_PATTERN_AMP);
        // Translate {#RRGGBB} → §x§R§R§G§G§B§B
        input = translateHex(input, HEX_PATTERN_BRACE);
        // Translate &[0-9a-fk-or] → §[0-9a-fk-or]
        input = translateAmpersand(input);

        return input;
    }

    /**
     * Strip all color codes from a string.
     *
     * @param input the colored string
     * @return the plain-text string
     */
    public static String strip(String input) {
        if (input == null) return "";
        return input.replaceAll("(?i)" + COLOR_CHAR + "[0-9a-fk-or]", "")
                    .replaceAll("(?i)&[0-9a-fk-or]", "")
                    .replaceAll("&#[A-Fa-f0-9]{6}", "")
                    .replaceAll("\\{#[A-Fa-f0-9]{6}}", "");
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    private static String translateHex(String input, Pattern pattern) {
        Matcher matcher = pattern.matcher(input);
        StringBuilder sb = new StringBuilder();
        while (matcher.find()) {
            String hex = matcher.group(1);
            StringBuilder replacement = new StringBuilder(COLOR_CHAR + "x");
            for (char c : hex.toCharArray()) {
                replacement.append(COLOR_CHAR).append(Character.toLowerCase(c));
            }
            matcher.appendReplacement(sb, replacement.toString());
        }
        matcher.appendTail(sb);
        return sb.toString();
    }

    private static String translateAmpersand(String input) {
        char[] b = input.toCharArray();
        for (int i = 0; i < b.length - 1; i++) {
            if (b[i] == '&' && "0123456789AaBbCcDdEeFfKkLlMmNnOoRrXx".indexOf(b[i + 1]) > -1) {
                b[i] = COLOR_CHAR;
                b[i + 1] = Character.toLowerCase(b[i + 1]);
            }
        }
        return new String(b);
    }
}
