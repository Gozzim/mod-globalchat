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
('gblacklist remove', 1, 'Syntax: .gblacklist remove $text\nRemoves a phrase from the GlobalChat Profanity Blacklist.'),
('gblacklist reload', 1, 'Reloads the GlobalChat Profanity Blacklist.');
