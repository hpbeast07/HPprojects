import os
import discord
from dotenv import load_dotenv

load_dotenv()

# Discord Bot Credentials
TOKEN = os.getenv("DISCORD_TOKEN", "")
GUILD_ID = int(os.getenv("GUILD_ID")) if os.getenv("GUILD_ID") and os.getenv("GUILD_ID").isdigit() else None

# Dedicated Channels
MC_JOIN_LEAVE_CHANNEL_ID = int(os.getenv("MC_JOIN_LEAVE_CHANNEL_ID")) if os.getenv("MC_JOIN_LEAVE_CHANNEL_ID") and os.getenv("MC_JOIN_LEAVE_CHANNEL_ID").isdigit() else None
MC_DEATH_CHANNEL_ID = int(os.getenv("MC_DEATH_CHANNEL_ID")) if os.getenv("MC_DEATH_CHANNEL_ID") and os.getenv("MC_DEATH_CHANNEL_ID").isdigit() else None
DISCORD_MONITOR_CHANNEL_ID = int(os.getenv("DISCORD_MONITOR_CHANNEL_ID")) if os.getenv("DISCORD_MONITOR_CHANNEL_ID") and os.getenv("DISCORD_MONITOR_CHANNEL_ID").isdigit() else None

# Minecraft Log Path
MC_LOG_PATH = os.path.abspath(os.getenv("MC_LOG_PATH", "./logs/latest.log"))
# Minecraft Server (for /mcstatus live status command)
MC_SERVER_ADDRESS = os.getenv("MC_SERVER_ADDRESS", "localhost")
MC_SERVER_PORT = int(os.getenv("MC_SERVER_PORT", "25565")) if os.getenv("MC_SERVER_PORT", "25565").isdigit() else 25565

# Colors (discord.Color)
COLOR_MC_JOIN = discord.Color.from_rgb(46, 204, 113)     # #2ecc71 Green
COLOR_MC_LEAVE = discord.Color.from_rgb(231, 76, 60)     # #e74c3c Red
COLOR_MC_DEATH = discord.Color.from_rgb(153, 45, 34)     # #992d22 Dark Red
COLOR_MC_PVP = discord.Color.from_rgb(192, 57, 43)       # #c0392b Crimson

COLOR_DISCORD_ONLINE = discord.Color.from_rgb(46, 204, 113)   # Green
COLOR_DISCORD_IDLE = discord.Color.from_rgb(241, 196, 15)     # Amber
COLOR_DISCORD_DND = discord.Color.from_rgb(231, 76, 60)       # Red
COLOR_DISCORD_OFFLINE = discord.Color.from_rgb(116, 127, 141) # Dark Grey

COLOR_VC_JOIN = discord.Color.from_rgb(46, 204, 113)
COLOR_VC_LEAVE = discord.Color.from_rgb(231, 76, 60)
COLOR_VC_MOVE = discord.Color.from_rgb(52, 152, 219)
COLOR_VC_STREAM = discord.Color.from_rgb(155, 89, 182)

COLOR_PRIMARY = discord.Color.blurple()
COLOR_GOLD = discord.Color.gold()
