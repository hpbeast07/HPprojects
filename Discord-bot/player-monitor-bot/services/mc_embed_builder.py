import discord
from datetime import datetime
import config
from database.db import format_duration

def build_join_embed(player_stats: dict, timestamp: int = None) -> discord.Embed:
    if timestamp is None:
        import time
        timestamp = int(time.time())

    username = player_stats["username"]
    join_count = player_stats.get("join_count", 1)
    total_playtime = format_duration(player_stats.get("total_playtime_seconds", 0))
    first_seen = player_stats.get("first_seen", timestamp)
    avatar_url = f"https://mc-heads.net/avatar/{username}/128"

    embed = discord.Embed(
        title="🟢 Player Joined the Server",
        color=config.COLOR_MC_JOIN,
        timestamp=datetime.fromtimestamp(timestamp)
    )
    embed.set_thumbnail(url=avatar_url)
    embed.add_field(name="👤 Player", value=f"```{username}```", inline=True)
    embed.add_field(name="🔢 Join Count", value=f"**#{join_count}** time{'s' if join_count != 1 else ''}", inline=True)
    embed.add_field(name="⏳ Total Playtime", value=f"`{total_playtime}`", inline=True)
    embed.add_field(name="⏰ Join Time", value=f"<t:{timestamp}:T> • <t:{timestamp}:R>", inline=True)
    embed.add_field(name="📅 Date", value=f"<t:{timestamp}:D>", inline=True)
    embed.add_field(name="🌟 First Seen", value=f"<t:{first_seen}:d>", inline=True)
    embed.set_footer(
        text="Minecraft Server Logs • Player Connected",
        icon_url="https://cdn.iconscout.com/icon/free/png-256/free-minecraft-logo-icon-download-in-svg-png-gif-file-formats--brand-social-media-pack-logos-icons-2661559.png"
    )
    return embed

def build_leave_embed(player_stats: dict, session_seconds: int, timestamp: int = None) -> discord.Embed:
    if timestamp is None:
        import time
        timestamp = int(time.time())

    username = player_stats["username"]
    session_formatted = format_duration(session_seconds)
    total_playtime = format_duration(player_stats.get("total_playtime_seconds", 0))
    join_count = player_stats.get("join_count", 1)
    avatar_url = f"https://mc-heads.net/avatar/{username}/128"

    embed = discord.Embed(
        title="🔴 Player Left the Server",
        color=config.COLOR_MC_LEAVE,
        timestamp=datetime.fromtimestamp(timestamp)
    )
    embed.set_thumbnail(url=avatar_url)
    embed.add_field(name="👤 Player", value=f"```{username}```", inline=True)
    embed.add_field(name="⏱️ Current Session", value=f"**{session_formatted}**", inline=True)
    embed.add_field(name="⏳ Total Playtime", value=f"`{total_playtime}`", inline=True)
    embed.add_field(name="⏰ Leave Time", value=f"<t:{timestamp}:T> • <t:{timestamp}:R>", inline=True)
    embed.add_field(name="📅 Date", value=f"<t:{timestamp}:D>", inline=True)
    embed.add_field(name="🔢 Total Joins", value=f"`{join_count}` session(s)", inline=True)
    embed.set_footer(
        text="Minecraft Server Logs • Player Disconnected",
        icon_url="https://cdn.iconscout.com/icon/free/png-256/free-minecraft-logo-icon-download-in-svg-png-gif-file-formats--brand-social-media-pack-logos-icons-2661559.png"
    )
    return embed

def build_death_embed(death_info: dict, timestamp: int = None) -> discord.Embed:
    if timestamp is None:
        import time
        timestamp = int(time.time())

    victim = death_info["victim"]
    killer = death_info.get("killer")
    death_message = death_info["death_message"]
    is_pvp = death_info.get("is_pvp", False)
    avatar_url = f"https://mc-heads.net/avatar/{victim}/128"

    color = config.COLOR_MC_PVP if is_pvp else config.COLOR_MC_DEATH
    title = "⚔️ PvP Kill Log" if is_pvp else "💀 Player Death Log"

    embed = discord.Embed(
        title=title,
        color=color,
        timestamp=datetime.fromtimestamp(timestamp)
    )
    embed.set_thumbnail(url=avatar_url)
    embed.add_field(name="👤 Victim", value=f"**{victim}**", inline=True)
    
    if is_pvp:
        embed.add_field(name="🗡️ Killer (Player)", value=f"**{killer}** ⚔️", inline=True)
    elif killer:
        embed.add_field(name="👾 Killer", value=f"`{killer}`", inline=True)
    else:
        embed.add_field(name="⚠️ Cause", value="`Environment / World`", inline=True)
        
    embed.add_field(name="📜 Full Death Log", value=f"> *{death_message}*", inline=False)
    embed.add_field(name="⏰ Time", value=f"<t:{timestamp}:T> • <t:{timestamp}:R>", inline=True)
    embed.add_field(name="📅 Date", value=f"<t:{timestamp}:D>", inline=True)
    embed.set_footer(
        text="Minecraft PvP Combat Logs" if is_pvp else "Minecraft Death Logs • Casualty Alert",
        icon_url="https://cdn.iconscout.com/icon/free/png-256/free-minecraft-logo-icon-download-in-svg-png-gif-file-formats--brand-social-media-pack-logos-icons-2661559.png"
    )
    return embed
