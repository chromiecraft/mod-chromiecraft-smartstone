DELETE FROM `command` WHERE `name` IN (
    'smartstone',
    'smartstone unlock service',
    'smartstone reload',
    'smartstone lookup',
    'smartstone lookup pets',
    'smartstone lookup costumes',
    'smartstone lookup allpets',
    'smartstone use',
    'smartstone use costume',
    'smartstone use pet'
);

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('smartstone',                 0, 'Syntax: .smartstone\nAdds the Smartstone item to your inventory if you do not already have one.'),
('smartstone unlock service',  1, 'Syntax: .smartstone unlock service $player $serviceType $id $add\nGrants or revokes access to a smartstone service for a player.\n$serviceType: 0=Companion, 1=Combat Pet, 2=Costume, 6=Aura, 7=Vehicle.\n$id: The service ID (CreatureId for pets, costume Id for costumes).\n$add: 1 to grant, 0 to revoke.'),
('smartstone reload',          3, 'Syntax: .smartstone reload\nReloads all smartstone data from the database.'),
('smartstone lookup',          0, 'Syntax: .smartstone lookup [pets|costumes|allpets]\nLookup commands for browsing available smartstone services.'),
('smartstone lookup pets',     0, 'Syntax: .smartstone lookup pets [$filter]\nLists all companion and combat pets currently available to you.\nOptionally filters results by name. Shows the ID used with .smartstone use pet.'),
('smartstone lookup costumes', 0, 'Syntax: .smartstone lookup costumes [$filter]\nLists all costumes currently available to you.\nOptionally filters results by name. Shows the ID used with .smartstone use costume.'),
('smartstone lookup allpets',  2, 'Syntax: .smartstone lookup allpets [$filter]\nLists every pet loaded in the system regardless of unlock status.\nOptionally filters results by name. Shows the ID used with .smartstone unlock service.'),
('smartstone use',             0, 'Syntax: .smartstone use [costume|pet]\nCommands to apply a smartstone costume or summon a pet.'),
('smartstone use costume',     0, 'Syntax: .smartstone use costume $id\nApplies the costume with the given ID if you have access to it.\nUse .smartstone lookup costumes to find available IDs.'),
('smartstone use pet',         0, 'Syntax: .smartstone use pet $id\nSummons the companion or combat pet with the given ID if you have access to it.\nUse .smartstone lookup pets to find available IDs.');
