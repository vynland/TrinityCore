UPDATE `creature` SET `position_x`='390.602356', `position_y`='216.257278', `position_z`='11.203223', `orientation`='2.688154' WHERE  `guid`=145985;
UPDATE `creature` SET `position_x`='389.432831', `position_y`='209.770981', `position_z`='11.208812', `orientation`='2.664593' WHERE  `guid`=145986;
SET @leaderGUID := 145911;
SET @memberGUID := 145985;
SET @memberGUID2 := 145986;
DELETE FROM `creature_formations` WHERE  `leaderGUID`=@leaderGUID;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
(@leaderGUID, @leaderGUID, '0', '0', '515'),
(@leaderGUID, @memberGUID, '4', '45', '515'),
(@leaderGUID, @memberGUID2, '4', '315', '515');
