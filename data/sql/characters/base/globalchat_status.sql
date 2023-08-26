DROP TABLE IF EXISTS `player_globalchat_status`;
CREATE TABLE IF NOT EXISTS `player_globalchat_status` (
  `guid` int unsigned NOT NULL DEFAULT 0,
  `enabled` BOOL NOT NULL DEFAULT '0',
  `last_msg` int unsigned NOT NULL DEFAULT 0,
  `mute_time` int unsigned NOT NULL DEFAULT 0,
  `total_mutes` bigint unsigned NOT NULL DEFAULT 0,
  `banned` BOOL NOT NULL DEFAULT '0',
PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
