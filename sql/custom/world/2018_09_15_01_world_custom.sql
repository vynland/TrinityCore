SET @NPC := 84381;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`) VALUES
(@PATH, 1, -26.1384, 542.307, 28.6054, '0'),
(@PATH, 2, -50.0924, 542.319, 28.6056, '0'),
(@PATH, 3, -67.7613, 542.713, 28.6057, '0'),
(@PATH, 4, -77.7153, 542.718, 28.6028, '0'),
(@PATH, 5, -83.9453, 542.721, 28.6022, '0'),
(@PATH, 6, -83.5585, 535.639, 28.6047, '0'),
(@PATH, 7, -83.4339, 507.603, 28.6018, '0'),
(@PATH, 8, -83.2587, 478.408, 28.6018, '0'),
(@PATH, 9, -83.2681, 475.118, 28.6018, '0'),
(@PATH, 10, -83.2694, 479.493, 28.6018, '0'),
(@PATH, 11, -82.9326, 511.082, 28.6018, '0'),
(@PATH, 12, -82.8837, 534.924, 28.6059, '0'),
(@PATH, 13, -82.8716, 540.832, 28.603, '0'),
(@PATH, 14, -77.6795, 541.61, 28.6017, '0'),
(@PATH, 15, -45.1599, 544.161, 28.6061, '0'),
(@PATH, 16, -17.2886, 544.056, 28.6047, '0');
