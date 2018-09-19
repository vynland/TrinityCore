-- Guards, Stomper, Captain
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` |1|2|16|32|64|512|1024|4096|8192|65536|8388608 WHERE `entry` IN (14326, 14322, 14321, 14323, 14325, 11501);

-- Chom'rush
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` |1|2|4|8|32|64|128|256|512|1024|2048|131072 WHERE `entry` = 14324;
