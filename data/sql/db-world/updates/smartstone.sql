--
DROP TABLE IF EXISTS `smartstone_pets`;
CREATE TABLE `smartstone_pets` (
 `CreatureId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0, -- if set, pet will be available for free for that sub level
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`CreatureId`)
 );
 
DROP TABLE IF EXISTS `smartstone_costumes`;
CREATE TABLE `smartstone_costumes` (
 `DisplayId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0,
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`DisplayId`)
 );
 
DROP TABLE IF EXISTS `smartstone_services`;
CREATE TABLE `smartstone_services` (
 `ServiceId` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`ServiceId`)
 );
 
DROP TABLE IF EXISTS `smartstone_categories`;
CREATE TABLE `smartstone_categories` (
 `Id` INT UNSIGNED NOT NULL,
 `CategoryType` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `NPCTextId` INT DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`Id`)
 );

DELETE FROM `smartstone_services`;
INSERT INTO `smartstone_services` (`ServiceId`, `Title`, `SubscriptionLevel`, `Enabled`) VALUES
(1, 'Request Barber Services', 2, 1),
(2, 'Rare Beasts of Azeroth', 0, 1),
(3, 'Limited Duration Companions', 0, 1),
(5, 'Costumes', 0, 1);

INSERT INTO `smartstone_categories` (`Id`, `CategoryType`, `Title`, `SubscriptionLevel`, `NPCTextId`, `Enabled`) VALUES
(1,0,'Azeroth Villains',0,92006,0);

DELETE FROM `smartstone_costumes`;
INSERT INTO `smartstone_costumes` (`DisplayId`, `Category`, `SubscriptionLevel`, `Duration`, `Description`, `Enabled`) VALUES
(2029,1,2,0,'Edwin VanCleef',1),
(2043,1,2,0,'High Inquisition Whitemane', 1);

UPDATE `item_template` SET `name` = "Smartstone", `ScriptName` = 'item_chromiecraft_smartstone', stackable = 1, `spellid_1` = 36177, maxcount = 1 WHERE (entry = 32547);
UPDATE `item_template_locale` SET `Name` = "Smartstone" WHERE (ID = 32547);

DELETE FROM `creature_template` WHERE `entry` = 80000;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `name`, `subname`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `dmgschool`, `DamageModifier`, `BaseAttackTime`, `RangeAttackTime`, `BaseVariance`, `RangeVariance`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `HoverHeight`, `HealthModifier`, `ManaModifier`, `ArmorModifier`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `spell_school_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
(80000, 0, 0, 0, 0, 0, 'Ripsy Blastfuse', 'Azeroth\'s Finest Hair Stylist', 92001, 70, 70, 0, 35, 1, 1, 1.14286, 1, 0, 0, 1, 2000, 2000, 1, 1, 1, 33536, 2048, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 1, 1.35, 1, 1, 0, 0, 1, 0, 0, 2, '', 0);

DELETE FROM `creature_template_model` WHERE `CreatureID` = 80000;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(80000, 0, 18969, 1, 1, 0);

DELETE FROM `creature_text` WHERE `CreatureID` = 80000;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Comment`) VALUES
(80000, 0, 0, 'Looking for a change, ay? I got just what you need! Come, take a seat.', 12, 0, 'Smartstone - Barber - Spawn'),
(80000, 1, 0, 'Time is up, pal! Off we go!', 12, 0, 'Smartstone - Barber - Despawn');

DELETE FROM `npc_text` WHERE `ID` IN (92000, 92001, 92002, 92003, 92004, 92005, 92006);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(92000, 'In the heart of Azeroth\'s magic, there exists a legendary artifact known as the Smartstone. This ancient gem, brimming with mystical energy and boundless wisdom, serves as a guide and companion to the brave and the curious. With a shimmer of its enchanted facets, the Smartstone can reveal hidden treasures, dispense sage advice, or provide crucial insights into the arcane and mundane alike. \n\nYet, its magic is not without its quirks; a flicker of its light might summon a mischievous imp or lead an adventurer to an unexpected adventure. \n\nThe Smartstone, ever playful and wise, stands ready to assist those who seek its guidance.'),
(92001, 'Hey there, pal! Welcome to the finest chop shop in Azeroth! Need a trim, a shave, or a whole new look? We’ve got styles to make your enemies quake in fear—or at least wonder where you got such a snazzy do! Sit down, relax, and let’s get you looking spectacular!'),
(92002, 'Scattered across Azeroth and beyond, these elusive creatures embody the essence of their environments. From spectral wolves prowling shadowed forests to fiery phoenixes soaring above molten peaks, each rare pet is a testament to the untamed beauty and danger of the world. Adventurers who seek them must be cunning, patient, and attuned to the whispers of the wild, for these beasts appear only to those who prove themselves worthy.'),
(92003, 'Whispered through the ages, these rare companions hail from Azeroth’s most elusive corners—be it deep jungles, ancient ruins, or shadowed realms. Each carries a spark of their origin’s mystery, offering unique abilities to amuse and amaze. But beware, their bond with mortals is fleeting, and their return to legend is inevitable. Claim them while their tale lingers!'),
(92004, 'Hey there, chum! Winter Veil enthusiast and purveyor of holiday cheer—Goblin-style at your service, if ya catch my drift. Ya look like someone who could use a little festive mischief. What d’ya say? Care to join ol’ Snizzlefizz for some presents, punch, and maybe a harmless boom or two? Ho, ho... ha-HA!"'),
(92005, 'Bound within this stone lies the magic of transformation, a power to reshape one’s form into that of legends and beasts alike. With but a touch, the enchantment takes hold, weaving illusion and essence together, granting the guise of heroes, monsters, and all manner of fabled beings. Whether to inspire awe or sow fear, the choice is yours — the veil of reality is but a whisper away'),
(92006, 'The line between myth and mortal blurs. This relic bends that line, allowing you to wear the shape of Azeroth’s darkest names. Appearances deceive — let yours terrify.');

