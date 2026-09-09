import os
import sqlite3
import time
from typing import Optional, Dict, Any, List, Tuple

DATA_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../data"))
os.makedirs(DATA_DIR, exist_ok=True)
DB_PATH = os.path.join(DATA_DIR, "monitor.db")

def get_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS mc_players (
            username TEXT PRIMARY KEY COLLATE NOCASE,
            uuid TEXT,
            join_count INTEGER DEFAULT 0,
            first_seen INTEGER,
            last_seen INTEGER,
            current_session_start INTEGER,
            total_playtime_seconds INTEGER DEFAULT 0
        )
    """)
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS discord_members (
            user_id TEXT PRIMARY KEY,
            username TEXT,
            current_vc_start INTEGER,
            total_vc_seconds INTEGER DEFAULT 0,
            total_vc_joins INTEGER DEFAULT 0,
            total_status_changes INTEGER DEFAULT 0,
            total_profile_changes INTEGER DEFAULT 0,
            last_seen INTEGER
        )
    """)
    
    # Auto-migrate table columns if older DB exists
    cursor.execute("PRAGMA table_info(discord_members)")
    cols = [row["name"] for row in cursor.fetchall()]
    if "total_vc_joins" not in cols:
        cursor.execute("ALTER TABLE discord_members ADD COLUMN total_vc_joins INTEGER DEFAULT 0")
    if "total_status_changes" not in cols:
        cursor.execute("ALTER TABLE discord_members ADD COLUMN total_status_changes INTEGER DEFAULT 0")
    if "total_profile_changes" not in cols:
        cursor.execute("ALTER TABLE discord_members ADD COLUMN total_profile_changes INTEGER DEFAULT 0")

    conn.commit()
    conn.close()

init_db()

def format_duration(seconds: int) -> str:
    if not seconds or seconds <= 0:
        return "0s"
    hrs = seconds // 3600
    mins = (seconds % 3600) // 60
    secs = seconds % 60
    parts = []
    if hrs > 0:
        parts.append(f"{hrs}h")
    if mins > 0:
        parts.append(f"{mins}m")
    if secs > 0 or not parts:
        parts.append(f"{secs}s")
    return " ".join(parts)

# --- Minecraft DB Operations ---

def record_player_join(username: str, uuid: Optional[str] = None) -> Dict[str, Any]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    
    cursor.execute("SELECT * FROM mc_players WHERE username = ?", (username,))
    row = cursor.fetchone()
    
    if row:
        cursor.execute("""
            UPDATE mc_players
            SET join_count = join_count + 1,
                last_seen = ?,
                current_session_start = ?,
                uuid = COALESCE(?, uuid)
            WHERE username = ?
        """, (now, now, uuid, username))
    else:
        cursor.execute("""
            INSERT INTO mc_players (username, uuid, join_count, first_seen, last_seen, current_session_start, total_playtime_seconds)
            VALUES (?, ?, 1, ?, ?, ?, 0)
        """, (username, uuid, now, now, now))
        
    conn.commit()
    cursor.execute("SELECT * FROM mc_players WHERE username = ?", (username,))
    updated = dict(cursor.fetchone())
    conn.close()
    return updated

def record_player_leave(username: str) -> Tuple[Dict[str, Any], int]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    session_seconds = 0
    
    cursor.execute("SELECT * FROM mc_players WHERE username = ?", (username,))
    row = cursor.fetchone()
    
    if row:
        current_session = row["current_session_start"]
        if current_session:
            session_seconds = max(0, now - current_session)
            cursor.execute("""
                UPDATE mc_players
                SET total_playtime_seconds = total_playtime_seconds + ?,
                    last_seen = ?,
                    current_session_start = NULL
                WHERE username = ?
            """, (session_seconds, now, username))
        else:
            cursor.execute("UPDATE mc_players SET last_seen = ?, current_session_start = NULL WHERE username = ?", (now, username))
        conn.commit()
        
    cursor.execute("SELECT * FROM mc_players WHERE username = ?", (username,))
    updated_row = cursor.fetchone()
    player_dict = dict(updated_row) if updated_row else {
        "username": username,
        "join_count": 1,
        "total_playtime_seconds": 0,
        "first_seen": now,
        "last_seen": now
    }
    conn.close()
    return player_dict, session_seconds

def get_player(username: str) -> Optional[Dict[str, Any]]:
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM mc_players WHERE username = ?", (username,))
    row = cursor.fetchone()
    conn.close()
    return dict(row) if row else None

