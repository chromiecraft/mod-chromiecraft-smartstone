--
-- Payload column for restore tokens: "itemEntry:count" for TOKEN_ITEM_RESTORE,
-- "spellId" for TOKEN_SPELL_RESTORE. Empty for the character-service types.
--
ALTER TABLE `smartstone_account_tokens`
    ADD COLUMN `Data` VARCHAR(32) NOT NULL DEFAULT '' AFTER `TokenType`;
