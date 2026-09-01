package com.hpmcpe.serverrules.api.model;

import java.time.Instant;
import java.util.Objects;
import java.util.UUID;

/**
 * Records a player's acceptance of the server rules at a specific version.
 */
public final class PlayerAcceptance {

    private final UUID playerUuid;
    private final String playerName;
    private final int rulesVersion;
    private final Instant acceptedAt;

    public PlayerAcceptance(UUID playerUuid, String playerName, int rulesVersion, Instant acceptedAt) {
        this.playerUuid = Objects.requireNonNull(playerUuid, "playerUuid");
        this.playerName = Objects.requireNonNull(playerName, "playerName");
        this.rulesVersion = rulesVersion;
        this.acceptedAt = Objects.requireNonNull(acceptedAt, "acceptedAt");
    }

    /** @return the player's unique identifier */
    public UUID getPlayerUuid() { return playerUuid; }

    /** @return last known player name at time of acceptance */
    public String getPlayerName() { return playerName; }

    /** @return the rules-version that was accepted */
    public int getRulesVersion() { return rulesVersion; }

    /** @return when the player accepted the rules */
    public Instant getAcceptedAt() { return acceptedAt; }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof PlayerAcceptance that)) return false;
        return Objects.equals(playerUuid, that.playerUuid);
    }

    @Override
    public int hashCode() { return Objects.hash(playerUuid); }

    @Override
    public String toString() {
        return "PlayerAcceptance{uuid=" + playerUuid + ", version=" + rulesVersion + ", at=" + acceptedAt + "}";
    }
}
