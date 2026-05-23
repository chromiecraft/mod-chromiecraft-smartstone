--
-- Class Perks: new content type with one parent category and 10 hardcoded
-- per-class child categories. Class-gating happens in C++; no ClassMask
-- column is needed on smartstone_categories.
--
-- npc_text row 92008 (gossip flavour for category 1000 "Class Perks")
-- lives alongside the existing smartstone npc_texts in 2_sts_data.sql.
--

CREATE TABLE IF NOT EXISTS `smartstone_perks` (
  `Id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
  `Title` TEXT,
  `Description` TEXT,
  `ClassId` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `Category` INT UNSIGNED NOT NULL DEFAULT 0,
  `Effect` TINYINT UNSIGNED NOT NULL DEFAULT 0,  -- numeric discriminator routed in C++ (see SmartstonePerkEffect)
  `Value` INT UNSIGNED NOT NULL DEFAULT 0,       -- effect-specific payload (e.g. a displayId)
  `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `Enabled` TINYINT NOT NULL DEFAULT 1
);

-- Parent "Class Perks" category + 10 per-class subcategories.
-- IDs 1000-1010 are reserved for class perks. ParentCategory = 0
-- (CATEGORY_MAIN) for the root entry; ParentCategory = 1000 for
-- each class subcategory.
DELETE FROM `smartstone_categories` WHERE `Id` BETWEEN 1000 AND 1010;
INSERT INTO `smartstone_categories`
    (`Id`, `ParentCategory`, `Title`, `SubscriptionLevel`, `NPCTextId`, `Enabled`)
VALUES
    (1000, 0,    '|TInterface/icons/Ability_Paladin_BeaconofLight:30:30:-18:0|t Class Perks', 0, 92008, 1),
    -- Non-druid subcategories ship disabled until they have content.
    -- Flip `Enabled` to 1 once perks are added.
    (1001, 1000, 'Warrior Perks',     0, 92009, 0),
    (1002, 1000, 'Paladin Perks',     0, 92010, 0),
    (1003, 1000, 'Hunter Perks',      0, 92011, 0),
    (1004, 1000, 'Rogue Perks',       0, 92012, 0),
    (1005, 1000, 'Priest Perks',      0, 92013, 0),
    (1006, 1000, 'Death Knight Perks',0, 92014, 0),
    (1007, 1000, '|TInterface/icons/Spell_Nature_SpiritWolf:30:30:-18:0|t Shaman Perks',      0, 92015, 1),
    (1008, 1000, 'Mage Perks',        0, 92016, 0),
    (1009, 1000, 'Warlock Perks',     0, 92017, 0),
    (1010, 1000, '|TInterface/icons/Spell_Nature_Regeneration:30:30:-18:0|t Druid Perks', 0, 92018, 1);

-- Seed perk rows live in mod-chromiecraft/data/sql/db-world/updates/8_smartstone_perks.sql
-- (data lives next to other smartstone seed data; prefix 8_ ensures it
-- sorts alphabetically after this file across modules).
