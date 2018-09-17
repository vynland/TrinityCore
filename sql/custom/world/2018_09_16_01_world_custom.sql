UPDATE `creature` SET `position_x`='469.107391', `position_y`='26.523718' WHERE  `guid`=145994;
UPDATE `creature_template` SET `speed_walk`='1.76' WHERE  `entry`=13036;
DELETE FROM `creature_formations` WHERE  `leaderGUID`=145891;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
('145891', '145891', '0', '0', '515'),
('145891', '145993', '4', '315', '515'),
('145891', '145994', '4', '45', '515');
