SET @ENTRY := 17000;

DELETE FROM `acore_string` WHERE `entry` IN (@ENTRY+0, @ENTRY+1, @ENTRY+2, @ENTRY+3, @ENTRY+4, @ENTRY+5,@ENTRY+6);
INSERT INTO `acore_string` (`entry`, `content_default`, `locale_koKR`, `locale_frFR`, `locale_deDE`, `locale_zhCN`, `locale_zhTW`, `locale_esES`, `locale_esMX`, `locale_ruRU`) VALUES
(@ENTRY+0, 'Player |cffff0000%s|r tried to post forbidden phrase: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+1, 'Player |cffff0000%s|r tried to post URL: |cffff0000%s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+2, '%s has |cffff0000%s|r the GlobalChat.|r', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+3, '%s has muted |cffff0000%s|r for %s in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+4, '%s has permanently muted |cffff0000%s|r in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+5, '|cffff0000You have been muted for %s in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(@ENTRY+6, '|cffff0000You have been permanently muted in the GlobalChat.|r Reason: %s', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);                                                                                                                                                                           ;
