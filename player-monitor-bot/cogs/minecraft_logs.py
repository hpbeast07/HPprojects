import discord
from discord.ext import commands
import asyncio
import config
from services.mc_log_watcher import MinecraftLogWatcher
from services.mc_embed_builder import build_join_embed, build_leave_embed, build_death_embed

class MinecraftLogsCog(commands.Cog):
    def __init__(self, bot: commands.Bot):
        self.bot = bot
        self.watcher = MinecraftLogWatcher(config.MC_LOG_PATH)
        self.watcher_task = None

    async def cog_load(self):
        self.watcher.on_join = self.handle_join
        self.watcher.on_leave = self.handle_leave
        self.watcher.on_death = self.handle_death
        self.watcher_task = asyncio.create_task(self.watcher.start())
        print("[Minecraft Logs Cog] Log watcher background task started.")

    async def cog_unload(self):
        if self.watcher:
            self.watcher.stop()
        if self.watcher_task:
            self.watcher_task.cancel()

    async def handle_join(self, username: str, uuid: str, player_stats: dict, timestamp: int):
        print(f"[MC Event] Player Joined: {username} (Join #{player_stats.get('join_count', 1)})")
        if not config.MC_JOIN_LEAVE_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.MC_JOIN_LEAVE_CHANNEL_ID)
        if not channel:
            return

        try:
            embed = build_join_embed(player_stats, timestamp)
            await channel.send(embed=embed)
        except Exception as e:
            print(f"[MC Event] Error sending Join embed: {e}")

    async def handle_leave(self, username: str, player_stats: dict, session_seconds: int, timestamp: int):
        print(f"[MC Event] Player Left: {username} (Session: {session_seconds}s)")
        if not config.MC_JOIN_LEAVE_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.MC_JOIN_LEAVE_CHANNEL_ID)
        if not channel:
            return

        try:
            embed = build_leave_embed(player_stats, session_seconds, timestamp)
            await channel.send(embed=embed)
        except Exception as e:
            print(f"[MC Event] Error sending Leave embed: {e}")

    async def handle_death(self, death_info: dict, timestamp: int):
        print(f"[MC Event] Player Death: {death_info['victim']} ({death_info.get('killer') or 'Environment'})")
        if not config.MC_DEATH_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.MC_DEATH_CHANNEL_ID)
        if not channel:
            return

        try:
            embed = build_death_embed(death_info, timestamp)
            await channel.send(embed=embed)
        except Exception as e:
            print(f"[MC Event] Error sending Death embed: {e}")

async def setup(bot: commands.Bot):
    await bot.add_cog(MinecraftLogsCog(bot))
