DELETE FROM `trinity_string` WHERE `entry` BETWEEN 15000 AND 15010;
INSERT INTO `trinity_string` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`) VALUES
('15000','Syntax: .qc $quest

Searches Quest Completer to see if $quest is bugged.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15001','%s completed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15002','%s is bugged!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15003','%s is not bugged!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15004','Syntax: .qc add $quest\n\nAdds $quest to the quest completer.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15005','%s was added!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15006','%s is already in Quest Completer!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15007','Syntax: .qc del $quest\n\nDeletes $quest from the quest completer.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15008','%s was removed!',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15009','%s is not in the Quest Completer.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL),
('15010','There was a error with your request.',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `command` WHERE `permission` BETWEEN 1000 AND 1003;
INSERT INTO `command` (`name`, `permission`, `help`) VALUES
('qc reload','1001','Reloads the quest completer completable quest list. Not required unless it has been edited through database.'),
('qc comment','1002','Syntax: $quest $flags "$comment".\rPosts a comment on the given quest. The comment is displayed whenever the quest completer command is queried.\rComment must be written between quotes.'),
('qc delcomment','1003','Syntax: $quest $comment.\rDeletes a given comment.'),
('qc all','1000','Completes every bugged quest for the given character.'),
('qc', '1000', '');
