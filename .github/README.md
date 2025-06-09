# Chromiecraft's Smartstone Module

## About

This module was inspired by Azerothshard's Smartstone module.

The Smartstone is a versatile item that offers a wide range of gossip options, enabling you to provide various services and entertaining features to players.

This module makes use of Azerothcore's subscription system (from [acore-cms](https://github.com/azerothcore/acore-cms/) intregated subscription system.)

## How to use this?

> [!WARNING]  
> This module changes an existing Item "Tier 5 Mage Test Gear" (32547) to be named Smartstone and use the smartstone scripts.

| Category ID | Catergory Name    | Catergory Range | Command Value       | Database Table      |
| ----------- | ----------------- | --------------- | ------------------- | ------------------- |
| 0           | Pets / Companions | 80001 - 89999   | PetID - 80000       | smartstone_pets     |
| 1           | Combat Pets       | 90001 - 99999   | CombatPetID - 90000 | smartstone_pets     |
| 2           | Costumes          | 20001 - 29999   | CostumesID - 20000  | smartstone_costumes |

When using the command for pets they have their respective "formula",

The command usage in-game or via the worldserver:

`.smartstone unlock service $characterName (or $characterGUID) $categoryID $petOrCostumeID $true (to add, or $false to remove)`.

`.smartstone unlock service Nyeriah 0 1 true`
Unlocks `Amani'shi Groundbreaker`
