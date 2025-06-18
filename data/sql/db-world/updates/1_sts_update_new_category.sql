ALTER TABLE `smartstone_categories` RENAME COLUMN `CategoryType` TO `ParentCategory`;
ALTER TABLE `smartstone_services` ADD COLUMN `Category` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `ServiceId`;
ALTER TABLE `smartstone_pets` ADD COLUMN `Type` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `CreatureId`;
