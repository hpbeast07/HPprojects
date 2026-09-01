package com.hpmcpe.serverrules.core.storage;

import com.google.gson.*;
import com.google.gson.reflect.TypeToken;
import com.hpmcpe.serverrules.api.model.PlayerAcceptance;
import com.hpmcpe.serverrules.core.manager.DefaultAcceptanceManager;
import com.hpmcpe.serverrules.core.util.SRLogger;

import java.io.*;
import java.lang.reflect.Type;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.time.Instant;
import java.util.*;

/**
 * Persists player acceptance records as JSON in
 * {@code plugins/ServerRules/data/players.json}.
 *
 * <p>Format:</p>
 * <pre>
 * [
 *   {
 *     "uuid": "...",
 *     "name": "PlayerName",
 *     "version": 1,
 *     "acceptedAt": "2024-01-01T00:00:00Z"
 *   }
 * ]
 * </pre>
 */
public class JsonPlayerStorage implements DefaultAcceptanceManager.StorageDelegate {

    private static final Gson GSON = new GsonBuilder()
            .setPrettyPrinting()
            .disableHtmlEscaping()
            .create();

    private final Path storageFile;

    public JsonPlayerStorage(Path dataFolder) {
        this.storageFile = dataFolder.resolve("players.json");
    }

    @Override
    public void save(Map<UUID, PlayerAcceptance> data) {
        List<JsonObject> list = new ArrayList<>();
        for (PlayerAcceptance acceptance : data.values()) {
            JsonObject obj = new JsonObject();
            obj.addProperty("uuid", acceptance.getPlayerUuid().toString());
            obj.addProperty("name", acceptance.getPlayerName());
            obj.addProperty("version", acceptance.getRulesVersion());
            obj.addProperty("acceptedAt", acceptance.getAcceptedAt().toString());
            list.add(obj);
        }

        try {
            Files.createDirectories(storageFile.getParent());
            try (Writer writer = new OutputStreamWriter(
                    Files.newOutputStream(storageFile, StandardOpenOption.CREATE,
                            StandardOpenOption.TRUNCATE_EXISTING),
                    StandardCharsets.UTF_8)) {
                GSON.toJson(list, writer);
            }
        } catch (IOException e) {
            SRLogger.severe("Failed to save player data: " + e.getMessage(), e);
        }
    }

    @Override
    public Map<UUID, PlayerAcceptance> load() {
        Map<UUID, PlayerAcceptance> result = new HashMap<>();
        if (!Files.exists(storageFile)) return result;

        try (Reader reader = new InputStreamReader(
                Files.newInputStream(storageFile), StandardCharsets.UTF_8)) {
            Type listType = new TypeToken<List<JsonObject>>() {}.getType();
            List<JsonObject> list = GSON.fromJson(reader, listType);
            if (list == null) return result;

            for (JsonObject obj : list) {
                try {
                    UUID uuid = UUID.fromString(obj.get("uuid").getAsString());
                    String name = obj.get("name").getAsString();
                    int version = obj.get("version").getAsInt();
                    Instant acceptedAt = Instant.parse(obj.get("acceptedAt").getAsString());
                    result.put(uuid, new PlayerAcceptance(uuid, name, version, acceptedAt));
                } catch (Exception e) {
                    SRLogger.warning("Skipping malformed player record: " + e.getMessage());
                }
            }
        } catch (IOException e) {
            SRLogger.severe("Failed to load player data: " + e.getMessage(), e);
        }

        return result;
    }
}
