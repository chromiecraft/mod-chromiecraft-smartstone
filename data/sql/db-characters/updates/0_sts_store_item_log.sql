--
-- One row per item unit delivered by GM/store mail (a mailed stack of 3
-- inserts 3 rows) or restored via a challenge-restore token claim. The
-- challenge-reclaim scan converts at most this many units of a
-- RequireStoreOrigin whitelist entry on death.
--
CREATE TABLE IF NOT EXISTS `smartstone_store_item_log` (
    `Id`           INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `Guid`         INT UNSIGNED NOT NULL,               -- character low GUID
    `ItemEntry`    INT UNSIGNED NOT NULL,
    `ReceivedTime` INT UNSIGNED NOT NULL,
    `Source`       TINYINT UNSIGNED NOT NULL DEFAULT 0, -- 0 = GM/store mail, 1 = restore token claim
    PRIMARY KEY (`Id`),
    KEY `idx_guid_entry` (`Guid`, `ItemEntry`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4;
