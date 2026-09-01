# ServerRules

**Professional Server Rules & Regulations Plugin for Paper/Spigot**

> Lightweight · Configurable · Bedrock-friendly · Developer API included

---

## Features

- 📋 **9 built-in rule categories** with 36 default rules (fully editable)
- 🖥️ **Inventory GUI** — category browser, paginated rules, accept button
- ✅ **Rule acceptance system** — track which players accepted which version
- 🔢 **Rule versioning** — bump the version to require re-acceptance after rule changes
- 📢 **Auto-announcements** — periodic rule reminders (sequential or random)
- 🔍 **Rule search** — `/rules search <keyword>`
- 📊 **Statistics** — total rules, categories, accepted players
- 🌐 **PlaceholderAPI** integration (optional)
- 📱 **Bedrock/Geyser friendly** — auto-detects Floodgate and uses chat fallback for Bedrock players
- 🧩 **Clean developer API** — via `ServerRulesAPI`
- 🌈 **Hex color support** — `&#RRGGBB` and `&` legacy codes
- 🔒 **Granular permissions**

---

## Supported Platforms

| Platform | Support |
|---|---|
| **Paper 1.21+** | ✅ Full |
| **Spigot 1.21+** | ✅ Full |
| **Purpur 1.21+** | ✅ Full |
| **Java Edition** | ✅ Full GUI |
| **Bedrock (via Geyser+Floodgate)** | ✅ Chat fallback |

> **Java 21+ is required.**

---

## Installation

1. Download `ServerRules-1.0.0.jar` from the releases page.
2. Place it in your server's `plugins/` folder.
3. Start or restart the server.
4. Edit `plugins/ServerRules/rules.yml` to customize your rules.
5. Run `/serverrules reload`.

---

## First Setup

After the first start, you'll see in console:
```
[ServerRules] ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
[ServerRules]   ServerRules — Rules & Regulations
[ServerRules]   Version: 1.0.0
[ServerRules] ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
[ServerRules] Platform: Paper
[ServerRules] Loaded 36 rules in 9 categories (version 1)
[ServerRules] PlaceholderAPI: Hooked
[ServerRules] Geyser: Not installed
[ServerRules] Floodgate: Not installed
[ServerRules] DecentHolograms: Not installed
[ServerRules] ServerRules v1.0.0 successfully enabled!
```

Open `plugins/ServerRules/rules.yml` and customize your rules. When you make significant changes, increase `rules-version` by 1 to require all players to re-accept.

---

## Commands

### Player Commands

| Command | Description | Permission |
|---|---|---|
| `/rules` | Open the rules GUI | `serverrules.view` |
| `/rules accept` | Accept the current rules | `serverrules.accept` |
| `/rules status` | Check your acceptance status | `serverrules.use` |
| `/rules search <text>` | Search rules by keyword | `serverrules.search` |
| `/rules help` | Show help | `serverrules.use` |

### Admin Commands

| Command | Description | Permission |
|---|---|---|
| `/serverrules reload` | Reload all configs | `serverrules.reload` |
| `/serverrules stats` | View statistics | `serverrules.admin` |
| `/serverrules version` | Show rules version | `serverrules.admin` |
| `/serverrules add <cat> <text>` | Add a rule | `serverrules.manage` |
| `/serverrules remove <id>` | Remove a rule | `serverrules.manage` |
| `/serverrules edit <id> <text>` | Edit a rule | `serverrules.manage` |
| `/serverrules list [cat]` | List rules | `serverrules.view` |
| `/serverrules reset <player>` | Reset player acceptance | `serverrules.admin` |
| `/serverrules category add <id>` | Add a category | `serverrules.manage` |
| `/serverrules category remove <id>` | Remove a category | `serverrules.manage` |
| `/serverrules category list` | List categories | `serverrules.view` |

**Command aliases:** `/rules`, `/serverrules`, `/serverrule`, `/srules`

---

## Permissions

| Permission | Default | Description |
|---|---|---|
| `serverrules.use` | Everyone | Use basic `/rules` command |
| `serverrules.view` | Everyone | View rules list and GUI |
| `serverrules.accept` | Everyone | Accept the rules |
| `serverrules.search` | Everyone | Search rules |
| `serverrules.reload` | OP | Reload configuration |
| `serverrules.manage` | OP | Add/remove/edit rules |
| `serverrules.admin` | OP | Full admin access |
| `serverrules.bypass` | Nobody | Bypass acceptance requirement |

---

## Configuration

### `config.yml` — Main Settings

```yaml
acceptance:
  enabled: true
  require-on-first-join: true
  remind-unaccepted-players: true

join-reminder:
  enabled: true
  delay-ticks: 20

announcements:
  enabled: false
  interval: 300        # seconds
  mode: sequential     # or: random
  messages:
    - "&e[ServerRules] Remember to read /rules!"
```

---

## Creating Custom Rules

Edit `plugins/ServerRules/rules.yml`:

```yaml
rules-version: 1   # Bump this when you change rules

categories:
  survival:
    display-name: "&aSurvival Rules"
    description: "Rules specific to Survival mode."
    icon: "GRASS_BLOCK"
    order: 10
    enabled: true
    rules:
      - id: "survival-1"
        text: "&fDo not destroy another player's base."
        description: "Respect other players' hard work."
        severity: HIGH
        enabled: true

      - id: "survival-2"
        text: "&fDo not exploit duplication glitches."
        severity: CRITICAL
        enabled: true
```

