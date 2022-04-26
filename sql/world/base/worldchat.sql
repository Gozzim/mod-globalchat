DELETE FROM `acore_string` WHERE `entry` IN (17000,17001,17002,17003,17004,17005,17006);

INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(17000, 'Player |cff00ff00%s|r tried to post forbidden phrase: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17001, 'Player |cff00ff00%s|r tried to post URL: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17002, 'cffffd500%s has %s the WorldChat.|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17003, '|cffff0000%s has been muted for %s in the WorldChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17004, '|cffff0000%s has been permenently muted in the WorldChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17005, '|cffff0000You have been muted for %s in the WorldChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17006, '|cffff0000You have been permenently muted in the WorldChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);                                                                                                                                                                           ;

DELETE FROM `command` WHERE `name` IN ('world','w','chat','c','joinworld','leaveworld','enableworld','disableworld','muteworld','unmuteworld');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('world', 0, 'Syntax: .world $text\r\nWrite a message in the WorldChat.\r\nAdditional commands: .joinworld & .leaveworld'),
('w', 0, 'Syntax: .w $text\r\nWrite a message in the WorldChat.\r\nAdditional commands: .joinworld & .leaveworld'),
('chat', 0, 'Syntax: .chat $text\r\nWrite a message in the WorldChat.\r\nAdditional commands: .joinworld & .leaveworld'),
('c', 0, 'Syntax: .c $text\r\nWrite a message in the WorldChat.\r\nAdditional commands: .joinworld & .leaveworld'),
('joinworld', 0, 'Join the WorldChat.'),
('leaveworld', 0, 'Leave the WorldChat.'),
('enableworld', 1, 'Enables WorldChat.'),
('disableworld', 1, 'Disables WorldChat.'),
('muteworld', 1, 'Syntax: $playername $bantime $reason\r\nMutes a player in the WorldChat\r\n$bantime: Negative values lead to perment mute. Otherwise use a timestring like \"1d2h30s\".'),
('unmuteworld', 1, 'Syntax: $playername\r\nUnmutes player in the WorldChat.');
