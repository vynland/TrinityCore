-- This should make it so the trap despawns in DM when the quest is completed by someone.

UPDATE `smart_scripts` SET `action_type`='41' WHERE `entryorguid`='179485' and`source_type`='1' and`id`='1' and`link`='2';