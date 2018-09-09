-- Makes Illyanna Ravenoak cast vanilla ability Aimed Shot

DELETE FROM `smart_scripts` WHERE `entryorguid`='11488' and`source_type`='0' and`id`='4' and`link`='0';


INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES ('11488', '0', '4', '0', '0', '0', '100', '2', '6000', '8000', '10000', '16000', '0', '11', '20904', '64', '0', '0', '0', '0', '5', '0', '0', '0', '0', '0', '0', '0', 'Illyanna Ravenoak - In Combat - Cast \'Aimed Shot\' (Normal Dungeon)');
