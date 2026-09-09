import sys
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8")
if hasattr(sys.stderr, "reconfigure"):
    sys.stderr.reconfigure(encoding="utf-8")

import os
import asyncio
import tracemalloc
tracemalloc.start()
import discord
from discord.ext import commands
import config

print("=====================================================")
print("🚀 Initializing Minecraft & Discord Monitor Bot (Python)...")
print("=====================================================")

if not config.TOKEN or config.TOKEN == "your_discord_bot_token_here":
    print("\n❌ ERROR: DISCORD_TOKEN is missing or not set in .env file!")
    print("👉 Please edit the .env file and paste your Discord Bot Token & Channel IDs.\n")
    sys.exit(1)

# Enable all required Gateway Intents
intents = discord.Intents.default()
intents.guilds = True
intents.members = True
intents.presences = True
intents.voice_states = True
intents.message_content = True

bot = commands.Bot(command_prefix="!", intents=intents)

@bot.event
async def on_ready():
    print(f"\n========================================")
    print(f"🤖 Logged in as: {bot.user.name}#{bot.user.discriminator} (ID: {bot.user.id})")
    print(f"🌐 Connected to {len(bot.guilds)} server(s)")
    print(f"========================================\n")

    # Sync Slash Commands
    try:
        if config.GUILD_ID:
            guild_obj = discord.Object(id=config.GUILD_ID)
            bot.tree.copy_global_to(guild=guild_obj)
            synced = await bot.tree.sync(guild=guild_obj)
            print(f"[Commands] Instantly synced {len(synced)} slash command(s) for Guild ID: {config.GUILD_ID}")
        else:
            synced = await bot.tree.sync()
            print(f"[Commands] Synced {len(synced)} global slash command(s)!")
    except Exception as e:
        print(f"[Commands] Failed to sync slash commands: {e}")

async def main():
    # Load all Cogs
    initial_extensions = [
        "cogs.minecraft_logs",
        "cogs.discord_monitor",
        "cogs.commands"
    ]

    for ext in initial_extensions:
        try:
            await bot.load_extension(ext)
            print(f"[Loaded Cog] {ext}")
        except Exception as e:
            print(f"[Error Loading Cog] {ext}: {e}")

    async with bot:
        await bot.start(config.TOKEN)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n👋 Bot shutting down...")
