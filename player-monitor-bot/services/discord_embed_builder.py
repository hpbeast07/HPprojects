import discord
from datetime import datetime
import time
from typing import Optional, Dict, Any
import config
from database.db import format_duration

def build_status_embed(member: discord.Member, old_status: discord.Status, new_status: discord.Status, member_stats: Optional[Dict[str, Any]] = None) -> discord.Embed:
    timestamp = int(time.time())

    status_color_map = {
        discord.Status.online: config.COLOR_DISCORD_ONLINE,
        discord.Status.idle: config.COLOR_DISCORD_IDLE,
        discord.Status.dnd: config.COLOR_DISCORD_DND,
        discord.Status.do_not_disturb: config.COLOR_DISCORD_DND,
        discord.Status.offline: config.COLOR_DISCORD_OFFLINE
    }

    status_emoji_map = {
        discord.Status.online: "🟢 Online",
        discord.Status.idle: "🟡 Idle",
        discord.Status.dnd: "🔴 Do Not Disturb",
        discord.Status.do_not_disturb: "🔴 Do Not Disturb",
        discord.Status.offline: "⚫ Offline"
    }

    color = status_color_map.get(new_status, config.COLOR_PRIMARY)
    old_text = status_emoji_map.get(old_status, f"❓ {old_status}")
    new_text = status_emoji_map.get(new_status, f"❓ {new_status}")

    # Activities formatting (Game, Spotify, Custom status)
    activity_lines = []
    for act in member.activities:
        if isinstance(act, discord.CustomActivity):
            emoji_str = str(act.emoji) + " " if act.emoji else ""
            activity_lines.append(f"💬 *{emoji_str}{act.name or ''}*")
        elif isinstance(act, discord.Spotify):
            activity_lines.append(f"🎵 **Spotify**: *{act.title}* by *{act.artist}*")
        elif isinstance(act, discord.Game) or isinstance(act, discord.Activity):
            activity_lines.append(f"🎮 **{act.name}**" + (f" ({act.details})" if act.details else ""))

    activity_text = "\n".join(activity_lines) if activity_lines else "None"
    total_changes = member_stats.get("total_status_changes", 1) if member_stats else 1

    embed = discord.Embed(
        title="👥 Discord Member Status Changed",
        color=color,
        timestamp=datetime.fromtimestamp(timestamp)
    )
    embed.set_thumbnail(url=member.display_avatar.url)
    embed.add_field(name="👤 Member", value=f"{member.mention} (`{member.name}`)", inline=True)
    embed.add_field(name="🔄 Status Change", value=f"{old_text} ➔ **{new_text}**", inline=True)
    embed.add_field(name="🔢 Total Status Changes", value=f"**#{total_changes}** update(s)", inline=True)
    embed.add_field(name="🆔 User ID", value=f"`{member.id}`", inline=True)
    embed.add_field(name="🎮 Activity / Status", value=activity_text, inline=False)
    embed.add_field(name="⏰ Time & Date", value=f"<t:{timestamp}:F> • <t:{timestamp}:R>", inline=False)
    embed.set_footer(
        text=f"Discord Member Monitor • Activity #{total_changes}",
        icon_url=member.display_avatar.url
    )
    return embed

