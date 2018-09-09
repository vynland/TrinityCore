-- Adding Cleave to Slip'kik
DELETE FROM `smart_scripts` WHERE `entryorguid`=14323 AND `id`='15' AND `source_type`='0';

INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES ('14323', '0', '15', '0', '0', '0', '100', '2', '3000', '5000', '7000', '9000', '0', '11', '20691', '1', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'Slipkik - In Combat - Cast Cleave');

-- Adding Cleave to Guard Mol'dar
DELETE FROM `smart_scripts` WHERE `entryorguid`=14326 AND `id`='8' AND `source_type`='0';

INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES ('14326', '0', '8', '0', '0', '0', '100', '2', '3000', '5000', '7000', '9000', '0', '11', '20691', '1', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'Guard Moldar - In Combat - Cast Cleave');


