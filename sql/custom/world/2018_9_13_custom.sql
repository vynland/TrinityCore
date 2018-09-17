-- Changing Immothar from smartAI to BossAI
UPDATE `creature_template` SET `ScriptName`='boss_immol_thar' WHERE `entry`='11496';


-- Changing Prince from smartAI to BossAI
UPDATE `creature_template` SET `AIName`='', `ScriptName`='boss_prince_tortheldrin' WHERE `entry`='11486';


-- Changing Tendris from smartAI to BossAI
UPDATE `creature_template` SET `AIName`='', `ScriptName`='boss_tendris_warpwood' WHERE `entry`='11489';
