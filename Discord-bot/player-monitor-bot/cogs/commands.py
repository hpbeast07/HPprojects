import os
import time
import asyncio
import tracemalloc
import platform
from datetime import datetime, timezone
import discord
from discord import app_commands
from discord.ext import commands
import config
from database.db import get_player, get_top_players, get_discord_member, format_duration, get_database_summary
from services.mc_status import fetch_mc_status

class GeneralCommandsCog(commands.Cog):
    def __init__(self, bot: commands.Bot):
        self.bot = bot
        self.start_time = time.time()

    # 1. /mcstats
    @app_commands.command(name="mcstats", description="View Minecraft player profile, total playtime, and 3D skin")
    @app_commands.describe(player="Minecraft in-game username")
    async def mcstats(self, interaction: discord.Interaction, player: str):
        player_stats = get_player(player)
        if not player_stats:
            await interaction.response.send_message(
                f"❌ Player **{player}** has not been recorded on this server yet!",
                ephemeral=True
            )
            return

        is_online = bool(player_stats.get("current_session_start"))
        current_session = 0
        if is_online:
            current_session = int(time.time()) - player_stats["current_session_start"]

        total_seconds = (player_stats.get("total_playtime_seconds") or 0) + current_session
        playtime_formatted = format_duration(total_seconds)
        avatar_url = f"https://mc-heads.net/avatar/{player_stats['username']}/128"
        body_render_url = f"https://mc-heads.net/body/{player_stats['username']}/right"

        embed = discord.Embed(
            title=f"🎮 Minecraft Player Profile: {player_stats['username']}",
            color=config.COLOR_MC_JOIN if is_online else config.COLOR_PRIMARY
        )
        embed.set_thumbnail(url=avatar_url)
        embed.set_image(url=body_render_url)
        embed.add_field(name="👤 Username", value=f"`{player_stats['username']}`", inline=True)
        embed.add_field(name="📡 Current Status", value="🟢 **Online Now**" if is_online else "⚫ **Offline**", inline=True)
        embed.add_field(name="🔢 Total Joins", value=f"**{player_stats.get('join_count', 1)}** time(s)", inline=True)
        embed.add_field(name="⏳ Total Playtime", value=f"`{playtime_formatted}`", inline=True)
        
        first_seen = player_stats.get("first_seen")
        last_seen = player_stats.get("last_seen")
        embed.add_field(name="🌟 First Joined", value=f"<t:{first_seen}:F>" if first_seen else "Unknown", inline=True)
        embed.add_field(name="⏰ Last Seen", value=f"<t:{last_seen}:R>" if last_seen else "Unknown", inline=True)
        embed.set_footer(
            text="Minecraft Server Logs System",
            icon_url="https://cdn.iconscout.com/icon/free/png-256/free-minecraft-logo-icon-download-in-svg-png-gif-file-formats--brand-social-media-pack-logos-icons-2661559.png"
        )
        await interaction.response.send_message(embed=embed)

    # 2. /mcleaderboard
    @app_commands.command(name="mcleaderboard", description="View top Minecraft players by playtime or total joins")
    @app_commands.choices(sort=[
        app_commands.Choice(name="⏳ Playtime (Default)", value="playtime"),
        app_commands.Choice(name="🔢 Total Joins", value="joins")
    ])
    async def mcleaderboard(self, interaction: discord.Interaction, sort: app_commands.Choice[str] = None):
        sort_by = sort.value if sort else "playtime"
        top_players = get_top_players(10, sort_by)

        if not top_players:
            await interaction.response.send_message("📊 No player logs recorded yet.", ephemeral=True)
            return

        medals = ["🥇", "🥈", "🥉", "4️⃣", "5️⃣", "6️⃣", "7️⃣", "8️⃣", "9️⃣", "🔟"]
        lines = []
        for idx, p in enumerate(top_players):
            medal = medals[idx] if idx < len(medals) else f"#{idx+1}"
            pt = format_duration(p.get("total_playtime_seconds", 0))
            joins = p.get("join_count", 0)
            lines.append(f"{medal} **{p['username']}** — ⏳ `{pt}` | 🔢 **{joins} joins**")

        embed = discord.Embed(
            title="🏆 Minecraft Server Top Players Leaderboard",
            description="\n\n".join(lines),
            color=config.COLOR_GOLD
        )
        embed.set_footer(text=f"Sorted by {'Total Joins' if sort_by == 'joins' else 'Playtime'} • Minecraft Logs")
        await interaction.response.send_message(embed=embed)

    # 3. /discordstats
    @app_commands.command(name="discordstats", description="View Discord user monitor stats and voice activity")
    @app_commands.describe(user="Select a Discord member (defaults to you)")
    async def discordstats(self, interaction: discord.Interaction, user: discord.Member = None):
        target = user or interaction.user
        member_stats = get_discord_member(str(target.id))
        total_vc = member_stats.get("total_vc_seconds", 0) if member_stats else 0
        vc_formatted = format_duration(total_vc)
        status_changes = member_stats.get("total_status_changes", 0) if member_stats else 0
        vc_visits = member_stats.get("total_vc_joins", 0) if member_stats else 0
        profile_changes = member_stats.get("total_profile_changes", 0) if member_stats else 0

        status = target.status if hasattr(target, "status") else discord.Status.offline
        status_map = {
            discord.Status.online: "🟢 Online",
            discord.Status.idle: "🟡 Idle",
            discord.Status.dnd: "🔴 Do Not Disturb",
            discord.Status.do_not_disturb: "🔴 Do Not Disturb",
            discord.Status.offline: "⚫ Offline"
        }

        vc_name = f"<#{target.voice.channel.id}>" if getattr(target, "voice", None) and target.voice.channel else "`Not in VC`"

        embed = discord.Embed(
            title=f"👥 Discord Member Stats: {target.name}",
            color=config.COLOR_PRIMARY
        )
        embed.set_thumbnail(url=target.display_avatar.url)
        embed.add_field(name="👤 User", value=f"{target.mention} (`{target.id}`)", inline=True)
        embed.add_field(name="📡 Current Status", value=status_map.get(status, "⚫ Offline"), inline=True)
        embed.add_field(name="🔊 Voice Channel", value=vc_name, inline=True)
        embed.add_field(name="🔄 Total Status Updates", value=f"**#{status_changes}**", inline=True)
        embed.add_field(name="🔢 Total VC Visits", value=f"**#{vc_visits}**", inline=True)
        embed.add_field(name="⏱️ Tracked VC Time", value=f"`{vc_formatted}`", inline=True)
        embed.add_field(name="📝 Total Profile Updates", value=f"**#{profile_changes}**", inline=True)
        embed.add_field(name="📅 Account Created", value=f"<t:{int(target.created_at.timestamp())}:R>", inline=True)
        if hasattr(target, "joined_at") and target.joined_at:
            embed.add_field(name="📥 Server Joined", value=f"<t:{int(target.joined_at.timestamp())}:R>", inline=True)
        embed.set_footer(text="Discord Activity Monitor System", icon_url=target.display_avatar.url)
        await interaction.response.send_message(embed=embed)

    # 4. /status
    @app_commands.command(name="status", description="Check bot health and system metrics")
    async def status(self, interaction: discord.Interaction):
        # 1. Event loop delay
        t_start = time.perf_counter()
        await asyncio.sleep(0)
        loop_delay_ms = round((time.perf_counter() - t_start) * 1000, 2)

        # 2. Websocket Ping
        ping_ms = round(self.bot.latency * 1000)

        # 3. Memory metrics (tracemalloc)
        if not tracemalloc.is_tracing():
            tracemalloc.start()
        current_mem, peak_mem = tracemalloc.get_traced_memory()
        current_mb = current_mem / (1024 * 1024)
        peak_mb = peak_mem / (1024 * 1024)
        memory_str = f"Python memory: {current_mb:.2f} MB | Peak: {peak_mb:.2f} MB"

        # 4. Uptime
        uptime_sec = int(time.time() - self.start_time)
        uptime_formatted = format_duration(uptime_sec)

        # 5. Guilds & Members count
        guilds_count = len(self.bot.guilds)
        total_members = sum(g.member_count or 0 for g in self.bot.guilds)

        # 6. Versions
        discord_version = f"discord.py {discord.__version__}"
        python_version = platform.python_version()

        # 7. Local Time UTC
        local_time_utc = datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S UTC")

        # Build clean aesthetic embed
        embed = discord.Embed(
            title="🤖 Bot Health",
            color=config.COLOR_PRIMARY
        )
        embed.add_field(name="📡 Websocket Ping", value=f"{ping_ms} ms", inline=True)
        embed.add_field(name="⚡ Event Loop Delay", value=f"{loop_delay_ms} ms", inline=True)
        embed.add_field(name="🧠 Memory", value=memory_str, inline=False)
        embed.add_field(name="⏱️ Uptime", value=uptime_formatted, inline=True)
        embed.add_field(name="🖥️ Guilds", value=str(guilds_count), inline=True)
        embed.add_field(name="👥 Members", value=str(total_members), inline=True)
        embed.add_field(name="🧩 Version", value=discord_version, inline=True)
        embed.add_field(name="🐍 Python", value=python_version, inline=True)
        embed.add_field(name="🕒 Local Time", value=local_time_utc, inline=False)

        await interaction.response.send_message(embed=embed, ephemeral=True)


    # 5. /mcstatus
    @app_commands.command(name="mcstatus", description="Check the live status of the Minecraft server")
    async def mcstatus(self, interaction: discord.Interaction):
        if not config.MC_SERVER_ADDRESS:
            await interaction.response.send_message(
                "⚠️ **MC_SERVER_ADDRESS** `.env` file me set nahi hai. Server address add karke bot restart karein.",
                ephemeral=True,
            )
            return

        await interaction.response.defer()

        data = await fetch_mc_status(config.MC_SERVER_ADDRESS, config.MC_SERVER_PORT)

        if not data["online"]:
            embed = discord.Embed(
                title="🔴 Minecraft Server Offline",
                description=f"Server **`{data['host']}:{data['port']}`** se abhi connect nahi ho paya.",
                color=config.COLOR_MC_LEAVE,
            )
            embed.add_field(name="🔌 Address", value=f"`{data['host']}:{data['port']}`", inline=True)
            embed.add_field(name="⚠️ Error", value=f"```{data.get('error', 'Connection failed')}```", inline=False)
            embed.set_footer(text="Minecraft Server Status • Try again later")
            await interaction.followup.send(embed=embed)
            return

        latency = data["latency_ms"]
        latency_icon = "🟢" if latency < 100 else ("🟡" if latency < 200 else "🔴")

        embed = discord.Embed(
            title="🟢 Minecraft Server Online",
            description=f"> {data['motd']}" if data["motd"] else "Minecraft Server",
            color=config.COLOR_MC_JOIN,
        )
        embed.add_field(name="🔌 Address", value=f"`{data['host']}:{data['port']}`", inline=True)
        embed.add_field(name="🧩 Version", value=f"`{data['version']}`", inline=True)
        embed.add_field(name="📡 Latency", value=f"{latency_icon} **{latency} ms**", inline=True)
        embed.add_field(name="👥 Players", value=f"**{data['players_online']}** / **{data['players_max']}**", inline=True)

        player_list = data.get("players") or []
        if player_list:
            shown = ", ".join(f"`{name}`" for name in player_list[:25])
            if len(player_list) > 25:
                shown += f" … +{len(player_list) - 25} more"
            embed.add_field(name="🕹️ Online Players", value=shown, inline=False)
        else:
            embed.add_field(
                name="🕹️ Online Players",
                value="Player list unavailable (server me `enable-query=true` enable karein)",
                inline=False,
            )

        embed.set_thumbnail(
            url="https://cdn.iconscout.com/icon/free/png-256/free-minecraft-logo-icon-download-in-svg-png-gif-file-formats--brand-social-media-pack-logos-icons-2661559.png"
        )
        embed.set_footer(text="Minecraft Server Status • Live Query")
        await interaction.followup.send(embed=embed)


    # 6. /help
    @app_commands.command(name="help", description="Displays information on bot commands and setup guide")
    async def help(self, interaction: discord.Interaction):
        embed = discord.Embed(
            title="📖 Minecraft & Discord Monitor Bot — Guide",
            description="Yeh bot Minecraft server ke logs aur Discord members ki activity ko dedicated channels me record karta hai.",
            color=config.COLOR_PRIMARY
        )
        embed.add_field(
            name="🎮 Minecraft Features",
            value="• **Join / Leave Logs**: Player skin head, join counter, timestamps, current session aur total playtime.\n"
                  "• **Death Logs**: Player death causes, Mob kills aur PvP Combat kills dedicated channel me.",
            inline=False
        )
        embed.add_field(
            name="👥 Discord Activity Monitor",
            value="• **Member Status**: 🟢 Online, 🟡 Idle, 🔴 DND, ⚫ Offline + Total Status Changes Counter.\n"
                  "• **Voice Channels**: VC Join, Leave (with duration), Move, Screen share + Total VC Visits Counter.\n"
                  "• **Profile Changes**: Nickname & Role changes tracking.",
            inline=False
        )
        embed.add_field(
            name="⚡ Slash Commands",
            value="`/mcstats <player>` — Minecraft player stats & skin.\n"
                  "`/mcleaderboard [sort]` — Top players leaderboard.\n"
                  "`/discordstats [user]` — Member monitor profile & VC time.\n"
                  "`/status` — Bot latency & system diagnostics.\n"
                  "`/mcstatus` — Minecraft server live status & players.\n"
                  "`/help` — Command guide.",
            inline=False
        )
        await interaction.response.send_message(embed=embed, ephemeral=True)

async def setup(bot: commands.Bot):
    await bot.add_cog(GeneralCommandsCog(bot))
