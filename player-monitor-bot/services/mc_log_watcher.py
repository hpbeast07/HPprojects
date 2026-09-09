import os
import re
import asyncio
import time
from typing import Callable, Optional
from database.db import record_player_join, record_player_leave

KNOWN_MOBS = [
    "zombie", "skeleton", "creeper", "spider", "enderman", "witch", "slime",
    "phantom", "drowned", "husk", "stray", "warden", "pillager", "vindicator",
    "evoker", "ravager", "piglin", "hoglin", "blaze", "ghast", "wither skeleton",
    "ender dragon", "wither", "guardian", "elder guardian", "iron golem", "wolf"
]

class MinecraftLogWatcher:
    def __init__(self, log_path: str):
        self.log_path = os.path.abspath(log_path)
        self.current_pos = 0
        self.pending_uuids = {}  # username -> uuid
        self.is_running = False

        # Callbacks
        self.on_join: Optional[Callable] = None
        self.on_leave: Optional[Callable] = None
        self.on_death: Optional[Callable] = None

    async def start(self):
        self.is_running = True
        log_dir = os.path.dirname(self.log_path)
        os.makedirs(log_dir, exist_ok=True)

        if os.path.exists(self.log_path):
            self.current_pos = os.path.getsize(self.log_path)

        print(f"[MC Log Watcher] Monitoring log file at: {self.log_path}")

        while self.is_running:
            try:
                if os.path.exists(self.log_path):
                    size = os.path.getsize(self.log_path)
                    if size < self.current_pos:
                        # File was rotated/truncated (server restart)
                        self.current_pos = 0

                    if size > self.current_pos:
                        with open(self.log_path, "r", encoding="utf-8", errors="ignore") as f:
                            f.seek(self.current_pos)
                            new_data = f.read()
                            self.current_pos = f.tell()

                        lines = new_data.splitlines()
                        for line in lines:
                            if line.strip():
                                await self.parse_line(line.strip())
            except Exception as e:
                # Handle transient file locks gracefully
                pass

            await asyncio.sleep(1.5)

    def stop(self):
        self.is_running = False

    async def parse_line(self, line: str):
        # 1. UUID capture
        uuid_match = re.search(r"UUID of player ([A-Za-z0-9_]{1,16}) is ([a-f0-9-]+)", line, re.IGNORECASE)
        if uuid_match:
            self.pending_uuids[uuid_match.group(1)] = uuid_match.group(2)
            return

        # 2. Player Join
        join_match1 = re.search(r"\[.*(?:INFO|Server thread)\]:\s+([A-Za-z0-9_]{1,16}) joined the game", line, re.IGNORECASE)
        join_match2 = re.search(r"\[.*(?:INFO|Server thread)\]:\s+([A-Za-z0-9_]{1,16})\[.*\] logged in with entity id", line, re.IGNORECASE)
        join_match = join_match1 or join_match2

        if join_match:
            username = join_match.group(1)
            uuid = self.pending_uuids.pop(username, None)
            player_stats = record_player_join(username, uuid)
            timestamp = int(time.time())

            if self.on_join:
                await self.on_join(username, uuid, player_stats, timestamp)
            return

        # 3. Player Leave
        leave_match1 = re.search(r"\[.*(?:INFO|Server thread)\]:\s+([A-Za-z0-9_]{1,16}) left the game", line, re.IGNORECASE)
        leave_match2 = re.search(r"\[.*(?:INFO|Server thread)\]:\s+([A-Za-z0-9_]{1,16}) lost connection:", line, re.IGNORECASE)
        leave_match = leave_match1 or leave_match2

        if leave_match:
            username = leave_match.group(1)
            player_stats, session_seconds = record_player_leave(username)
            timestamp = int(time.time())

            if self.on_leave:
                await self.on_leave(username, player_stats, session_seconds, timestamp)
            return

        # 4. Player Death
        await self.check_death(line)

    async def check_death(self, line: str):
        prefix_match = re.search(r"\[.*(?:INFO|Server thread)\]:\s+(.*)$", line, re.IGNORECASE)
        if not prefix_match:
            return

        message = prefix_match.group(1).strip()

        # Filter non-death chat & login lines
        if (
            message.startswith("<") or
            "joined the game" in message or
            "left the game" in message or
            "lost connection" in message or
            "logged in with" in message or
            "issued server command" in message or
            "Saving the game" in message or
            "UUID of player" in message
        ):
            return

        death_patterns = [
            (
                r"^([A-Za-z0-9_]{1,16})\s+(was slain by|was shot by|was blown up by|was fireballed by|was pummeled by|was killed by)\s+([A-Za-z0-9_]{1,16}(?:\s+using\s+.+)?)",
                lambda m: {
                    "victim": m.group(1),
                    "killer": m.group(3),
                    "is_pvp": not any(mob in m.group(3).lower() for mob in KNOWN_MOBS)
                }
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(fell from a high place|fell off a ladder|fell off some vines|hit the ground too hard|fell while climbing)",
                lambda m: {"victim": m.group(1), "killer": "Gravity / Fall Damage", "is_pvp": False}
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(went up in flames|burned to death|walked into fire|tried to swim in lava|discovered floor was lava)",
                lambda m: {"victim": m.group(1), "killer": "Lava / Fire", "is_pvp": False}
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(drowned|drowned whilst trying to escape.*)",
                lambda m: {"victim": m.group(1), "killer": "Drowning", "is_pvp": False}
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(blew up|was blown up)",
                lambda m: {"victim": m.group(1), "killer": "Explosion", "is_pvp": False}
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(fell into the void|fell out of the world|experienced kinetic energy)",
                lambda m: {"victim": m.group(1), "killer": "The Void / Elytra Impact", "is_pvp": False}
            ),
            (
                r"^([A-Za-z0-9_]{1,16})\s+(withered away|starved to death|suffocated in a wall|was squashed by a falling block|died)",
                lambda m: {"victim": m.group(1), "killer": "Natural Causes / Suffocation", "is_pvp": False}
            ),
        ]

        for regex, handler in death_patterns:
            m = re.search(regex, message, re.IGNORECASE)
            if m:
                details = handler(m)
                details["death_message"] = message
                timestamp = int(time.time())

                if self.on_death:
                    await self.on_death(details, timestamp)
                return
