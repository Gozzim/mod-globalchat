DELETE FROM `acore_string` WHERE `entry` IN (17000,17001,17002,17003,17004,17005,17006);

INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(17000, 'Player |cffff0000%s|r tried to post forbidden phrase: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17001, 'Player |cffff0000%s|r tried to post URL: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17002, '%s has |cffff0000%s|r the GlobalChat.|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17003, '%s has muted |cffff0000%s|r for %s in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17004, '%s has permanently muted |cffff0000%s|r in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17005, '|cffff0000You have been muted for %s in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(17006, '|cffff0000You have been permanently muted in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);                                                                                                                                                                           ;

DELETE FROM `command` WHERE `name` IN ('global','g','chat','c','joinglobal','leaveglobal','genable','gdisable','gmute','gunmute','ginfo','galliance','ghorde','gblacklist','gblacklist add','gblacklist remove','gblacklist reload');
INSERT INTO `command` (`name`, `security`, `help`) VALUES
('global', 0, 'Syntax: .global $text\nWrite a message in the GlobalChat.\nAdditional commands: .joinglobal & .leaveglobal'),
('g', 0, 'Syntax: .g $text\nWrite a message in the GlobalChat.\nAdditional commands: .joinglobal & .leaveglobal'),
('chat', 0, 'Syntax: .chat $text\nWrite a message in the GlobalChat.\nAdditional commands: .joinglobal & .leaveglobal'),
('c', 0, 'Syntax: .c $text\nWrite a message in the GlobalChat.\nAdditional commands: .joinglobal & .leaveglobal'),
('joinglobal', 0, 'Join the GlobalChat.'),
('leaveglobal', 0, 'Leave the GlobalChat.'),
('ginfo', 1, 'Syntax: $playername\nGives GlobalChat information about a player.'),
('genable', 1, 'Enables GlobalChat.'),
('gdisable', 1, 'Disables GlobalChat.'),
('gmute', 1, 'Syntax: $playername $bantime $reason\nMutes a player in the GlobalChat\n$bantime: Negative values lead to perment mute. Otherwise use a timestring like "1d2h30s".'),
('gunmute', 1, 'Syntax: $playername\nUnmutes player in the GlobalChat.'),
('galliance', 1, 'Syntax: .galliance $text\nWrite a message in the Alliance GlobalChat.'),
('ghorde', 1, 'Syntax: .ghorde $text\nWrite a message in the Horde GlobalChat.'),
('gblacklist', 1, 'Type .gblacklist to see the list of possible subcommands or .help gblacklist $subcommand to see info on subcommands.'),
('gblacklist add', 1, 'Syntax: .gblacklist add $text\nAdds a phrase to the GlobalChat Profanity Blacklist.'),
('gblacklist remove', 1, 'Syntax: .galliance $text\nRemoves a phrase from the GlobalChat Profanity Blacklist.'),
('gblacklist reload', 1, 'Reloads the GlobalChat Profanity Blacklist.');
