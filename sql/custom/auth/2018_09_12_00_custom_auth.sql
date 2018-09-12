DROP TABLE IF EXISTS `quest_completer`;
CREATE TABLE `quest_completer` (
  `id` int(5) unsigned NOT NULL,
  PRIMARY KEY (`id`)
);

DROP TABLE IF EXISTS `quest_completer_comments`;
CREATE TABLE `quest_completer_comments` (
  `questId` int(10) unsigned  DEFAULT NULL,
  `id` int(10) unsigned  DEFAULT NULL,
  `comment` text,
  `date`  int(10) unsigned DEFAULT NULL,
  `flags` tinyint(4) DEFAULT '0',
  `author` varchar(20) DEFAULT NULL
);

DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 1000 AND 1006;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(1000, 'Command: .qc'),
(1001, 'Command: .qc status'),
(1002, 'Command: .qc add'),
(1003, 'Command: .qc del'),
(1004, 'Command: .qc reload'),
(1005, 'Command: .qc comment'),
(1006, 'Command: .qc delcomment');
