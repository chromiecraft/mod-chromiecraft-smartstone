--
DROP TABLE IF EXISTS `smartstone_pets`;
CREATE TABLE `smartstone_pets` (
 `CreatureId` INT UNSIGNED NOT NULL,
 `Enabled` TINYINT DEFAULT 1,
 `Description` TEXT,
 PRIMARY KEY(`CreatureId`)
 );
 
DROP TABLE IF EXISTS `smartstone_services`;
CREATE TABLE `smartstone_services` (
 `ServiceId` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` INT UNSIGNED NOT NULL,
 `Enabled` TINYINT,
 PRIMARY KEY(`ServiceId`)
 );

INSERT INTO `smartstone_services` (`ServiceId`, `Title`, `SubscriptionLevel`, `Enabled`) VALUES
(1, 'Request Barber Services', 1, 1),
(2, 'Rare Beasts of Azeroth', 0, 1);

UPDATE `item_template` SET `name` = "Smartstone", `ScriptName` = 'item_chromiecraft_smartstone', stackable = 1, `spellid_1` = 36177, maxcount = 1 WHERE (entry = 32547);

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

DELETE FROM `npc_text` WHERE `ID` IN (92000, 92001, 92002);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(92000, 'In the heart of Azeroth\'s magic, there exists a legendary artifact known as the Smartstone. This ancient gem, brimming with mystical energy and boundless wisdom, serves as a guide and companion to the brave and the curious. With a shimmer of its enchanted facets, the Smartstone can reveal hidden treasures, dispense sage advice, or provide crucial insights into the arcane and mundane alike. \n\nYet, its magic is not without its quirks; a flicker of its light might summon a mischievous imp or lead an adventurer to an unexpected adventure. \n\nThe Smartstone, ever playful and wise, stands ready to assist those who seek its guidance.'),
(92001, 'Hey there, pal! Welcome to the finest chop shop in Azeroth! Need a trim, a shave, or a whole new look? We’ve got styles to make your enemies quake in fear—or at least wonder where you got such a snazzy do! Sit down, relax, and let’s get you looking spectacular!'),
(92002, 'Scattered across Azeroth and beyond, these elusive creatures embody the essence of their environments. From spectral wolves prowling shadowed forests to fiery phoenixes soaring above molten peaks, each rare pet is a testament to the untamed beauty and danger of the world. Adventurers who seek them must be cunning, patient, and attuned to the whispers of the wild, for these beasts appear only to those who prove themselves worthy.');

DELETE FROM `gossip_menu` WHERE `MenuID` IN (92000, 92001);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(92000, 92000),
(92001, 92001);

DELETE FROM `spell_dbc` WHERE `ID` = 90000;
INSERT INTO `spell_dbc` (`ID`, `Category`, `DispelType`, `Mechanic`, `Attributes`, `AttributesEx`, `AttributesEx2`, `AttributesEx3`, `AttributesEx4`, `AttributesEx5`, `AttributesEx6`, `AttributesEx7`, `ShapeshiftMask`, `unk_320_2`, `ShapeshiftExclude`, `unk_320_3`, `Targets`, `TargetCreatureType`, `RequiresSpellFocus`, `FacingCasterFlags`, `CasterAuraState`, `TargetAuraState`, `ExcludeCasterAuraState`, `ExcludeTargetAuraState`, `CasterAuraSpell`, `TargetAuraSpell`, `ExcludeCasterAuraSpell`, `ExcludeTargetAuraSpell`, `CastingTimeIndex`, `RecoveryTime`, `CategoryRecoveryTime`, `InterruptFlags`, `AuraInterruptFlags`, `ChannelInterruptFlags`, `ProcTypeMask`, `ProcChance`, `ProcCharges`, `MaxLevel`, `BaseLevel`, `SpellLevel`, `DurationIndex`, `PowerType`, `ManaCost`, `ManaCostPerLevel`, `ManaPerSecond`, `ManaPerSecondPerLevel`, `RangeIndex`, `Speed`, `ModalNextSpell`, `CumulativeAura`, `Totem_1`, `Totem_2`, `Reagent_1`, `Reagent_2`, `Reagent_3`, `Reagent_4`, `Reagent_5`, `Reagent_6`, `Reagent_7`, `Reagent_8`, `ReagentCount_1`, `ReagentCount_2`, `ReagentCount_3`, `ReagentCount_4`, `ReagentCount_5`, `ReagentCount_6`, `ReagentCount_7`, `ReagentCount_8`, `EquippedItemClass`, `EquippedItemSubclass`, `EquippedItemInvTypes`, `Effect_1`, `Effect_2`, `Effect_3`, `EffectDieSides_1`, `EffectDieSides_2`, `EffectDieSides_3`, `EffectRealPointsPerLevel_1`, `EffectRealPointsPerLevel_2`, `EffectRealPointsPerLevel_3`, `EffectBasePoints_1`, `EffectBasePoints_2`, `EffectBasePoints_3`, `EffectMechanic_1`, `EffectMechanic_2`, `EffectMechanic_3`, `ImplicitTargetA_1`, `ImplicitTargetA_2`, `ImplicitTargetA_3`, `ImplicitTargetB_1`, `ImplicitTargetB_2`, `ImplicitTargetB_3`, `EffectRadiusIndex_1`, `EffectRadiusIndex_2`, `EffectRadiusIndex_3`, `EffectAura_1`, `EffectAura_2`, `EffectAura_3`, `EffectAuraPeriod_1`, `EffectAuraPeriod_2`, `EffectAuraPeriod_3`, `EffectMultipleValue_1`, `EffectMultipleValue_2`, `EffectMultipleValue_3`, `EffectChainTargets_1`, `EffectChainTargets_2`, `EffectChainTargets_3`, `EffectItemType_1`, `EffectItemType_2`, `EffectItemType_3`, `EffectMiscValue_1`, `EffectMiscValue_2`, `EffectMiscValue_3`, `EffectMiscValueB_1`, `EffectMiscValueB_2`, `EffectMiscValueB_3`, `EffectTriggerSpell_1`, `EffectTriggerSpell_2`, `EffectTriggerSpell_3`, `EffectPointsPerCombo_1`, `EffectPointsPerCombo_2`, `EffectPointsPerCombo_3`, `EffectSpellClassMaskA_1`, `EffectSpellClassMaskA_2`, `EffectSpellClassMaskA_3`, `EffectSpellClassMaskB_1`, `EffectSpellClassMaskB_2`, `EffectSpellClassMaskB_3`, `EffectSpellClassMaskC_1`, `EffectSpellClassMaskC_2`, `EffectSpellClassMaskC_3`, `SpellVisualID_1`, `SpellVisualID_2`, `SpellIconID`, `ActiveIconID`, `SpellPriority`, `Name_Lang_enUS`, `Name_Lang_enGB`, `Name_Lang_koKR`, `Name_Lang_frFR`, `Name_Lang_deDE`, `Name_Lang_enCN`, `Name_Lang_zhCN`, `Name_Lang_enTW`, `Name_Lang_zhTW`, `Name_Lang_esES`, `Name_Lang_esMX`, `Name_Lang_ruRU`, `Name_Lang_ptPT`, `Name_Lang_ptBR`, `Name_Lang_itIT`, `Name_Lang_Unk`, `Name_Lang_Mask`, `NameSubtext_Lang_enUS`, `NameSubtext_Lang_enGB`, `NameSubtext_Lang_koKR`, `NameSubtext_Lang_frFR`, `NameSubtext_Lang_deDE`, `NameSubtext_Lang_enCN`, `NameSubtext_Lang_zhCN`, `NameSubtext_Lang_enTW`, `NameSubtext_Lang_zhTW`, `NameSubtext_Lang_esES`, `NameSubtext_Lang_esMX`, `NameSubtext_Lang_ruRU`, `NameSubtext_Lang_ptPT`, `NameSubtext_Lang_ptBR`, `NameSubtext_Lang_itIT`, `NameSubtext_Lang_Unk`, `NameSubtext_Lang_Mask`, `Description_Lang_enUS`, `Description_Lang_enGB`, `Description_Lang_koKR`, `Description_Lang_frFR`, `Description_Lang_deDE`, `Description_Lang_enCN`, `Description_Lang_zhCN`, `Description_Lang_enTW`, `Description_Lang_zhTW`, `Description_Lang_esES`, `Description_Lang_esMX`, `Description_Lang_ruRU`, `Description_Lang_ptPT`, `Description_Lang_ptBR`, `Description_Lang_itIT`, `Description_Lang_Unk`, `Description_Lang_Mask`, `AuraDescription_Lang_enUS`, `AuraDescription_Lang_enGB`, `AuraDescription_Lang_koKR`, `AuraDescription_Lang_frFR`, `AuraDescription_Lang_deDE`, `AuraDescription_Lang_enCN`, `AuraDescription_Lang_zhCN`, `AuraDescription_Lang_enTW`, `AuraDescription_Lang_zhTW`, `AuraDescription_Lang_esES`, `AuraDescription_Lang_esMX`, `AuraDescription_Lang_ruRU`, `AuraDescription_Lang_ptPT`, `AuraDescription_Lang_ptBR`, `AuraDescription_Lang_itIT`, `AuraDescription_Lang_Unk`, `AuraDescription_Lang_Mask`, `ManaCostPct`, `StartRecoveryCategory`, `StartRecoveryTime`, `MaxTargetLevel`, `SpellClassSet`, `SpellClassMask_1`, `SpellClassMask_2`, `SpellClassMask_3`, `MaxTargets`, `DefenseType`, `PreventionType`, `StanceBarOrder`, `EffectChainAmplitude_1`, `EffectChainAmplitude_2`, `EffectChainAmplitude_3`, `MinFactionID`, `MinReputation`, `RequiredAuraVision`, `RequiredTotemCategoryID_1`, `RequiredTotemCategoryID_2`, `RequiredAreasID`, `SchoolMask`, `RuneCostID`, `SpellMissileID`, `PowerDisplayID`, `EffectBonusMultiplier_1`, `EffectBonusMultiplier_2`, `EffectBonusMultiplier_3`, `SpellDescriptionVariableID`, `SpellDifficultyID`) VALUE
(90000,0,0,0,262416,0,0,536870912,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,31,0,0,0,101,0,0,0,0,21,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,28,0,0,1,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,0,0,0,0,0,0,0,0,190011,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,458,0,2808,0,0,'Summon Exotic Pet','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,'Right Click to summon and dismiss your Exotic Pet.','','','','','','','','','','','','','','','',16712190,'','','','','','','','','','','','','','','','',16712190,0,133,1500,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

SET
@Entry := 80001,
@Name := "Amani'shi Bruteling",
@Model := 22353,
@Scale := 0.30;
@Description := "Summon Amani'shi Bruteling";
DELETE FROM `creature_template` WHERE `entry` = @Entry;

INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, 0, 1, 1, 2, 35, 0, @Scale, 0, 0, 2000, 0, 1, 0, 7, 0, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
INSERT INTO `smartstone_pets` (`CreatureId`, `Enabled`, `Description`) VALUES
(80001, 1, @Description);
 
 