DELETE FROM `module_string`        WHERE `module` = 'mod-cc-smartstone';
DELETE FROM `module_string_locale` WHERE `module` = 'mod-cc-smartstone';

INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
-- Item / gossip / player action messages (1-18)
('mod-cc-smartstone',  1, 'Invalid action selected.'),
('mod-cc-smartstone',  2, 'You cannot use this feature in battlegrounds or arenas.'),
('mod-cc-smartstone',  3, 'You cannot use this feature while in combat.'),
('mod-cc-smartstone',  4, 'You cannot use this feature while in PvP.'),
('mod-cc-smartstone',  5, 'Barbershop service is currently unavailable.'),
('mod-cc-smartstone',  6, 'You cannot use this feature inside instances or battlegrounds.'),
('mod-cc-smartstone',  7, 'The barber is already summoned.'),
('mod-cc-smartstone',  8, 'You cannot use this feature for another {} minute(s) and {} second(s).'),
('mod-cc-smartstone',  9, 'You already have this aura active.'),
('mod-cc-smartstone', 10, '{} is now active.'),
('mod-cc-smartstone', 11, 'You cannot summon a vehicle while in combat.'),
('mod-cc-smartstone', 12, 'You need Expert Riding skill to fly this vehicle.'),
('mod-cc-smartstone', 13, 'You may not fly here.'),
('mod-cc-smartstone', 14, 'You must be mounted to use this feature.'),
('mod-cc-smartstone', 15, 'Invalid action type selected.'),
('mod-cc-smartstone', 16, 'Smartstone is not properly configured. Please contact an administrator.'),
('mod-cc-smartstone', 17, 'Category not found.'),
('mod-cc-smartstone', 18, 'No actions available.'),
-- Singleton system messages (19)
('mod-cc-smartstone', 19, 'Your grasp on {} wanes, you can no longer summon that pet.'),
-- Command messages (20-59)
('mod-cc-smartstone', 20, 'The smartstone is disabled.'),
('mod-cc-smartstone', 21, 'Costume {} does not exist.'),
('mod-cc-smartstone', 22, 'You do not have access to ''{}''.'),
('mod-cc-smartstone', 23, 'You cannot use costumes in battlegrounds or arenas.'),
('mod-cc-smartstone', 24, 'You cannot use costumes while in combat.'),
('mod-cc-smartstone', 25, 'You cannot use costumes while in PvP.'),
('mod-cc-smartstone', 26, '''{}'' is on cooldown for {}m {}s.'),
('mod-cc-smartstone', 27, 'Costume ''{}'' applied.'),
('mod-cc-smartstone', 28, 'Pet {} does not exist.'),
('mod-cc-smartstone', 29, 'You cannot use combat pets inside instances or battlegrounds.'),
('mod-cc-smartstone', 30, 'Console usage requires a player name.'),
('mod-cc-smartstone', 31, '[{}] {} ({})'),
('mod-cc-smartstone', 32, '{} has no pets available.'),
('mod-cc-smartstone', 33, 'No pets found matching ''{}'' for {}.'),
('mod-cc-smartstone', 34, '[{}] {}'),
('mod-cc-smartstone', 35, '{} has no costumes available.'),
('mod-cc-smartstone', 36, 'No costumes found matching ''{}'' for {}.'),
('mod-cc-smartstone', 37, '[{}] ''{}'' - {}m {}s remaining.'),
('mod-cc-smartstone', 38, 'All costumes - {}m {}s remaining.'),
('mod-cc-smartstone', 39, 'Global cooldowns are not available for offline players.'),
('mod-cc-smartstone', 40, '{} has no active costume cooldowns.'),
('mod-cc-smartstone', 41, 'No pets are loaded.'),
('mod-cc-smartstone', 42, 'No pets found matching ''{}''.'),
('mod-cc-smartstone', 43, 'Costume conversion is not available.'),
('mod-cc-smartstone', 44, 'No legacy costume data is loaded.'),
('mod-cc-smartstone', 45, 'Could not find a matching costume for ''{}'' (display ID: {}).'),
('mod-cc-smartstone', 46, '''{}'' was already unlocked — item removed.'),
('mod-cc-smartstone', 47, 'Converted ''{}'' into ''{}''.'),
('mod-cc-smartstone', 48, 'No convertible costumes found in your inventory.'),
('mod-cc-smartstone', 49, 'You already have a smartstone.'),
('mod-cc-smartstone', 50, 'The {} is already unlocked.'),
('mod-cc-smartstone', 51, 'The {} is already locked.'),
('mod-cc-smartstone', 52, '{} has been unlocked for {}.'),
('mod-cc-smartstone', 53, '{} has been removed for {}.'),
('mod-cc-smartstone', 54, 'The pet {} does not exist.'),
('mod-cc-smartstone', 55, 'The costume {} does not exist.'),
('mod-cc-smartstone', 56, 'The aura {} does not exist.'),
('mod-cc-smartstone', 57, 'The vehicle {} does not exist.'),
('mod-cc-smartstone', 58, 'Unknown service type.'),
('mod-cc-smartstone', 59, 'Smartstone data reloaded.');

-- Locale translations can be added below.
-- Supported locales: koKR, frFR, deDE, zhCN, zhTW, esES, esMX, ruRU
-- Example:
-- INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
-- ('mod-cc-smartstone', 1, 'deDE', 'Ungültige Aktion ausgewählt.');
