DELETE FROM `pickpocketing_loot_template` WHERE  `Entry`=14326 AND `Item`=18268;
INSERT INTO `pickpocketing_loot_template` VALUES (14326, 18268, 0, 100, 0, 1, 0, 1, 1, NULL);
UPDATE `creature_template` SET `pickpocketloot`='14326' WHERE  `entry`=14326;
