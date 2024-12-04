DROP TABLE IF EXISTS `smartstone_char_temp_services`;
CREATE TABLE `smartstone_char_temp_services` (
 `PlayerGUID` INT UNSIGNED NOT NULL,
 `ServiceId` INT UNSIGNED NOT NULL,
 `Category` INT UNSIGNED NOT NULL,
 `ActivationTime` INT UNSIGNED NOT NULL,
 `ExpirationTime` INT UNSIGNED NOT NULL
 );