**Severity levels:** `LOW`, `MEDIUM`, `HIGH`, `CRITICAL` (informational only — no automatic punishment)

Then run `/serverrules reload`.

---

## Rule Acceptance

The acceptance system tracks whether each player has agreed to the current rules.

**How versioning works:**

1. When a player types `/rules accept`, their UUID, name, and the current `rules-version` are saved.
2. If you change `rules-version` in `rules.yml` to a higher number, players who accepted the old version will be flagged as "Pending" again.
3. They will be reminded on join to accept the updated rules.

**Reset a specific player:**
```
/serverrules reset PlayerName
```

---

## PlaceholderAPI

When PlaceholderAPI is installed, these placeholders are available:

| Placeholder | Description |
|---|---|
| `%serverrules_total%` | Total rule count |
| `%serverrules_enabled%` | Enabled rule count |
| `%serverrules_categories%` | Category count |
| `%serverrules_version%` | Current rules version |
| `%serverrules_player_status%` | `Accepted` or `Pending` |
| `%serverrules_player_accepted%` | `true` or `false` |
| `%serverrules_accepted_count%` | Players who accepted current version |
| `%serverrules_category_chat%` | Rule count for `chat` category |

**Example in a scoreboard plugin:**
```
&7Rules: &f%serverrules_total%
&7Status: &a%serverrules_player_status%
```

---

## Geyser / Bedrock Support

ServerRules auto-detects Floodgate. When a Bedrock player (identified by Floodgate) types `/rules`, they receive a clean **chat-based rule list** instead of the inventory GUI (which behaves differently on Bedrock clients).

No extra configuration is needed. Just install Geyser + Floodgate normally.

---

## API

### Dependency (Gradle)

```groovy
dependencies {
    compileOnly files('libs/ServerRules-1.0.0.jar')
}
```

### Usage

```java
import com.hpmcpe.serverrules.api.ServerRulesAPI;
import com.hpmcpe.serverrules.api.model.Rule;

// Check if available
if (ServerRulesAPI.isAvailable()) {

    // Get all rules in the "chat" category
    List<Rule> chatRules = ServerRulesAPI.getRuleManager()
        .getRulesByCategory("chat");

    // Check if a player has accepted the current version
    boolean accepted = ServerRulesAPI.getAcceptanceManager()
        .hasAccepted(player.getUniqueId());

    // Get current rules version
    int version = ServerRulesAPI.getRulesVersion();

    // Add a rule programmatically
    Rule newRule = new Rule.Builder("my-custom-rule", "general")
        .text("Custom server rule.")
        .severity(RuleSeverity.MEDIUM)
        .build();
    ServerRulesAPI.getRuleManager().addRule(newRule);
}
```

---

## File Structure

```
plugins/ServerRules/
├── config.yml          — Main plugin settings
├── rules.yml           — Rules and categories
├── messages.yml        — All player-facing messages
├── gui.yml             — GUI appearance settings
├── holograms.yml       — Hologram configuration
└── data/
    └── players.json    — Player acceptance records
```

---

## Troubleshooting

**Plugin doesn't load:**
- Check you're running Paper/Spigot 1.21+ with Java 21+
- Check console for error messages

**Rules not showing:**
- Run `/serverrules reload` after editing `rules.yml`
- Check that rule `enabled: true` and category `enabled: true`

**GUI not opening for Bedrock players:**
- This is expected — Bedrock players get a chat-based list instead
- Install Floodgate alongside Geyser for auto-detection

**PlaceholderAPI placeholders not working:**
- Make sure PlaceholderAPI is installed **before** ServerRules loads
- Check console for `PlaceholderAPI: Hooked` confirmation

**Player acceptance not saving:**
- Check file write permissions for `plugins/ServerRules/data/`
- Check console for storage errors

---

## Building From Source

**Requirements:** JDK 21+, internet connection (for downloading dependencies)

```bash
git clone https://github.com/hpmcpe/ServerRules.git
cd ServerRules
./gradlew build
```

Output JAR: `serverrules-paper/build/libs/ServerRules-1.0.0.jar`

**Run tests only:**
```bash
./gradlew :serverrules-core:test
```

---

## Version Compatibility

| ServerRules Version | Minecraft | Java |
|---|---|---|
| 1.0.0 | 1.21, 1.21.1, 1.21.3, 1.21.4 | 21+ |

> Older Minecraft versions (1.20.x and below) are not supported. Use a different plugin version for legacy servers.

---

## FAQ

**Q: Can I add rules without editing the YAML file?**  
A: Yes — `/serverrules add <category> <rule text>`. However, these changes are in-memory only. Add them to `rules.yml` to make them permanent.

**Q: Can I have sub-categories?**  
A: Not currently. Categories are flat. Use descriptive category names instead.

**Q: Will players lose their acceptance when I update the plugin?**  
A: No. Acceptance data is stored in `data/players.json` and persists across restarts and updates.

**Q: Can I use MiniMessage formatting?**  
A: Legacy `&` codes and hex `&#RRGGBB` are supported everywhere. Full MiniMessage support may be added in a future version.

**Q: Can Bedrock players accept rules?**  
A: Yes — via `/rules accept` in chat. The GUI is Java-only; Bedrock players see a chat list.

---

## License

MIT License — see [LICENSE](LICENSE).

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

---

*ServerRules is not affiliated with Mojang, Microsoft, PaperMC, or the Geyser project.*
