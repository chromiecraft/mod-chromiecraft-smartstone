--
-- Do not add DROP statements to this file.
-- It should not be reapplied.
-- DO NOT ChANGE THE DATABASE STRUCTURE, CHANGE AFTER (in another SQL) after the database is created and before the SQL queries related to that Database are ran.
--

CREATE TABLE IF NOT EXISTS `smartstone_pets` (
 `CreatureId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0, -- if set, pet will be available for free for that sub level
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`CreatureId`)
 );

CREATE TABLE IF NOT EXISTS `smartstone_costumes` (
 `Id` INT UNSIGNED NOT NULL,
 `DisplayId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0,
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`Id`)
 );

CREATE TABLE IF NOT EXISTS `smartstone_services` (
 `ServiceId` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`ServiceId`)
 );

CREATE TABLE IF NOT EXISTS `smartstone_categories` (
 `Id` INT UNSIGNED NOT NULL,
 `CategoryType` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `NPCTextId` INT DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`Id`)
);

CREATE TABLE IF NOT EXISTS `smartstone_auras` (
 `Id` INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
 `SpellID` INT UNSIGNED NOT NULL,
 `Description` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `Enabled` TINYINT NOT NULL DEFAULT 1
);
