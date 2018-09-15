-- Ferra
DELETE FROM `creature` WHERE `guid`=333285 AND `id`=14308;
INSERT INTO `creature` (`guid`, `id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `curhealth`) VALUES ('333285', '14308', '429', '-11.6649', '544.295', '28.6046', '3.1557', '100000', '15717');
UPDATE `creature_template` SET `speed_walk`='1' WHERE  `entry`=14308;
DELETE FROM `creature_formations` WHERE  `leaderGUID`=84381;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
('84381', '333285', '2', '270', '515'),
('84381', '84381', '0', '0', '515');
