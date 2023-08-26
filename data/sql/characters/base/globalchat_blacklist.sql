DROP TABLE IF EXISTS `globalchat_blacklist`;
CREATE TABLE IF NOT EXISTS `globalchat_blacklist` (
`phrase` VARCHAR(255) NOT NULL,
PRIMARY KEY (`phrase`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