DELETE FROM `gossip_menu` WHERE `MenuID` IN (92000, 92001, 92004);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(92000, 92000),
(92001, 92001),
(92004, 92004);

DELETE FROM `gossip_menu_option` WHERE `MenuID` = 92004;
INSERT INTO `gossip_menu_option` (`MenuID`, `OptionID`, `OptionIcon`, `OptionText`, `OptionType`, `OptionNpcFlag`, `ActionMenuID`) VALUES
(92004, 0, 0, "Okay, I think I had enough holiday cheer for now. Goodbye, little friend!", 1, 1, 0);

DELETE FROM `conditions` WHERE (`SourceTypeOrReferenceId` = 15) AND (`SourceGroup` = 92004) AND (`SourceEntry` = 0) AND (`SourceId` = 0) AND (`ConditionTypeOrReference` = 33);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(15, 92004, 0, 0, 0, 33, 1, 0, 3, 0, 0, 0, 0, '', 'Winterveil Helper - Only show gossip to summoner');

DELETE FROM `spell_dbc` WHERE `ID` IN (90000, 90001, 90002);
INSERT INTO `spell_dbc` (`ID`, `Category`, `DispelType`, `Mechanic`, `Attributes`, `AttributesEx`, `AttributesEx2`, `AttributesEx3`, `AttributesEx4`, `AttributesEx5`, `AttributesEx6`, `AttributesEx7`, `ShapeshiftMask`, `unk_320_2`, `ShapeshiftExclude`, `unk_320_3`, `Targets`, `TargetCreatureType`, `RequiresSpellFocus`, `FacingCasterFlags`, `CasterAuraState`, `TargetAuraState`, `ExcludeCasterAuraState`, `ExcludeTargetAuraState`, `CasterAuraSpell`, `TargetAuraSpell`, `ExcludeCasterAuraSpell`, `ExcludeTargetAuraSpell`, `CastingTimeIndex`, `RecoveryTime`, `CategoryRecoveryTime`, `InterruptFlags`, `AuraInterruptFlags`, `ChannelInterruptFlags`, `ProcTypeMask`, `ProcChance`, `ProcCharges`, `MaxLevel`, `BaseLevel`, `SpellLevel`, `DurationIndex`, `PowerType`, `ManaCost`, `ManaCostPerLevel`, `ManaPerSecond`, `ManaPerSecondPerLevel`, `RangeIndex`, `Speed`, `ModalNextSpell`, `CumulativeAura`, `Totem_1`, `Totem_2`, `Reagent_1`, `Reagent_2`, `Reagent_3`, `Reagent_4`, `Reagent_5`, `Reagent_6`, `Reagent_7`, `Reagent_8`, `ReagentCount_1`, `ReagentCount_2`, `ReagentCount_3`, `ReagentCount_4`, `ReagentCount_5`, `ReagentCount_6`, `ReagentCount_7`, `ReagentCount_8`, `EquippedItemClass`, `EquippedItemSubclass`, `EquippedItemInvTypes`, `Effect_1`, `Effect_2`, `Effect_3`, `EffectDieSides_1`, `EffectDieSides_2`, `EffectDieSides_3`, `EffectRealPointsPerLevel_1`, `EffectRealPointsPerLevel_2`, `EffectRealPointsPerLevel_3`, `EffectBasePoints_1`, `EffectBasePoints_2`, `EffectBasePoints_3`, `EffectMechanic_1`, `EffectMechanic_2`, `EffectMechanic_3`, `ImplicitTargetA_1`, `ImplicitTargetA_2`, `ImplicitTargetA_3`, `ImplicitTargetB_1`, `ImplicitTargetB_2`, `ImplicitTargetB_3`, `EffectRadiusIndex_1`, `EffectRadiusIndex_2`, `EffectRadiusIndex_3`, `EffectAura_1`, `EffectAura_2`, `EffectAura_3`, `EffectAuraPeriod_1`, `EffectAuraPeriod_2`, `EffectAuraPeriod_3`, `EffectMultipleValue_1`, `EffectMultipleValue_2`, `EffectMultipleValue_3`, `EffectChainTargets_1`, `EffectChainTargets_2`, `EffectChainTargets_3`, `EffectItemType_1`, `EffectItemType_2`, `EffectItemType_3`, `EffectMiscValue_1`, `EffectMiscValue_2`, `EffectMiscValue_3`, `EffectMiscValueB_1`, `EffectMiscValueB_2`, `EffectMiscValueB_3`, `EffectTriggerSpell_1`, `EffectTriggerSpell_2`, `EffectTriggerSpell_3`, `EffectPointsPerCombo_1`, `EffectPointsPerCombo_2`, `EffectPointsPerCombo_3`, `EffectSpellClassMaskA_1`, `EffectSpellClassMaskA_2`, `EffectSpellClassMaskA_3`, `EffectSpellClassMaskB_1`, `EffectSpellClassMaskB_2`, `EffectSpellClassMaskB_3`, `EffectSpellClassMaskC_1`, `EffectSpellClassMaskC_2`, `EffectSpellClassMaskC_3`, `SpellVisualID_1`, `SpellVisualID_2`, `SpellIconID`, `ActiveIconID`, `SpellPriority`, `Name_Lang_enUS`, `Name_Lang_enGB`, `Name_Lang_koKR`, `Name_Lang_frFR`, `Name_Lang_deDE`, `Name_Lang_enCN`, `Name_Lang_zhCN`, `Name_Lang_enTW`, `Name_Lang_zhTW`, `Name_Lang_esES`, `Name_Lang_esMX`, `Name_Lang_ruRU`, `Name_Lang_ptPT`, `Name_Lang_ptBR`, `Name_Lang_itIT`, `Name_Lang_Unk`, `Name_Lang_Mask`, `NameSubtext_Lang_enUS`, `NameSubtext_Lang_enGB`, `NameSubtext_Lang_koKR`, `NameSubtext_Lang_frFR`, `NameSubtext_Lang_deDE`, `NameSubtext_Lang_enCN`, `NameSubtext_Lang_zhCN`, `NameSubtext_Lang_enTW`, `NameSubtext_Lang_zhTW`, `NameSubtext_Lang_esES`, `NameSubtext_Lang_esMX`, `NameSubtext_Lang_ruRU`, `NameSubtext_Lang_ptPT`, `NameSubtext_Lang_ptBR`, `NameSubtext_Lang_itIT`, `NameSubtext_Lang_Unk`, `NameSubtext_Lang_Mask`, `Description_Lang_enUS`, `Description_Lang_enGB`, `Description_Lang_koKR`, `Description_Lang_frFR`, `Description_Lang_deDE`, `Description_Lang_enCN`, `Description_Lang_zhCN`, `Description_Lang_enTW`, `Description_Lang_zhTW`, `Description_Lang_esES`, `Description_Lang_esMX`, `Description_Lang_ruRU`, `Description_Lang_ptPT`, `Description_Lang_ptBR`, `Description_Lang_itIT`, `Description_Lang_Unk`, `Description_Lang_Mask`, `AuraDescription_Lang_enUS`, `AuraDescription_Lang_enGB`, `AuraDescription_Lang_koKR`, `AuraDescription_Lang_frFR`, `AuraDescription_Lang_deDE`, `AuraDescription_Lang_enCN`, `AuraDescription_Lang_zhCN`, `AuraDescription_Lang_enTW`, `AuraDescription_Lang_zhTW`, `AuraDescription_Lang_esES`, `AuraDescription_Lang_esMX`, `AuraDescription_Lang_ruRU`, `AuraDescription_Lang_ptPT`, `AuraDescription_Lang_ptBR`, `AuraDescription_Lang_itIT`, `AuraDescription_Lang_Unk`, `AuraDescription_Lang_Mask`, `ManaCostPct`, `StartRecoveryCategory`, `StartRecoveryTime`, `MaxTargetLevel`, `SpellClassSet`, `SpellClassMask_1`, `SpellClassMask_2`, `SpellClassMask_3`, `MaxTargets`, `DefenseType`, `PreventionType`, `StanceBarOrder`, `EffectChainAmplitude_1`, `EffectChainAmplitude_2`, `EffectChainAmplitude_3`, `MinFactionID`, `MinReputation`, `RequiredAuraVision`, `RequiredTotemCategoryID_1`, `RequiredTotemCategoryID_2`, `RequiredAreasID`, `SchoolMask`, `RuneCostID`, `SpellMissileID`, `PowerDisplayID`, `EffectBonusMultiplier_1`, `EffectBonusMultiplier_2`, `EffectBonusMultiplier_3`, `SpellDescriptionVariableID`, `SpellDifficultyID`) VALUE
(90000,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,28,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,190011,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Summon Exotic Pet','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Right Click to summon and dismiss your Exotic Pet.','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(90001,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,28,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,190011,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Summon Combat Pet','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Right Click to summon and dismiss your Exotic Pet.','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
(90002,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Costume Cooldown Marker (DUMMY)','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Costume cooldown marker (dummy spell).','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

UPDATE `spell_dbc` SET `EffectMiscValueB_1` = 67 WHERE `ID` = 90001;

SET
@Entry := 80001,
@Name := "Amani'shi Groundbreaker",
@Model := 22353,
@Scale := 0.30,
@SubLevel := 2,
@Description := "Summon Amani'shi Groundbreaker";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80002,
@Name := "Hyjal Wisp",
@Model := 10045,
@Scale := 0.3,
@Description := "Summon Hyjal Wisp";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Description`) VALUES
(@Entry, 1, @Description);
 
SET
@Entry := 80003,
@Name := "Serpentshrine Waterspawn",
@Model := 20076,
@Scale := 0.2,
@Description := "Summon Serpentshrine Waterspawn";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Description`) VALUES
(@Entry, 1, @Description);

SET
@Entry := 80004,
@Name := "Vashj'ir Enforcer",
@Model := 20200,
@Scale := 0.2,
@SubLevel := 2,
@Description := "Summon Vashj'ir Enforcer";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80005,
@Name := "Infernaling",
@Model := 12817,
@Scale := 0.2,
@SubLevel := 2,
@Description := "Summon Infernaling";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80006,
@Name := "Thunderwing",
@Model := 22255,
@Scale := 0.40,
@Description := "Summon Thunderwing";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Description`) VALUES
(@Entry, 1, @Description);

-- Thunderwing AI
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` = 80006;

DELETE FROM `smart_scripts` WHERE (`source_type` = 0 AND `entryorguid` = 80006);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `event_param6`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(80006, 0, 0, 0, 1, 0, 100, 1, 1, 1, 0, 0, 0, 0, 11, 37964, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Thunderwing - Out of Combat - Cast \'Call Lightning (Cosmetic)\' (No Repeat)'),
(80006, 0, 1, 0, 1, 0, 100, 0, 3000, 6000, 900000, 1800000, 0, 0, 75, 32910, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Thunderwing - Out of Combat - Add Aura \'Windfury\'');

SET
@Entry := 80007,
@Name := "Wretched Servant",
@Model := 22845,
@Scale := 0.50,
@SubLevel := 2,
@Description := "Summon Wretched Servant";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80008,
@Name := "Mistress of Suffering",
@Model := 21503,
@Scale := 0.10,
@SubLevel := 2,
@Description := "Summon Mistress of Suffering";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80009,
@Name := "Sunblade Mini Protector",
@Model := 23240,
@Scale := 0.10,
@SubLevel := 2,
@Description := "Summon Sunblade Mini Protector";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 80010,
@Name := "Alythessa",
@Model := 23334,
@Scale := 0.20,
@SubLevel := 0,
@Description := "Summon Alythessa";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

DELETE FROM `creature_template_addon` WHERE `entry` = @ENTRY;
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `visibilityDistanceType`, `auras`) VALUES
(@ENTRY, 0, 0, 0, 0, 0, 0, '45457');

SET
@Entry := 80011,
@Name := "Scorchling of Azzinoth",
@Model := 21321,
@Scale := 0.40,
@SubLevel := 0,
@Description := "Summon Scorchling of Azzinoth";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `SubscriptionLevel`, `Description`) VALUES
(@Entry, 1, @SubLevel, @Description);

SET
@Entry := 90001,
@Name := "Winterveil Helper",
@Model := 19342,
@Scale := 1,
@Description := "Summon Winter Reveler";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 768, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
DELETE FROM `creature_template_spell`  WHERE `CreatureId` = @Entry; 
INSERT INTO `creature_template_spell` (`CreatureId`, `Index`, `Spell`) VALUES
(@Entry, 0, 46661),
(@Entry, 1, 44755);
 
DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Category`, `Enabled`, `Description`) VALUES
(@Entry, 1, 1, @Description);
