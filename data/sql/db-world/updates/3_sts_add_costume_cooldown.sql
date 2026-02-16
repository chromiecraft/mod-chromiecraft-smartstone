DELIMITER //
CREATE PROCEDURE AddCooldownToSmartstoneCostumesIfNotExists()
BEGIN -- ALTER TABLE `smartstone_costumes` ADD COLUMN `Cooldown` INT UNSIGNED NOT NULL DEFAULT 1800 AFTER `Duration`;
IF NOT EXISTS (SELECT 1 FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'smartstone_costumes' AND COLUMN_NAME = 'Cooldown') THEN
    ALTER TABLE `smartstone_costumes` ADD COLUMN `Cooldown` INT UNSIGNED NOT NULL DEFAULT 1800 AFTER `Duration`;
END IF;
END //
DELIMITER ;
CALL AddCooldownToSmartstoneCostumesIfNotExists();
DROP PROCEDURE AddCooldownToSmartstoneCostumesIfNotExists;