def get_top_players(limit: int = 10, sort_by: str = "playtime") -> List[Dict[str, Any]]:
    conn = get_connection()
    cursor = conn.cursor()
    col = "join_count" if sort_by == "joins" else "total_playtime_seconds"
    cursor.execute(f"SELECT * FROM mc_players ORDER BY {col} DESC LIMIT ?", (limit,))
    rows = cursor.fetchall()
    conn.close()
    return [dict(r) for r in rows]

# --- Discord Activity DB Operations ---

def record_status_change(user_id: str, username: str) -> Dict[str, Any]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("""
        INSERT INTO discord_members (user_id, username, total_status_changes, total_vc_seconds, total_vc_joins, total_profile_changes, last_seen)
        VALUES (?, ?, 1, 0, 0, 0, ?)
        ON CONFLICT(user_id) DO UPDATE SET
            username = excluded.username,
            total_status_changes = total_status_changes + 1,
            last_seen = excluded.last_seen
    """, (str(user_id), username, now))
    conn.commit()
    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    row = dict(cursor.fetchone())
    conn.close()
    return row

def record_vc_join(user_id: str, username: str) -> Dict[str, Any]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("""
        INSERT INTO discord_members (user_id, username, current_vc_start, total_vc_seconds, total_vc_joins, total_status_changes, total_profile_changes, last_seen)
        VALUES (?, ?, ?, 0, 1, 0, 0, ?)
        ON CONFLICT(user_id) DO UPDATE SET
            username = excluded.username,
            current_vc_start = excluded.current_vc_start,
            total_vc_joins = total_vc_joins + 1,
            last_seen = excluded.last_seen
    """, (str(user_id), username, now, now))
    conn.commit()
    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    row = dict(cursor.fetchone())
    conn.close()
    return row

def record_vc_leave(user_id: str, username: str) -> Tuple[Dict[str, Any], int]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    session_seconds = 0
    
    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    row = cursor.fetchone()
    if row and row["current_vc_start"]:
        session_seconds = max(0, now - row["current_vc_start"])
        cursor.execute("""
            UPDATE discord_members
            SET total_vc_seconds = total_vc_seconds + ?,
                current_vc_start = NULL,
                last_seen = ?,
                username = ?
            WHERE user_id = ?
        """, (session_seconds, now, username, str(user_id)))
        conn.commit()

    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    updated_row = cursor.fetchone()
    member_dict = dict(updated_row) if updated_row else {
        "user_id": str(user_id),
        "username": username,
        "total_vc_seconds": session_seconds,
        "total_vc_joins": 1,
        "total_status_changes": 0,
        "total_profile_changes": 0,
        "last_seen": now
    }
    conn.close()
    return member_dict, session_seconds

def record_profile_change(user_id: str, username: str) -> Dict[str, Any]:
    now = int(time.time())
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("""
        INSERT INTO discord_members (user_id, username, total_status_changes, total_vc_seconds, total_vc_joins, total_profile_changes, last_seen)
        VALUES (?, ?, 0, 0, 0, 1, ?)
        ON CONFLICT(user_id) DO UPDATE SET
            username = excluded.username,
            total_profile_changes = total_profile_changes + 1,
            last_seen = excluded.last_seen
    """, (str(user_id), username, now))
    conn.commit()
    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    row = dict(cursor.fetchone())
    conn.close()
    return row

def get_discord_member(user_id: str) -> Optional[Dict[str, Any]]:
    conn = get_connection()
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM discord_members WHERE user_id = ?", (str(user_id),))
    row = cursor.fetchone()
    conn.close()
    return dict(row) if row else None

def get_database_summary() -> Dict[str, Any]:
    conn = get_connection()
    cursor = conn.cursor()
    
    cursor.execute("SELECT COUNT(*) as count, SUM(total_playtime_seconds) as playtime, SUM(join_count) as joins FROM mc_players")
    mc_row = cursor.fetchone()
    
    cursor.execute("SELECT COUNT(*) as count, SUM(total_status_changes) as status_changes, SUM(total_vc_joins) as vc_joins, SUM(total_vc_seconds) as vc_seconds FROM discord_members")
    dc_row = cursor.fetchone()
    
    conn.close()
    return {
        "mc_players_count": mc_row["count"] or 0,
        "mc_total_playtime": mc_row["playtime"] or 0,
        "mc_total_joins": mc_row["joins"] or 0,
        "dc_members_count": dc_row["count"] or 0,
        "dc_total_status_changes": dc_row["status_changes"] or 0,
        "dc_total_vc_joins": dc_row["vc_joins"] or 0,
        "dc_total_vc_seconds": dc_row["vc_seconds"] or 0
    }

