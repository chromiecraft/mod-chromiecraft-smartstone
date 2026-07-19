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

Tokens are one-shot, account-wide grants of a **character service**: a name change, faction change, race change, or appearance (customize) change. A moderator grants a token to an account; any character on that account can then claim it. Claiming sets the matching at-login flag, so the actual change happens at the character selection screen the next time the player logs out to it.

Tokens are stored in `acore_auth` (`smartstone_account_tokens`), so a granted token is available on every realm and to every character on the account until claimed. Claimed tokens are kept (not deleted) as an audit record.

### Token types

| Type | Service           | At-login effect          |
| ---- | ----------------- | ------------------------ |
| 1    | Name Change       | `AT_LOGIN_RENAME`        |
| 2    | Faction Change    | `AT_LOGIN_CHANGE_FACTION`|
| 3    | Race Change       | `AT_LOGIN_CHANGE_RACE`   |
| 4    | Appearance Change | `AT_LOGIN_CUSTOMIZE`     |

### Commands

Moderator commands (also usable from the worldserver console). The account can be given by name or account id:

```bash
# Grant a Faction Change (type 2) token to an account
.smartstone token grant Nyeriah 2

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
| TokenType    | 1-4 (see [Tokens](#tokens))                            |
| GrantedBy      | Granting account id (0 = console)                          |
| GrantedTime    | Unix timestamp of the grant                                |
| ConsumedByGUID | Character GUID that claimed it (0 = unclaimed)             |
| ConsumedTime   | Unix timestamp of the claim (0 = unclaimed)                |
