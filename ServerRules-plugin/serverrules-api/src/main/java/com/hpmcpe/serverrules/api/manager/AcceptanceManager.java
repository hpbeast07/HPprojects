package com.hpmcpe.serverrules.api.manager;

import com.hpmcpe.serverrules.api.model.PlayerAcceptance;

import java.util.Collection;
import java.util.Optional;
import java.util.UUID;

/**
 * Manages player rule acceptance records.
 *
 * <p>Tracks whether each player has accepted the current rules version.
 * If the rules version increases, players who accepted an older version
 * are considered "pending" until they re-accept.</p>
 */
public interface AcceptanceManager {

    /**
     * Check whether a player has accepted the current rules version.
     *
     * @param uuid the player UUID
     * @return true if the player has accepted the current version
     */
    boolean hasAccepted(UUID uuid);

    /**
     * Check whether a player has ever accepted any version of the rules.
     *
     * @param uuid the player UUID
     * @return true if any acceptance record exists
     */
    boolean hasEverAccepted(UUID uuid);

    /**
     * Get the full acceptance record for a player.
     *
     * @param uuid the player UUID
     * @return Optional containing the record, or empty if never accepted
     */
    Optional<PlayerAcceptance> getAcceptance(UUID uuid);

    /**
     * Record that a player has accepted the current rules version.
     *
     * @param uuid       the player UUID
     * @param playerName the player's current name
     */
    void setAccepted(UUID uuid, String playerName);

    /**
     * Reset a player's acceptance (requires them to accept again).
     *
     * @param uuid the player UUID
     * @return true if a record was found and removed
     */
    boolean resetAcceptance(UUID uuid);

    /**
     * Get all current acceptance records.
     *
     * @return unmodifiable collection of all records
     */
    Collection<PlayerAcceptance> getAllAcceptances();

    /**
     * Count players who have accepted the current rules version.
     *
     * @return count of accepted players
     */
    int getAcceptedCount();

    /**
     * Save acceptance data to storage.
     */
    void save();

    /**
     * Load acceptance data from storage.
     */
    void load();
}
