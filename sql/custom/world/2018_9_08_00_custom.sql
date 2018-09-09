-- Adding Bruising Blow to Guard Fengus
DELETE FROM `smart_scripts` WHERE `entryorguid`=14321 AND `id`='10' AND `source_type`='0';

INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES ('14321', '0', '10', '0', '0', '0', '100', '2', '7000', '9000', '10000', '14000', '0', '11', '22572', '1', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'Guard Fengus - In Combat - Cast Bruising Blow');

-- Adding Cleave to Guard Fengus
DELETE FROM `smart_scripts` WHERE `entryorguid`=14321 AND `id`='11' AND `source_type`='0';

INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES ('14321', '0', '11', '0', '0', '0', '100', '2', '3000', '5000', '7000', '9000', '0', '11', '20691', '1', '0', '0', '0', '0', '2', '0', '0', '0', '0', '0', '0', '0', 'Guard Fengus - In Combat - Cast Cleave');

