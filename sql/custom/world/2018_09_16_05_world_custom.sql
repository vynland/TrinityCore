SET @NPC := 145892;
SET @PATH := @NPC * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@NPC;
INSERT INTO `creature_addon` (`guid`,`path_id`,`mount`,`bytes1`,`bytes2`,`emote`,`auras`) VALUES (@NPC,@PATH,0,0,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`, `delay`) VALUES
(@PATH, 1, 310.687988, -12.719400, -3.801290, '15000'),
(@PATH, 2, 307.775177, -101.390770, -3.885917, '0'),
(@PATH, 3, 307.010010, 54.026604, -3.968496, '0'),
(@PATH, 4, 305.667969, -12.722033, -3.884593, '0');
