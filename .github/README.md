# Chromiecraft's Smartstone Module

## About

This module was inspired by Azerothshard's Smartstone module.

The Smartstone is a versatile item that offers a wide range of gossip options, enabling you to provide various services and entertaining features to players.

This module makes use of Azerothcore's subscription system (from [acore-cms](https://github.com/azerothcore/acore-cms/) intregated subscription system.)

> [!NOTE]
> You may get a worldserver error `STORAGE: Can't equip or store initial item 32547 for race X class Y, error msg = 17` you may ignore the error.

## How to use this?

> [!WARNING]
> This module changes an existing Item `Tier 5 Mage Test Gear` ([32547](https://wowgaming.altervista.org/aowow/?item=32547)) to be named `Smartstone` and use the smartstone scripts.

> [!NOTE]
> This module requires you to have `EnablePlayerSettings = 1` in your `worldserver.conf` to work, the information gets stored in `character_settings` in your `acore_characters`.

| Category ID     | Catergory Name    | Catergory Range | Command Value           | Database Table      |
| --------------- | ----------------- | --------------- | ----------------------- | ------------------- |
| 0 (PetID)       | Pets / Companions | 80001 - 89999   | PetID minus 80000       | smartstone_pets     |
| 1 (CombatPetID) | Combat Pets       | 90001 - 99999   | CombatPetID minus 90000 | smartstone_pets     |
| 2 (CostumeID)   | Costumes          | 20001 - 29999   | CostumeID minus 20000   | smartstone_costumes |

The command usage in-game or via the worldserver:

`.smartstone unlock service $characterName (or $characterGUID) $categoryID $petOrCostumeID $true # (to add, or $false to remove)`

```bash
# Unlocks (Pet) Amani'shi Groundbreaker
.smartstone unlock service Nyeriah 0 80001 true

# Unlocks (Combat Pet) Winterveil Helper
.smartstone unlock service Nyeriah 1 90001 true

# Unlocks (Costume) Edwin VanCleef
.smartstone unlock service Nyeriah 2 20001 true
```

> [!NOTE]
> Some of the existing pets as example `Summon Hyjal Wisp`, will not work if you haven't completed their required raids in mythic from [mod-zone-difficulty](https://github.com/azerothcore/mod-zone-difficulty).

## Tokens

Tokens are one-shot, account-wide grants. Types 1-4 cover a **character service**: a name change, faction change, race change, or appearance (customize) change. A moderator grants a token to an account; any character on that account can then claim it. Claiming sets the matching at-login flag, so the actual change happens at the character selection screen the next time the player logs out to it.

Types 5-6 are **challenge restore** tokens: they grant an item stack or teach a spell on claim. They carry their payload in the `Data` column (`itemEntry:count` for items, `spellId` for spells) and can only be seen and claimed by challenge mode characters (marker auras 2000100-2000102, any level).

Tokens are stored in `acore_auth` (`smartstone_account_tokens`), so a granted token is available on every realm and to every character on the account until claimed. Claimed tokens are kept (not deleted) as an audit record.

### Token types

| Type | Service           | Effect on claim          |
| ---- | ----------------- | ------------------------ |
| 1    | Name Change       | `AT_LOGIN_RENAME`        |
| 2    | Faction Change    | `AT_LOGIN_CHANGE_FACTION`|
| 3    | Race Change       | `AT_LOGIN_CHANGE_RACE`   |
| 4    | Appearance Change | `AT_LOGIN_CUSTOMIZE`     |
| 5    | Item Restoration  | Adds the item(s) from `Data` to the inventory |
| 6    | Spell Restoration | Teaches the spell from `Data`                 |

### Challenge reclaim

When a challenge mode character below level 70 suffers a fatal death (killed by a creature or a player, the same two events the challenge script deletes on), the module converts its whitelisted possessions on the spot: it scans the live character's inventory (equipped, bags, bank) and spellbook against the `smartstone_challenge_reclaimable` whitelist (`acore_world`), grants one restore token per match to the account, and destroys the items / removes the spells before the character is saved and unlinked. A replacement challenge character can then claim the tokens from **Character > Tokens**, and a GM-restored character comes back without the converted possessions, so nothing can be duplicated.

As a fallback, ordinary deletions of a challenge character (e.g. from the character selection screen) grant the same tokens by scanning the characters DB at deletion time. On soft deletions (`CharDelete.Method = 1`) the matched rows are stripped for the same no-duplication guarantee; this path requires the core's `OnPlayerUnlinkFromDB` hook ([azerothcore#26794](https://github.com/azerothcore/azerothcore-wotlk/pull/26794)).

#### Store-origin gating

Whitelist entries with `RequireStoreOrigin = 1` only convert units the character actually received from the store. Every item delivered by GM mail (`.send items`, which is also the CMS shop's delivery path) is logged into `smartstone_store_item_log`, one row per item unit with a timestamp; the same PR also needs the core's `MailDraft::GetItems` accessor. On death, conversion is capped at the logged credit: a character who bought 3 Gigantique Bags but owns 4 gets a token for 3, and the fourth dies with the character. Claiming an item-restore token re-credits the claiming character, so restored items stay convertible across successive deaths. Entries with `RequireStoreOrigin = 0` convert everything owned, regardless of origin.

### Commands

Moderator commands (also usable from the worldserver console). The account can be given by name or account id:

```bash
# Grant a Faction Change (type 2) token to an account
.smartstone token grant Nyeriah 2

# Grant an Item Restoration (type 5) token: 3x item 19019
.smartstone token grant Nyeriah 5 19019:3

# Grant a Spell Restoration (type 6) token for spell 33388
.smartstone token grant Nyeriah 6 33388

# List an account's unclaimed tokens and their ids
.smartstone token list Nyeriah

# List all tokens including claimed ones (who claimed each, and when)
.smartstone token list Nyeriah all

# Remove an unclaimed token by id (claimed tokens are preserved)
.smartstone token revoke 42
```

Player command:

```bash
# Claim token id 42 on the current character
.smartstone token claim 42
```

Players can also claim from the Smartstone menu under **Character > Tokens** (the category only appears while the account has unclaimed tokens). On login, a reminder lists any unclaimed tokens and the command to claim each one.

## Announcements

The Smartstone exposes the core per-character announcer settings (the same ones as the `.settings announcer` command) under **Character > Announcements**. Each entry is a toggle whose label reflects its current state, letting a player silence or restore an announcer type without a command:

- Battleground queue announcements
- Arena queue announcements
- PvP start announcements
- Server broadcasts (autobroadcast)

These are stored as the core `ac_default` character settings, so they require `PlayerSettings.Enable` to be on in the core config.

New characters are created with PvP announcements (battleground queue, arena queue, and PvP start) disabled by default, as if `.settings announcer pvpall off` had been run. Server broadcasts are left on. Players can re-enable any of them from the menu or the command at any time.

Two config options control this feature:

- `ModChromiecraftSmartstone.QueueAnnouncer.Enable` (default `1`) — shows the Announcements menu. When `0`, the category is hidden.
- `ModChromiecraftSmartstone.QueueAnnouncer.DisablePvpOnCreate` (default `1`) — sets PvP announcements off on character creation. When `0`, new characters keep the core defaults.

## SQL File Prefixes

All SQL files should have a numeric prefix to indicate their purpose and order of execution. The prefixes are as follows:

-   0 -> creation of the table
-   1 -> updates to the table
-   2 -> add data to the table
-   3 -> used by other modules to add data to the tables

## How to make your own?

### Pet

Requires `creature_template`, `creature_template_model` and `smartstone_pets`.

Example for `Amani'shi Groundbreaker` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/0203eb05a78b27475630f3ff3b57eac2bbc6f6d9/data/sql/db-world/updates/2_sts_data.sql#L70).

### Combat Pet

Requires `creature_template`, `creature_template_model`, `creature_template_spell` and `smartstone_pets`.

Example for `Winterveil Helper` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/0203eb05a78b27475630f3ff3b57eac2bbc6f6d9/data/sql/db-world/updates/2_sts_data.sql#L302).

### Costumes

Requires only `smartstone_costumes`.

Example for `Edwin VanCleef` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/0203eb05a78b27475630f3ff3b57eac2bbc6f6d9/data/sql/db-world/updates/2_sts_data.sql#L19).

## Documentation

### smartstone_pets (acore_world)

| CreatureId     | Category | SubscriptionLevel | Duration          | Description | Enabled              |
| -------------- | -------- | ----------------- | ----------------- | ----------- | -------------------- |
| 80001 to 99999 | 0 or 1   | 1 to 3            | In Unix Timestamp | Gossip Text | 1 (Show) or 0 (Hide) |

### smartstone_costumes (acore_world)

| Id             | [DisplayId](https://www.azerothcore.org/wiki/creature_template_model#creaturedisplayid) | Category                 | SubscriptionLevel | Duration   | Description | Enabled              |
| -------------- | --------------------------------------------------------------------------------------- | ------------------------ | ----------------- | ---------- | ----------- | -------------------- |
| 20001 to 29999 | ModelID                                                                                 | smartstone_categories.id | 1 to 3            | In seconds | Gossip Text | 1 (Show) or 0 (Hide) |

### smartstone_categories (acore_world)

| Id                | CategoryType | Title                        | SubscriptionLevel | [NPCTextId](https://www.azerothcore.org/wiki/npc_text#id) | Enabled              |
| ----------------- | ------------ | ---------------------------- | ----------------- | --------------------------------------------------------- | -------------------- |
| Unique Identifier | 0            | Title that shows a sub-menu? | 1 to 3            | npc_text.id                                               | 1 (Show) or 0 (Hide) |

### smartstone_account_tokens (acore_auth)

One row per granted token. Populated by `.smartstone token grant`; rows are marked consumed (not deleted) when claimed.

| Column         | Meaning                                                    |
| -------------- | ---------------------------------------------------------- |
| Id             | Auto-increment; the id used to claim/revoke the token    |
| AccountId      | Owning account                                             |
| TokenType    | 1-6 (see [Tokens](#tokens))                            |
| Data           | Restore payload: `itemEntry:count` (type 5) or `spellId` (type 6); empty for types 1-4 |
| GrantedBy      | Granting account id (0 = console/automatic)                |
| GrantedTime    | Unix timestamp of the grant                                |
| ConsumedByGUID | Character GUID that claimed it (0 = unclaimed)             |
| ConsumedTime   | Unix timestamp of the claim (0 = unclaimed)                |

### smartstone_challenge_reclaimable (acore_world)

Whitelist of what converts into restore tokens when a challenge character is deleted.

| Column             | Meaning                                  |
| ------------------ | ---------------------------------------- |
| Type               | 0 = item entry, 1 = spell id             |
| Entry              | The item entry / spell id                |
| RequireStoreOrigin | Items only: 1 = cap conversion at the character's `smartstone_store_item_log` credit; 0 = convert everything owned |
| Comment            | Free-form note for maintainers           |

### smartstone_store_item_log (acore_characters)

One row per item unit received from GM/store mail or restored by an item-restore token claim. Maintained automatically; rows for an entry are consumed when a death converts that entry.

| Column       | Meaning                                          |
| ------------ | ------------------------------------------------ |
| Guid         | Character low GUID                               |
| ItemEntry    | The item entry                                   |
| ReceivedTime | Unix timestamp of the delivery/restore           |
| Source       | 0 = GM/store mail, 1 = restore token claim       |
