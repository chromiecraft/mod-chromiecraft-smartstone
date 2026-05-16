DELETE FROM `command` WHERE `name` IN ('smartstone costume', 'smartstone costume convert');

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('smartstone costume',         0, 'Syntax: .smartstone costume [convert]\nCostume-related subcommands for the Smartstone.'),
('smartstone costume convert', 0, 'Syntax: .smartstone costume convert\nScans your inventory for legacy costume items and converts them into Smartstone costume unlocks on your account.\nRequires ModChromiecraftSmartstone.CostumeConvert.Enable = 1 in the server config.');
