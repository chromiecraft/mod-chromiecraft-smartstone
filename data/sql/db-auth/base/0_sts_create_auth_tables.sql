CREATE TABLE IF NOT EXISTS `smartstone_account_settings` (
    `accountId` INT UNSIGNED NOT NULL,
    `settingId` TINYINT UNSIGNED NOT NULL,
    `data` TEXT NOT NULL,
    PRIMARY KEY (`accountId`, `settingId`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4;
