-- ADD always "IF NOT EXISTS" between ADD COLUMN and Table name
ALTER TABLE `smartstone_categories` RENAME COLUMN IF NOT EXISTS `CategoryType` TO `ParentCategory`;
ALTER TABLE `smartstone_services` ADD COLUMN IF NOT EXISTS `Category` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `ServiceId`;
ALTER TABLE `smartstone_pets` ADD COLUMN IF NOT EXISTS `Type` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `CreatureId`;
ALTER TABLE `smartstone_costumes` ADD COLUMN IF NOT EXISTS `Scale` FLOAT NOT NULL DEFAULT 1 AFTER `DisplayId`;
