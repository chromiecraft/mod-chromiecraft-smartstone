-- ADD always "IF NOT EXISTS" between ADD COLUMN and Table name for the cases of adding to a table. 
-- Use the example how to prevent errors in the cases that commands DO NOT support "IF NOT EXISTS" by usage of procedures.

DELIMITER // 
CREATE PROCEDURE RenameCategoryTypeIfExists() 
BEGIN -- Checks if CategoryType in smartstone_categories exists, before renaming, if doesn't exist it just skips this.
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_categories' AND COLUMN_NAME = 'CategoryType') THEN
        ALTER TABLE `smartstone_categories` RENAME COLUMN `CategoryType` TO `ParentCategory`;
    END IF;
END //
DELIMITER ; -- 
CALL RenameCategoryTypeIfExists();
DROP PROCEDURE RenameCategoryTypeIfExists;

ALTER TABLE `smartstone_services` ADD COLUMN IF NOT EXISTS `Category` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `ServiceId`;
ALTER TABLE `smartstone_pets` ADD COLUMN IF NOT EXISTS `Type` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `CreatureId`;
ALTER TABLE `smartstone_costumes` ADD COLUMN IF NOT EXISTS `Scale` FLOAT NOT NULL DEFAULT 1 AFTER `DisplayId`;