def build_voice_embed(action: str, member: discord.Member, member_stats: Optional[Dict[str, Any]] = None, **kwargs) -> discord.Embed:
    timestamp = int(time.time())
    avatar_url = member.display_avatar.url

    title = "🔊 Voice Channel Activity"
    color = config.COLOR_VC_JOIN

    embed = discord.Embed(timestamp=datetime.fromtimestamp(timestamp))
    embed.set_thumbnail(url=avatar_url)
    embed.add_field(name="👤 Member", value=f"{member.mention} (`{member.name}`)", inline=True)
    embed.add_field(name="🆔 User ID", value=f"`{member.id}`", inline=True)

    total_vc_joins = member_stats.get("total_vc_joins", 1) if member_stats else 1
    total_vc_seconds = member_stats.get("total_vc_seconds", 0) if member_stats else 0
    total_vc_formatted = format_duration(total_vc_seconds)

    if action == "join":
        title = "🟢 Joined Voice Channel"
        color = config.COLOR_VC_JOIN
        channel = kwargs.get("channel")
        embed.add_field(name="🔢 Total VC Visits", value=f"**#{total_vc_joins}** time(s)", inline=True)
        embed.add_field(name="🔊 Channel Joined", value=f"{channel.mention} (`{channel.name}`)", inline=False)
        embed.add_field(name="⏳ Lifetime VC Time", value=f"`{total_vc_formatted}`", inline=True)

    elif action == "leave":
        title = "🔴 Left Voice Channel"
        color = config.COLOR_VC_LEAVE
        channel = kwargs.get("channel")
        session_seconds = kwargs.get("session_seconds", 0)
        formatted_time = format_duration(session_seconds)
        embed.add_field(name="🔇 Channel Left", value=f"{channel.mention} (`{channel.name}`)", inline=True)
        embed.add_field(name="⏱️ Current Session Time", value=f"**{formatted_time}**", inline=True)
        embed.add_field(name="⏳ Total Lifetime VC Time", value=f"`{total_vc_formatted}`", inline=True)

    elif action == "move":
        title = "🔀 Switched Voice Channel"
        color = config.COLOR_VC_MOVE
        old_ch = kwargs.get("old_channel")
        new_ch = kwargs.get("new_channel")
        embed.add_field(
            name="🔀 Channel Movement",
            value=f"{old_ch.mention} (`{old_ch.name}`) ➔ {new_ch.mention} (`{new_ch.name}`)",
            inline=False
        )

    elif action == "stream_start":
        title = "📺 Screen Share / Stream Started"
        color = config.COLOR_VC_STREAM
        channel = kwargs.get("channel")
        embed.add_field(name="🔊 Voice Channel", value=f"{channel.mention} (`{channel.name}`)", inline=False)

    elif action == "stream_stop":
        title = "⏹️ Screen Share / Stream Ended"
        color = config.COLOR_DISCORD_OFFLINE
        channel = kwargs.get("channel")
        embed.add_field(name="🔊 Voice Channel", value=f"{channel.mention} (`{channel.name}`)", inline=False)

    embed.title = title
    embed.color = color
    embed.add_field(name="⏰ Time & Date", value=f"<t:{timestamp}:F> • <t:{timestamp}:R>", inline=False)
    embed.set_footer(
        text="Discord Member Monitor • Voice Event",
        icon_url=avatar_url
    )
    return embed

def build_profile_update_embed(member: discord.Member, change_type: str, before_val: str, after_val: str, member_stats: Optional[Dict[str, Any]] = None) -> discord.Embed:
    timestamp = int(time.time())
    total_profile = member_stats.get("total_profile_changes", 1) if member_stats else 1

    embed = discord.Embed(
        title=f"📝 Member {change_type} Updated",
        color=config.COLOR_PRIMARY,
        timestamp=datetime.fromtimestamp(timestamp)
    )
    embed.set_thumbnail(url=member.display_avatar.url)
    embed.add_field(name="👤 Member", value=f"{member.mention} (`{member.name}`)", inline=True)
    embed.add_field(name="🔢 Total Profile Changes", value=f"**#{total_profile}**", inline=True)
    embed.add_field(name="📋 Change Details", value=f"`{before_val}` ➔ **`{after_val}`**", inline=False)
    embed.add_field(name="⏰ Time & Date", value=f"<t:{timestamp}:F> • <t:{timestamp}:R>", inline=False)
    embed.set_footer(
        text=f"Discord Member Monitor • Profile Update #{total_profile}",
        icon_url=member.display_avatar.url
    )
    return embed
