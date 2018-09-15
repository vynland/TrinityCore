-- Quel'Serrar questline.
UPDATE `item_template` SET `startquest`='7507' WHERE  `entry`=18401;
DELETE FROM `creature_questender` WHERE  `id`=14368 AND `quest`=7507;
DELETE FROM `creature_questender` WHERE  `id`=14368 AND `quest`=7508;
DELETE FROM `creature_questender` WHERE  `id`=14368 AND `quest`=7509;
INSERT INTO `creature_questender` (`id`, `quest`) VALUES 
('14368', '7507'),
('14368', '7508'),
('14368', '7509');
DELETE FROM `creature_queststarter` WHERE  `id`=14368 AND `quest`=7508;
DELETE FROM `creature_queststarter` WHERE  `id`=14368 AND `quest`=7509;
INSERT INTO `creature_queststarter` (`id`, `quest`) VALUES 
('14368', '7508'),
('14368', '7509');

-- Add mind controll immunity to some bosses.
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 1 WHERE `entry` IN
(10811, 9319, 9024, 9041, 9056, 8983, 9543, 9537, 14327, 14349, 16097, 14326, 14321, 14323, 14325, 11501, 14324, 14322, 9219, 9218, 9217, 9596, 9736, 10899, 15796, 10509);

-- Cavindra position and movement.
UPDATE `creature` SET `position_x`='-1458.039063', `position_y`='2797.462646', `position_z`='93.77252', `orientation`='3.578274', `spawndist`='6', `MovementType`='1' WHERE  `guid`=29063;
UPDATE `creature_template` SET `MovementType`='1' WHERE  `entry`=13697;

-- Add Missing Gordok Brute
DELETE FROM `creature` WHERE  `guid`=213818 AND `id`=11441;
INSERT INTO `creature` (`guid`, `id`, `map`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`) VALUES ('213818', '11441', '429', '1', '298.245270', '-71.463470', '-3.887040', '2.732579', '86400');
