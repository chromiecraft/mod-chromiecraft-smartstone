# Chromiecraft's Smartstone Module

## About

This module was inspired by Azerothshard's Smartstone module.

The Smartstone is a versatile item that offers a wide range of gossip options, enabling you to provide various services and entertaining features to players.

This module makes use of Azerothcore's subscription system (from [acore-cms](https://github.com/azerothcore/acore-cms/) intregated subscription system.)

## How to use this?

> [!WARNING]  
> This module changes an existing Item "Tier 5 Mage Test Gear" (32547) to be named Smartstone and use the smartstone scripts.

> [!NOTE]
> This module requires you to have `EnablePlayerSettings = 1` in your `worldserver.conf` to work, the information gets stored in `character_settings` in your `acore_characters`.

| Category ID     | Catergory Name    | Catergory Range | Command Value           | Database Table      |
| --------------- | ----------------- | --------------- | ----------------------- | ------------------- |
| 0 (PetID)       | Pets / Companions | 80001 - 89999   | PetID minus 80000       | smartstone_pets     |
| 1 (CombatPetID) | Combat Pets       | 90001 - 99999   | CombatPetID minus 90000 | smartstone_pets     |
| 2 (CostumeID)   | Costumes          | 20001 - 29999   | CostumeID minus 20000   | smartstone_costumes |

The command usage in-game or via the worldserver:

`.smartstone unlock service $characterName (or $characterGUID) $categoryID $petOrCostumeID $true (to add, or $false to remove)`.

`.smartstone unlock service Nyeriah 0 1 true`
Unlocks `(Pet) Amani'shi Groundbreaker`

`.smartstone unlock service Nyeriah 1 1 true`
Unlocks `(Combat Pet) Winterveil Helper`

`.smartstone unlock service Nyeriah 2 1 true`
Unlocks `(Costume) Edwin VanCleef`

> [!NOTE]
> Some of the existing pets as example `Summon Hyjal Wisp`, will not work if you haven't completed their required raids in mythic from [mod-zone-difficulty](https://github.com/azerothcore/mod-zone-difficulty).

## How to make your own?

### Pet

Requires `creature_template`, `creature_template_model` and `smartstone_pets`.

Example for `Amani'shi Groundbreaker` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/master/data/sql/db-world/updates/smartstone.sql#L108).

### Combat Pet

Requires `creature_template`, `creature_template_model`, `creature_template_spell` and `smartstone_pets`.

Example for `Winterveil Helper` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/master/data/sql/db-world/updates/smartstone.sql#L326).

### Costumes

Requires only `smartstone_costumes`.

Example for `Edwin VanCleef` [here](https://github.com/chromiecraft/mod-chromiecraft-smartstone/blob/master/data/sql/db-world/updates/smartstone.sql#L56).

## Documentation

### smartstone_pets (acore_world)

| CreatureId     | Category | SubscriptionLevel | Duration          | Description | Enabled              |
| -------------- | -------- | ----------------- | ----------------- | ----------- | -------------------- |
| 80001 to 99999 | 0 or 1   | 1 to 3            | In Unix Timestamp | Gossip Text | 1 (Show) or 0 (Hide) |

### smartstone_costumes (acore_world)

| Id             | [DisplayId](https://www.azerothcore.org/wiki/creature_template_model#creaturedisplayid) | Category | SubscriptionLevel | Duration   | Description | Enabled              |
| -------------- | --------------------------------------------------------------------------------------- | -------- | ----------------- | ---------- | ----------- | -------------------- |
| 20001 to 29999 | ModelID                                                                                 | 1        | 1 to 3            | In seconds | Gossip Text | 1 (Show) or 0 (Hide) |

### smartstone_categories (acore_world)

| Id                | CategoryType | Title                        | SubscriptionLevel | [NPCTextId](https://www.azerothcore.org/wiki/npc_text#id) | Enabled              |
| ----------------- | ------------ | ---------------------------- | ----------------- | --------------------------------------------------------- | -------------------- |
| Unique Identifier | 0            | Title that shows a sub-menu? | 1 to 3            | npc_text.id                                               | 1 (Show) or 0 (Hide) |

