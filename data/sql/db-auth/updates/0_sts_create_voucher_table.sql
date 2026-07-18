--
-- Account-scoped, one-shot character-service vouchers (rename / faction /
-- race / customize). Lives in the auth DB so a voucher granted to an account
-- is claimable from any character on any realm. One row = one voucher;
-- ConsumedByGUID = 0 means unclaimed.
--
CREATE TABLE IF NOT EXISTS `smartstone_account_vouchers` (
    `Id`             INT UNSIGNED NOT NULL AUTO_INCREMENT,
    `AccountId`      INT UNSIGNED NOT NULL,
    `VoucherType`    TINYINT UNSIGNED NOT NULL,        -- SmartstoneVoucherType
    `GrantedBy`      INT UNSIGNED NOT NULL DEFAULT 0,  -- granting account id (0 = console)
    `GrantedTime`    INT UNSIGNED NOT NULL DEFAULT 0,
    `ConsumedByGUID` INT UNSIGNED NOT NULL DEFAULT 0,  -- character low GUID that consumed it (0 = unconsumed)
    `ConsumedTime`   INT UNSIGNED NOT NULL DEFAULT 0,
    PRIMARY KEY (`Id`),
    KEY `idx_account_unconsumed` (`AccountId`, `ConsumedByGUID`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4;
