# 🎮 Minecraft & Discord Activity Monitor Bot

Ek professional Python Discord bot (`discord.py` v2.4+) jo Minecraft server logs aur
Discord members ki activity ko dedicated channels me record karta hai, aur live
server status check karne ke liye slash commands provide karta hai.

**Features:**
1. **Minecraft Join / Leave Logs** — player skins, join counters, timestamps, session duration aur total playtime (`#minecraft-join-leave`).
2. **Minecraft Death & Kill Logs** — death causes, mob kills aur PvP kills (`#minecraft-death-logs`).
3. **Discord Activity Monitor** — status changes (🟢 🟡 🔴 ⚫), voice channel activity aur profile changes (`#player-monitor-log`).
4. **Slash Commands** — `/mcstats`, `/mcleaderboard`, `/mcstatus`, `/discordstats`, `/status`, `/help`.

---

## 📁 Project Structure

```
player-monitor-bot/
├── cogs/
│   ├── minecraft_logs.py        # MC Join/Leave/Death events listener
│   ├── discord_monitor.py       # Discord presence & VC events listener
│   └── commands.py              # Slash commands
├── services/
│   ├── mc_log_watcher.py        # Async latest.log parser
│   ├── mc_status.py             # Minecraft server live status (mcstatus)
│   ├── mc_embed_builder.py      # Minecraft embeds
│   └── discord_embed_builder.py # Discord activity embeds
├── database/
│   └── db.py                    # SQLite storage & analytics
├── data/
│   └── monitor.db               # SQLite database file
├── logs/
│   └── latest.log               # Minecraft server log (configurable)
├── config.py                    # Central configuration loader
├── main.py                      # Bot entry point
├── requirements.txt
└── .env                         # Token & channel IDs
```

---

## 🚀 Setup

### Step 1: Discord Developer Portal
1. [Discord Developer Portal](https://discord.com/developers/applications) par **New Application** banayein.
2. **Bot** tab me **Reset Token** se apna **Bot Token** copy karein.
3. **Privileged Gateway Intents** enable karein:
   - ✅ **Presence Intent**
   - ✅ **Server Members Intent**
   - ✅ **Message Content Intent**
4. **OAuth2 > URL Generator** me scopes `bot` + `applications.commands` aur `Administrator` (ya `Send Messages`, `Embed Links`) permission ke sath bot ko invite karein.

### Step 2: `.env` Configure Karein
`.env.example` ko copy karke `.env` banayein aur values fill karein:

```env
DISCORD_TOKEN=your_bot_token_here
GUILD_ID=your_server_id_here

# Minecraft Join/Leave & Death log channels
MC_JOIN_LEAVE_CHANNEL_ID=123456789012345678
MC_DEATH_CHANNEL_ID=123456789012345678

# Discord activity monitor channel
DISCORD_MONITOR_CHANNEL_ID=123456789012345678

# Minecraft server log path (default ./logs/latest.log)
MC_LOG_PATH=C:/path/to/your/server/logs/latest.log

# Minecraft server address & port (for /mcstatus)
MC_SERVER_ADDRESS=your.server.ip.or.domain
MC_SERVER_PORT=25565
```

> 💡 **Note:** `/mcstatus` me full online player list ke liye server ke
> `server.properties` me `enable-query=true` set karein (optional; player
> count & status bina iske bhi kaam karta hai).

### Step 3: Dependencies Install & Run
```bash
pip install -r requirements.txt
python main.py
```

---

## ⚡ Slash Commands

| Command | Description |
|---|---|
| `/mcstats <player>` | Player ka 3D skin render, join count, status aur total playtime |
| `/mcleaderboard [sort]` | Top 10 players leaderboard (playtime / joins) |
| `/mcstatus` | Minecraft server ka live status: MOTD, version, latency, players |
| `/discordstats [user]` | Discord member status, VC time aur activity stats |
| `/status` | Bot latency, memory aur system diagnostics |
| `/help` | Complete command guide |

---

## 🐍 Requirements

- Python 3.13+ (tested & runs on the latest **Python 3.14**)
- `discord.py>=2.4.0`
- `python-dotenv>=1.0.0`
- `mcstatus>=14.0.0`
