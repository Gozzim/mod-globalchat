DELETE FROM `acore_string` WHERE `entry` IN (17000,17001,17002);

INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(17000, 'Player |cff00ff00%s|r tried to post forbidden phrase: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17001, 'Player |cff00ff00%s|r tried to post URL: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17002, 'cffffd500%s has %s the World Chat.|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

DELETE FROM `command` WHERE  `name`='world';
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('world', 0, 'Syntax: .world $text\r\nWrite a message in world chat.\r\nAdditional commands: .showworld & .hideworld'),
('w', 0, 'Syntax: .w $text\r\nWrite a message in world chat.\r\nAdditional commands: .showworld & .hideworld'),
('c', 0, 'Syntax: .c $text\r\nWrite a message in world chat.\r\nAdditional commands: .showworld & .hideworld'),
('chat', 0, 'Syntax: .chat $text\r\nWrite a message in world chat.\r\nAdditional commands: .showworld & .hideworld'),
('showworld', 0, 'Shows the World Chat.'),
('hideworld', 0, 'Hides the World Chat.'),
('enableworld', 1, 'Enables world chat.'),
('disableworld', 1, 'Disables world chat.');
