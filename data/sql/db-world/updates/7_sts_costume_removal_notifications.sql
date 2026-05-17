DELETE FROM `module_string`        WHERE `module` = 'mod-cc-smartstone' AND `id` IN (60, 61);
DELETE FROM `module_string_locale` WHERE `module` = 'mod-cc-smartstone' AND `id` IN (60, 61);

INSERT INTO `module_string` (`module`, `id`, `string`) VALUES
('mod-cc-smartstone', 60, 'Your ''{}'' costume has expired.'),
('mod-cc-smartstone', 61, 'Your costume has been removed. Costumes are not allowed in battlegrounds or arenas.');

INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `string`) VALUES
-- deDE
('mod-cc-smartstone', 60, 'deDE', 'Dein ''{}'' Kostüm ist abgelaufen.'),
('mod-cc-smartstone', 61, 'deDE', 'Dein Kostüm wurde entfernt. Kostüme sind in Schlachtfeldern oder Arenen nicht erlaubt.'),
-- frFR
('mod-cc-smartstone', 60, 'frFR', 'Votre costume ''{}'' a expiré.'),
('mod-cc-smartstone', 61, 'frFR', 'Votre costume a été retiré. Les costumes ne sont pas autorisés dans les champs de bataille ou les arènes.'),
-- esES
('mod-cc-smartstone', 60, 'esES', 'Tu disfraz ''{}'' ha expirado.'),
('mod-cc-smartstone', 61, 'esES', 'Tu disfraz ha sido eliminado. Los disfraces no están permitidos en campos de batalla o arenas.'),
-- esMX
('mod-cc-smartstone', 60, 'esMX', 'Tu disfraz ''{}'' ha expirado.'),
('mod-cc-smartstone', 61, 'esMX', 'Tu disfraz ha sido eliminado. Los disfraces no están permitidos en campos de batalla o arenas.'),
-- ruRU
('mod-cc-smartstone', 60, 'ruRU', 'Срок действия вашего костюма ''{}'' истёк.'),
('mod-cc-smartstone', 61, 'ruRU', 'Ваш костюм был снят. Костюмы не разрешены на полях сражений или в аренах.'),
-- koKR
('mod-cc-smartstone', 60, 'koKR', '''{}'' 코스튬이 만료되었습니다.'),
('mod-cc-smartstone', 61, 'koKR', '코스튬이 제거되었습니다. 전장이나 투기장에서는 코스튬을 사용할 수 없습니다.'),
-- zhCN
('mod-cc-smartstone', 60, 'zhCN', '您的''{}''服装已过期。'),
('mod-cc-smartstone', 61, 'zhCN', '您的服装已被移除。服装不允许在战场或竞技场中使用。'),
-- zhTW
('mod-cc-smartstone', 60, 'zhTW', '您的''{}''服裝已過期。'),
('mod-cc-smartstone', 61, 'zhTW', '您的服裝已被移除。服裝不允許在戰場或競技場中使用。');
