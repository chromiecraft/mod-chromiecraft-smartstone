--
-- Do not add DROP statements to this file.
-- It should not be reapplied.
--

CREATE TABLE `smartstone_pets` (
 `CreatureId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0, -- if set, pet will be available for free for that sub level
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`CreatureId`)
 );

CREATE TABLE `smartstone_costumes` (
 `Id` INT UNSIGNED NOT NULL,
 `DisplayId` INT UNSIGNED NOT NULL,
 `Category` TINYINT DEFAULT 0,
 `SubscriptionLevel` TINYINT DEFAULT 0,
 `Duration` INT UNSIGNED NOT NULL DEFAULT 0,
 `Description` TEXT,
 `Enabled` TINYINT DEFAULT 1,
 PRIMARY KEY(`Id`)
 );

CREATE TABLE `smartstone_services` (
 `ServiceId` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`ServiceId`)
 );

CREATE TABLE `smartstone_categories` (
 `Id` INT UNSIGNED NOT NULL,
 `CategoryType` INT UNSIGNED NOT NULL,
 `Title` TEXT,
 `SubscriptionLevel` TINYINT UNSIGNED NOT NULL DEFAULT 0,
 `NPCTextId` INT DEFAULT 0,
 `Enabled` TINYINT,
 PRIMARY KEY(`Id`)
 );
