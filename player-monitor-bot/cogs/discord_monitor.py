import discord
from discord.ext import commands
import config
from services.discord_embed_builder import build_status_embed, build_voice_embed, build_profile_update_embed
from database.db import record_status_change, record_vc_join, record_vc_leave, record_profile_change

class DiscordMonitorCog(commands.Cog):
    def __init__(self, bot: commands.Bot):
        self.bot = bot
        self.last_status_cache = {}  # user_id -> status

    @commands.Cog.listener()
    async def on_presence_update(self, before: discord.Member, after: discord.Member):
        if after.bot:
            return

        if not config.DISCORD_MONITOR_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.DISCORD_MONITOR_CHANNEL_ID)
        if not channel:
            return

        old_status = before.status if before else discord.Status.offline
        new_status = after.status

        # Deduplicate identical status spam
        cached = self.last_status_cache.get(after.id)
        if old_status == new_status and cached == new_status:
            return

        self.last_status_cache[after.id] = new_status

        # Record counter in SQLite DB
        member_stats = record_status_change(str(after.id), after.name)

        try:
            embed = build_status_embed(after, old_status, new_status, member_stats=member_stats)
            await channel.send(embed=embed)
        except Exception as e:
            print(f"[Presence Update] Error sending embed: {e}")

    @commands.Cog.listener()
    async def on_voice_state_update(self, member: discord.Member, before: discord.VoiceState, after: discord.VoiceState):
        if member.bot:
            return

        if not config.DISCORD_MONITOR_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.DISCORD_MONITOR_CHANNEL_ID)
        if not channel:
            return

        old_channel = before.channel
        new_channel = after.channel

        try:
            # 1. Joined VC
            if not old_channel and new_channel:
                member_stats = record_vc_join(str(member.id), member.name)
                embed = build_voice_embed("join", member, member_stats=member_stats, channel=new_channel)
                await channel.send(embed=embed)

            # 2. Left VC
            elif old_channel and not new_channel:
                member_stats, session_seconds = record_vc_leave(str(member.id), member.name)
                embed = build_voice_embed("leave", member, member_stats=member_stats, channel=old_channel, session_seconds=session_seconds)
                await channel.send(embed=embed)

            # 3. Switched VC
            elif old_channel and new_channel and old_channel.id != new_channel.id:
                embed = build_voice_embed("move", member, old_channel=old_channel, new_channel=new_channel)
                await channel.send(embed=embed)

            # 4. Stream / Screen Share Start/Stop
            elif not before.self_stream and after.self_stream and new_channel:
                embed = build_voice_embed("stream_start", member, channel=new_channel)
                await channel.send(embed=embed)
            elif before.self_stream and not after.self_stream and new_channel:
                embed = build_voice_embed("stream_stop", member, channel=new_channel)
                await channel.send(embed=embed)
        except Exception as e:
            print(f"[Voice State Update] Error sending embed: {e}")

    @commands.Cog.listener()
    async def on_member_update(self, before: discord.Member, after: discord.Member):
        if after.bot:
            return

        if not config.DISCORD_MONITOR_CHANNEL_ID:
            return

        channel = self.bot.get_channel(config.DISCORD_MONITOR_CHANNEL_ID)
        if not channel:
            return

        try:
            # 1. Nickname Change
            if before.nick != after.nick:
                member_stats = record_profile_change(str(after.id), after.name)
                embed = build_profile_update_embed(
                    after,
                    change_type="Nickname",
                    before_val=before.nick or before.name,
                    after_val=after.nick or after.name,
                    member_stats=member_stats
                )
                await channel.send(embed=embed)

            # 2. Roles Added/Removed
            elif before.roles != after.roles:
                added_roles = [r.name for r in after.roles if r not in before.roles]
                removed_roles = [r.name for r in before.roles if r not in after.roles]
                
                if added_roles or removed_roles:
                    member_stats = record_profile_change(str(after.id), after.name)
                    details_before = f"Roles removed: {', '.join(removed_roles)}" if removed_roles else "No roles removed"
                    details_after = f"Roles added: {', '.join(added_roles)}" if added_roles else "No roles added"
                    embed = build_profile_update_embed(
                        after,
                        change_type="Roles",
                        before_val=details_before,
                        after_val=details_after,
                        member_stats=member_stats
                    )
                    await channel.send(embed=embed)
        except Exception as e:
            print(f"[Member Update] Error sending embed: {e}")

async def setup(bot: commands.Bot):
    await bot.add_cog(DiscordMonitorCog(bot))
