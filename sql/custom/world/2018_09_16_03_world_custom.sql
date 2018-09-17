UPDATE `creature_template` SET `speed_walk`='1.76' WHERE  `entry` IN (11444, 13036);
UPDATE `creature` SET `position_x`='434.049683', `position_y`='173.408173', `position_z`='2.852250', `orientation`='3.151170' WHERE  `guid`=145968;
SET @leaderGUID := 145835;
SET @memberGUID := 145967;
SET @memberGUID2 := 145968;
DELETE FROM `creature_formations` WHERE  `leaderGUID`=@leaderGUID;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
(@leaderGUID, @leaderGUID, '0', '0', '515'),
(@leaderGUID, @memberGUID, '4', '45', '515'),
(@leaderGUID, @memberGUID2, '4', '315', '515');
