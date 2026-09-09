"""
Minecraft Java server status service.

Uses the ``mcstatus`` library to query a Java Edition server over the
Server List Ping protocol (and optionally the Query protocol) to build a
rich status snapshot consumed by the ``/mcstatus`` slash command.
"""
from __future__ import annotations

from typing import Any, Dict

from mcstatus import JavaServer


def _motd_to_plain(motd: Any) -> str:
    """Safely convert a Motd (or raw str/dict MOTD) into plain text."""
    if motd is None:
        return ""

    to_plain = getattr(motd, "to_plain", None)
    if callable(to_plain):
        try:
            return to_plain()
        except Exception:
            pass

    if isinstance(motd, str):
        return motd
    if isinstance(motd, dict):
        text = motd.get("text", "")
        return text if isinstance(text, str) else str(text)
    return str(motd)


async def fetch_mc_status(address: str, port: int = 25565, timeout: float = 5.0) -> Dict[str, Any]:
    """
    Query a Minecraft Java server and return a status dict.

    When the server responds, the dict has ``online: True`` and includes the
    MOTD, version, latency (ms), online/max player counts and the list of
    online player names (when the Query protocol is enabled on the server).
    On any failure a dict with ``online: False`` plus an ``error`` message is
    returned so callers can render a friendly offline embed.
    """
    host = f"{address}:{port}"

    try:
        server = await JavaServer.async_lookup(host, timeout=timeout)
    except Exception as exc:  # DNS / parse failure
        return {"online": False, "host": address, "port": port, "error": str(exc)}

    try:
        status = await server.async_status()
    except Exception as exc:  # unreachable / connection refused / timeout
        return {"online": False, "host": address, "port": port, "error": str(exc)}

    data: Dict[str, Any] = {
        "online": True,
        "host": address,
        "port": port,
        "latency_ms": round(float(status.latency), 1),
        "version": status.version.name,
        "protocol": status.version.protocol,
        "motd": _motd_to_plain(status.motd),
        "players_online": int(status.players.online),
        "players_max": int(status.players.max),
        "players": [],
    }

    # Use the player sample that the status ping advertises (if provided).
    sample = getattr(status.players, "sample", None)
    if sample:
        data["players"] = [p.name for p in sample if p.name]

    # The Query protocol returns the complete player list; use it when enabled
    # (requires `enable-query=true` in the server's server.properties).
    try:
        query = await server.async_query()
        if query.players.list:
            data["players"] = list(query.players.list)
        data["players_online"] = int(query.players.online)
        data["players_max"] = int(query.players.max)
        data["motd"] = _motd_to_plain(query.motd) or data["motd"]
    except Exception:
        pass

    return data
