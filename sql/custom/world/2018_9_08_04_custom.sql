-- Fix gossip menu id for Guard Fengus.
UPDATE `gossip_menu_option` SET `OptionBroadcastTextID`=9394, `OptionText`='I am, am I?  Well what have you got for the new big dog of Gordok, Fengus?', `MenuID`=5734 WHERE `MenuID`=5734 && `OptionID`=0;
UPDATE `creature_template` SET `gossip_menu_id`=5734 WHERE `entry`=14321;

-- Fix gossip menu id for Guard Slip'kik.
UPDATE `gossip_menu_option` SET `OptionBroadcastTextID`=9398, `OptionText`='Yeah, you\'re a real brainiac.  Just how smart do you think you are, Slip\'kik?', `MenuID`=5733 WHERE `MenuID`=5733 && `OptionID`=0;
UPDATE `creature_template` SET `gossip_menu_id`=5733 WHERE `entry`=14323;

-- Fix gossip menu id for Guard Mol'dar.
UPDATE `gossip_menu_option` SET `OptionBroadcastTextID`=9401, `OptionText`='Call me "Boss".  What have you got for me!', `MenuID`=5735 WHERE `MenuID`=5735 && `OptionID`=0;
UPDATE `creature_template` SET `gossip_menu_id`=5735 WHERE `entry`=14326;

-- Fix gossip menu id for Captain Kromcrush.
UPDATE `gossip_menu_option` SET `OptionBroadcastTextID`=9421, `OptionText`='Um, I\'m taking some prisoners we found outside before the king for punishment.', `MenuID`=5739, `ActionMenuID`=5738 WHERE `MenuID`=5739 && `OptionID`=0;
UPDATE `creature_template` SET `gossip_menu_id`=5739 WHERE `entry`=14325;
UPDATE `gossip_menu_option` SET `OptionBroadcastTextID`=9423, `OptionText`='Er... that\'s how I found them.  I wanted to show the king that they were a threat!  Say Captain... I overheard Guard Fengus calling you a fat, useless gnoll lover!', `MenuID`=5738 WHERE `MenuID`=5738 && `OptionID`=0;

-- Disables trap respawning was respawning after 2 minutes.
UPDATE `gameobject` SET `spawntimesecs`='21600' WHERE `guid`='159';