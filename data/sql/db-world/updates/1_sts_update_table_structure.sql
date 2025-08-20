-- Use the example how to prevent errors in the cases that commands DO NOT support "IF NOT EXISTS" by usage of procedures.

DELIMITER // 
CREATE PROCEDURE RenameCategoryTypeIfExists() 
BEGIN -- ALTER TABLE `smartstone_categories` RENAME COLUMN `CategoryType` TO `ParentCategory`;
IF EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_categories' AND COLUMN_NAME = 'CategoryType') THEN
    ALTER TABLE `smartstone_categories` RENAME COLUMN `CategoryType` TO `ParentCategory`;
END IF;
END //
DELIMITER ; -- 
CALL RenameCategoryTypeIfExists();
DROP PROCEDURE RenameCategoryTypeIfExists;

DELIMITER //
CREATE PROCEDURE AddCategoryToSmartstoneServicesIfNotExists()
BEGIN -- ALTER TABLE `smartstone_services` ADD COLUMN `Category` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `ServiceId`;
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_services' AND COLUMN_NAME = 'Category') THEN
    ALTER TABLE `smartstone_services` ADD COLUMN `Category` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `ServiceId`;
END IF;
END //
DELIMITER ;
CALL AddCategoryToSmartstoneServicesIfNotExists();
DROP PROCEDURE AddCategoryToSmartstoneServicesIfNotExists;

DELIMITER // 
CREATE PROCEDURE AddTypeToSmartstonePetsIfNotExists()
BEGIN -- ALTER TABLE `smartstone_pets` ADD COLUMN `Type` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `CreatureId`;
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_pets' AND COLUMN_NAME = 'Type') THEN
    ALTER TABLE `smartstone_pets` ADD COLUMN `Type` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `CreatureId`;
END IF;
END //
DELIMITER ;
CALL AddTypeToSmartstonePetsIfNotExists();
DROP PROCEDURE AddTypeToSmartstonePetsIfNotExists;

DELIMITER // -- ALTER TABLE `smartstone_costumes` ADD COLUMN `Scale` FLOAT NOT NULL DEFAULT 1 AFTER `DisplayId`;
CREATE PROCEDURE AddScaleToSmartstoneCostumesIfNotExists()
BEGIN
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_costumes' AND COLUMN_NAME = 'Scale') THEN
    ALTER TABLE `smartstone_costumes` ADD COLUMN `Scale` FLOAT NOT NULL DEFAULT 1 AFTER `DisplayId`;
END IF;
END //
DELIMITER ;
CALL AddScaleToSmartstoneCostumesIfNotExists();
DROP PROCEDURE AddScaleToSmartstoneCostumesIfNotExists;
