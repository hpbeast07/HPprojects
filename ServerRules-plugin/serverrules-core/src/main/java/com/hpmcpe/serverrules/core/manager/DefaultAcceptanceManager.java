package com.hpmcpe.serverrules.core.manager;

import com.hpmcpe.serverrules.api.manager.AcceptanceManager;
import com.hpmcpe.serverrules.api.model.PlayerAcceptance;

import java.time.Instant;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Supplier;
import java.util.stream.Collectors;

/**
 * Default thread-safe implementation of {@link AcceptanceManager}.
 *
 * <p>Delegates persistence to an injected {@link StorageDelegate}, which
 * allows the Paper plugin to provide JSON or future SQL storage without
 * changing this class.</p>
 */
public class DefaultAcceptanceManager implements AcceptanceManager {

    private final Map<UUID, PlayerAcceptance> acceptances = new ConcurrentHashMap<>();
    private final Supplier<Integer> currentVersionSupplier;
    private final StorageDelegate storageDelegate;

    public DefaultAcceptanceManager(Supplier<Integer> currentVersionSupplier,
                                    StorageDelegate storageDelegate) {
        this.currentVersionSupplier = currentVersionSupplier;
        this.storageDelegate = storageDelegate;
    }

    @Override
    public boolean hasAccepted(UUID uuid) {
        PlayerAcceptance record = acceptances.get(uuid);
        if (record == null) return false;
        return record.getRulesVersion() >= currentVersionSupplier.get();
    }

    @Override
    public boolean hasEverAccepted(UUID uuid) {
        return acceptances.containsKey(uuid);
    }

    @Override
    public Optional<PlayerAcceptance> getAcceptance(UUID uuid) {
        return Optional.ofNullable(acceptances.get(uuid));
    }

    @Override
    public void setAccepted(UUID uuid, String playerName) {
        PlayerAcceptance record = new PlayerAcceptance(
                uuid, playerName, currentVersionSupplier.get(), Instant.now());
        acceptances.put(uuid, record);
        save();
    }

    @Override
    public boolean resetAcceptance(UUID uuid) {
        boolean removed = acceptances.remove(uuid) != null;
        if (removed) save();
        return removed;
    }

    @Override
    public Collection<PlayerAcceptance> getAllAcceptances() {
        return Collections.unmodifiableCollection(acceptances.values());
    }

    @Override
    public int getAcceptedCount() {
        int currentVersion = currentVersionSupplier.get();
        return (int) acceptances.values().stream()
                .filter(a -> a.getRulesVersion() >= currentVersion)
                .count();
    }

    @Override
    public void save() {
        storageDelegate.save(acceptances);
    }

    @Override
    public void load() {
        Map<UUID, PlayerAcceptance> loaded = storageDelegate.load();
        acceptances.clear();
        acceptances.putAll(loaded);
    }

    // -------------------------------------------------------------------------
    // Storage delegate interface
    // -------------------------------------------------------------------------

    /**
     * Pluggable persistence strategy for acceptance records.
     */
    public interface StorageDelegate {
        void save(Map<UUID, PlayerAcceptance> data);
        Map<UUID, PlayerAcceptance> load();
    }
}
