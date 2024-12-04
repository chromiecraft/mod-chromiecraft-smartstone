--
SET
@Entry := 90001,
@Name := "Winterveil Helper",
@Model := 19342,
@Scale := 1,
@Gossip := 92004,
@AIName := "SmartAI",
@Description := "Summon Winter Helper (7 days)";

DELETE FROM `creature_template` WHERE `entry` = @Entry;
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(@Entry, @Name, NULL, NULL, @Gossip, 1, 1, 2, 35, 1, @Scale, 0, 0, 2000, 0, 1, 768, 7, 0, 0, 0, 0, @AIName, 0, 1, 0, 0, 1, 0, 0, '');

DELETE FROM `creature_template_model` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`, `VerifiedBuild`) VALUES
(@Entry, 0, @Model, @Scale, 1, 0);
 
DELETE FROM `creature_template_spell`  WHERE `CreatureId` = @Entry; 
INSERT INTO `creature_template_spell` (`CreatureId`, `Index`, `Spell`) VALUES
(@Entry, 0, 46661),
(@Entry, 1, 44755),
(@Entry, 2, 3714),
 
DELETE FROM `smartstone_pets` WHERE `CreatureId` = @Entry;
INSERT INTO `smartstone_pets` (`CreatureId`, `Category`, `Enabled`, `Description`) VALUES
(@Entry, 1, 1, @Description);

DELETE FROM `creature_text` WHERE `CreatureID` = @Entry;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Text`, `Type`, `Language`, `Probability`, `Emote`, `TextRange`, `comment`) VALUES
(@Entry, 0, 0, "Whoo-hoo, made it just in time! Snow’s fresh, the cider’s warm, and I’ve got a sack full o’ surprises. Now, where’s the action? Let’s get this Winter Veil started!", 12, 0, 100, 11, 0, 'Winterveil Helper - On Spawn'),
(@Entry, 0, 1, "Ah, just in time! The snow’s fallin’, the fires are burnin’, and I’ve got a few surprises ready to go. Let’s make this Winter Veil one for the history books, eh? Come on, don’t stand there like a draenei at a goblin market—let’s get to the fun!", 12, 0, 100, 11, 0, 'Winterveil Helper - On Spawn');

DELETE FROM `smart_scripts` WHERE (`entryorguid` = 90001) AND (`source_type` = 0) AND (`id` IN (0, 1));
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `event_param6`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_param4`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(90001, 0, 0, 1, 60, 0, 100, 1, 1000, 1000, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Winterveil Helper - On Update - Say Line 0 (No Repeat)'),
(90001, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 0, 11, 25996, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Winterveil Helper - On Update - Cast \'Snowball Lasting Visual\' (No Repeat)');
