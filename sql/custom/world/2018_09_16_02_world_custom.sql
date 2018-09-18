UPDATE `creature` SET `position_x`='647.592407', `position_y`='502.581085', `position_z`='29.461740' WHERE  `guid`=145983;
UPDATE `creature` SET `position_x`='647.849304', `position_y`='505.958984', `position_z`='29.462654' WHERE  `guid`=145984;
UPDATE `creature` SET `position_x`='642.460876', `position_y`='502.060364', `position_z`='29.468754' WHERE  `guid`=145982;
SET @NPC := 145983;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`) VALUES
(@PATH, 1, 647.592407, 502.581085, 29.461740, '10000'),
(@PATH, 2, 684.822571, 493.836090, 29.462868, '0'),
(@PATH, 3, 709.803406, 513.367371, 28.177431, '10000'),
(@PATH, 4, 762.990723, 483.263916, 28.400089, '0'),
(@PATH, 5, 817.365784, 449.537964, 37.318211, '0'),
(@PATH, 6, 845.305847, 449.317657, 36.874657, '0'),
(@PATH, 7, 862.452393, 472.706818, 36.398125, '0'),
(@PATH, 8, 840.275940, 515.460876, 37.113319, '0'),
(@PATH, 9, 794.094910, 498.613739, 37.318233, '0'),
(@PATH, 10, 756.126526, 475.120880, 28.184032, '0'),
(@PATH, 11, 757.959534, 442.870148, 28.184032, '0'),
(@PATH, 12, 733.127747, 441.818878, 28.181429, '10000'),
(@PATH, 13, 715.233643, 449.170471, 28.179024, '0'),
(@PATH, 14, 693.168091, 435.485046, 28.173841, '10000'),
(@PATH, 15, 695.037781, 471.995789, 28.175241, '0');
DELETE FROM `creature_formations` WHERE  `leaderGUID`=145983;
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`) VALUES 
('145983', '145983', '0', '0', '515'),
('145983', '145984', '4', '315', '515'),
('145983', '145982', '4', '45', '515');
