--
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

DELETE FROM `npc_text` WHERE `ID` IN (92000, 92001);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(92000, 'In the heart of Azeroth\'s magic, there exists a legendary artifact known as the Smartstone. This ancient gem, brimming with mystical energy and boundless wisdom, serves as a guide and companion to the brave and the curious. With a shimmer of its enchanted facets, the Smartstone can reveal hidden treasures, dispense sage advice, or provide crucial insights into the arcane and mundane alike. \n\nYet, its magic is not without its quirks; a flicker of its light might summon a mischievous imp or lead an adventurer to an unexpected adventure. \n\nThe Smartstone, ever playful and wise, stands ready to assist those who seek its guidance.'),
(92001, 'Hey there, pal! Welcome to the finest chop shop in Azeroth! Need a trim, a shave, or a whole new look? We’ve got styles to make your enemies quake in fear—or at least wonder where you got such a snazzy do! Sit down, relax, and let’s get you looking spectacular!');

DELETE FROM `gossip_menu` WHERE `MenuID` IN (92000, 92001);
INSERT INTO `gossip_menu` (`MenuID`, `TextID`) VALUES
(92000, 92000),
(92001, 92001);
