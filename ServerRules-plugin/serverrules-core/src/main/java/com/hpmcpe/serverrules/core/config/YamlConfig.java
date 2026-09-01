package com.hpmcpe.serverrules.core.config;

import com.hpmcpe.serverrules.core.util.SRLogger;
import org.yaml.snakeyaml.Yaml;
import org.yaml.snakeyaml.constructor.SafeConstructor;
import org.yaml.snakeyaml.LoaderOptions;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;

/**
 * A lightweight YAML configuration wrapper backed by SnakeYAML.
 *
 * <p>Provides a convenient API for reading nested keys using dot-notation
 * (e.g. {@code "acceptance.enabled"}) and returning typed values with
 * sensible defaults.</p>
 *
 * <p>This class does NOT write back to disk; saving is handled by the
 * platform-specific config managers.</p>
 */
public class YamlConfig {

    private final Map<String, Object> data;
    private final Path filePath;

    protected YamlConfig(Map<String, Object> data, Path filePath) {
        this.data = data;
        this.filePath = filePath;
    }

    /**
     * Load a YAML file from disk.
     *
     * @param path the path to the YAML file
     * @return a loaded YamlConfig instance
     * @throws IOException if the file cannot be read
     */
    public static YamlConfig load(Path path) throws IOException {
        LoaderOptions options = new LoaderOptions();
        Yaml yaml = new Yaml(new SafeConstructor(options));
        try (InputStream in = Files.newInputStream(path);
             Reader reader = new InputStreamReader(in, StandardCharsets.UTF_8)) {
            Map<String, Object> raw = yaml.load(reader);
            return new YamlConfig(raw == null ? new LinkedHashMap<>() : raw, path);
        }
    }

    /**
     * Load a YAML file from a classpath resource stream.
     *
     * @param stream the input stream (e.g. from getClass().getResourceAsStream())
     * @return a loaded YamlConfig instance
     */
    public static YamlConfig loadFromStream(InputStream stream) {
        if (stream == null) return new YamlConfig(new LinkedHashMap<>(), null);
        LoaderOptions options = new LoaderOptions();
        Yaml yaml = new Yaml(new SafeConstructor(options));
        try (Reader reader = new InputStreamReader(stream, StandardCharsets.UTF_8)) {
            Map<String, Object> raw = yaml.load(reader);
            return new YamlConfig(raw == null ? new LinkedHashMap<>() : raw, null);
        } catch (IOException e) {
            SRLogger.warning("Failed to load YAML from stream: " + e.getMessage());
            return new YamlConfig(new LinkedHashMap<>(), null);
        }
    }

    /** @return raw data map (unmodifiable) */
    public Map<String, Object> getRaw() { return Collections.unmodifiableMap(data); }

    /** @return the file path this config was loaded from, or null if loaded from stream */
    public Path getFilePath() { return filePath; }

    // -------------------------------------------------------------------------
    // Typed getters with dot-notation key support
    // -------------------------------------------------------------------------

    /**
     * Get a value at the given dot-notation path.
     *
     * @param key the dot-separated key path
     * @return the raw value, or null if not found
     */
    @SuppressWarnings("unchecked")
    public Object get(String key) {
        String[] parts = key.split("\\.");
        Map<String, Object> current = data;
        for (int i = 0; i < parts.length - 1; i++) {
            Object next = current.get(parts[i]);
            if (!(next instanceof Map)) return null;
            current = (Map<String, Object>) next;
        }
        return current.get(parts[parts.length - 1]);
    }

    public String getString(String key, String defaultValue) {
        Object val = get(key);
        return val != null ? String.valueOf(val) : defaultValue;
    }

    public String getString(String key) { return getString(key, null); }

    public boolean getBoolean(String key, boolean defaultValue) {
        Object val = get(key);
        if (val instanceof Boolean b) return b;
        if (val instanceof String s) return Boolean.parseBoolean(s);
        return defaultValue;
    }

    public int getInt(String key, int defaultValue) {
        Object val = get(key);
        if (val instanceof Number n) return n.intValue();
        if (val instanceof String s) {
            try { return Integer.parseInt(s); } catch (NumberFormatException ignored) {}
        }
        return defaultValue;
    }

    public long getLong(String key, long defaultValue) {
        Object val = get(key);
        if (val instanceof Number n) return n.longValue();
        return defaultValue;
    }

    public double getDouble(String key, double defaultValue) {
        Object val = get(key);
        if (val instanceof Number n) return n.doubleValue();
        return defaultValue;
    }

    @SuppressWarnings("unchecked")
    public List<String> getStringList(String key) {
        Object val = get(key);
        if (val instanceof List<?> list) {
            List<String> result = new ArrayList<>();
            for (Object item : list) result.add(String.valueOf(item));
            return result;
        }
        return new ArrayList<>();
    }

    /**
     * Get a sub-section as a raw map.
     *
     * @param key the dot-notation key
     * @return the sub-map, or an empty map if not found or not a map
     */
    @SuppressWarnings("unchecked")
    public Map<String, Object> getSection(String key) {
        Object val = get(key);
        if (val instanceof Map<?, ?> map) {
            return (Map<String, Object>) map;
        }
        return new LinkedHashMap<>();
    }

    /**
     * Get the keys of the top-level map, or of a sub-section.
     *
     * @param key dot-notation path, or null for the root
     * @return set of keys
     */
    @SuppressWarnings("unchecked")
    public Set<String> getKeys(String key) {
        if (key == null || key.isBlank()) return data.keySet();
        Object section = get(key);
        if (section instanceof Map<?, ?> map) {
            return ((Map<String, Object>) map).keySet();
        }
        return new LinkedHashSet<>();
    }

    public boolean contains(String key) { return get(key) != null; }

    @Override
    public String toString() {
        return "YamlConfig{file=" + filePath + ", keys=" + data.keySet() + "}";
    }

    // -------------------------------------------------------------------------
    // SectionView — wraps an existing Map as a YamlConfig for sub-sections
    // -------------------------------------------------------------------------

    /**
     * A lightweight view over an already-parsed {@code Map<String, Object>}.
     * Used internally to parse sub-sections without re-reading the file.
     */
    public static final class SectionView extends YamlConfig {
        public SectionView(Map<String, Object> data) {
            super(data == null ? new LinkedHashMap<>() : data, null);
        }
    }
}
