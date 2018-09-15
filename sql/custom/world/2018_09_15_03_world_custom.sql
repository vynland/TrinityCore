-- Prince Tortheldrin Poisition.
UPDATE `creature` SET `position_z`='-48.466908' WHERE  `guid`=56951;

-- Knot Thimblejack's cache shouldnt be spawned in dire maul west.
DELETE FROM `gameobject` WHERE  `guid`=35831 AND `id`=179501;
