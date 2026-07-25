--
-- Whitelist of items/spells converted into account-scoped restore tokens
-- when a challenge character (marker auras 2000100-2000102) below level 70
-- is deleted. Type: 0 = item entry, 1 = spell id.
--
CREATE TABLE IF NOT EXISTS `smartstone_challenge_reclaimable` (
    `Type`               TINYINT UNSIGNED NOT NULL,
    `Entry`              INT UNSIGNED NOT NULL,
    -- Items only: 1 = convert at most as many units as smartstone_store_item_log
    -- records for the dying character (store purchases + token restores);
    -- 0 = convert everything owned regardless of origin.
    `RequireStoreOrigin` TINYINT UNSIGNED NOT NULL DEFAULT 0,
    `Comment`            VARCHAR(255) NOT NULL DEFAULT '',
    PRIMARY KEY (`Type`, `Entry`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4;
