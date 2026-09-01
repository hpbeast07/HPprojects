# Changelog

All notable changes to ServerRules will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2024-09-01

### Added
- Initial release of ServerRules
- 9 built-in rule categories: General, Chat, Gameplay, Griefing, PvP, Economy, Accounts, Staff, Security
- 36 default configurable rules
- Inventory GUI with category selection, paginated rule view, and accept button
- Rule acceptance system with version tracking
- Rule versioning — bump `rules-version` to require re-acceptance
- Join reminder messages
- Periodic rule announcements (sequential / random mode)
- Rule search via `/rules search <keyword>`
- PlaceholderAPI integration (8 placeholders)
- Geyser / Floodgate detection with chat fallback for Bedrock players
- Admin commands: add, remove, edit, list, reset, stats, version, reload
- Category management commands
- Tab completion for all commands
- JSON player data storage
- Hex color support (`&#RRGGBB`) and legacy `&` codes
- Clean public developer API via `ServerRulesAPI`
- Granular permission system
- Professional config files: `config.yml`, `rules.yml`, `messages.yml`, `gui.yml`, `holograms.yml`
- JUnit 5 unit tests for core logic
