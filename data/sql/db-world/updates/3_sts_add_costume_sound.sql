CREATE TABLE IF NOT EXISTS `smartstone_costume_sound` (
  `DisplayId` INT UNSIGNED NOT NULL COMMENT 'References smartstone_costumes.DisplayId',
  `Sound`     INT UNSIGNED NOT NULL COMMENT 'Sound kit ID to play when costume is applied',
  `Mode`      TINYINT      NOT NULL DEFAULT 1 COMMENT '1 = self only, 2 = surround (all nearby players)',
  PRIMARY KEY (`DisplayId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
