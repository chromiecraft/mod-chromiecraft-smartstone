DELETE FROM `command` WHERE `name` IN (
    'smartstone unlock account'
);

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('smartstone unlock account',  1, 'Syntax: .smartstone unlock account $account $serviceType $id $add\nGrants or revokes an account-wide smartstone service for an account, without needing to select a character.\n$serviceType: 2=Costume, 9=Perk.\n$id: The service ID (costume Id for costumes, perk Id for perks).\n$add: 1 to grant, 0 to revoke.\nFor per-character services (pets, auras, vehicles) use .smartstone unlock service instead.');
