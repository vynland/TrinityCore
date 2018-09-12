/*
 * Copyright (C) 2008-2018 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Chat.h"
#include "ObjectMgr.h"
#include "ReputationMgr.h"
#include "Mail.h"
#include "Group.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "QuestCompleter.h"
#include "ObjectAccessor.h"
#include "WorldSession.h"
#include "GameObject.h"

class custom_commandscript : public CommandScript
{
public:
    custom_commandscript() : CommandScript("custom_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> questcompleterCommandTable =
        {
            { "add",        rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_ADD,         true, &HandleQuestCompleterAddCommand,      "" },
            { "del",        rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_DEL,         true, &HandleQuestCompleterDelCommand,      "" },
            { "reload",     rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_RELOAD,      true, &HandleQuestCompleterReloadCommand,   "" },
            { "comment",    rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_COMMENT,     true, &HandleQuestCompleterComment,         "" },
            { "delcomment", rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_DELCOMMENT,  true, &HandleQuestDeleteComment,            "" },
            { "all",        rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER,             true, &HandleQuestCompleterCompleteAllCommand, "" },
            { "",           rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER_STATUS,      true, &HandleQuestCompleterStatusCommand,   "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "qc",         rbac::RBAC_PERM_COMMAND_CUS_QUESTCOMPLETER,             true, NULL,                                 "", questcompleterCommandTable },
        };

        return commandTable;
    }

    static bool HandleQuestCompleterStatusCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");
        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage("Please enter a quest link.");
            handler->SetSentErrorMessage(true);
            return false;
        }
        else
        {
            if (entry != 0)
            {
                std::string questTitle = quest->GetTitle();

                // Display comments for quests, whether they're completable or not.
                ReadQuestCompleterComments(handler, entry);

                if (!sQuestCompleter->IsCompletableQuest(entry))
                    handler->PSendSysMessage(LANG_COMMAND_QC_EMPTY, questTitle.c_str());
                else
                {
                    const char* playerName = handler->GetSession() ? handler->GetSession()->GetPlayer()->GetName().c_str() : NULL;
                    if (playerName)
                    {
                        std::string name = playerName;
                        normalizePlayerName(name);
                        Player* player = ObjectAccessor::FindPlayerByName(name);
                        if (player->GetQuestStatus(entry) != QUEST_STATUS_INCOMPLETE)
                            handler->PSendSysMessage(LANG_COMMAND_QC_FOUND, questTitle.c_str());
                        else
                            sQuestCompleter->CheckAndCompleteQuest(player, entry);
                    }
                    else
                        handler->PSendSysMessage(LANG_COMMAND_QC_FOUND, questTitle.c_str());
                }

                return true;
            }
            else
            {
                handler->PSendSysMessage("Please enter a quest link.");
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
    }

    static bool HandleQuestCompleterCompleteAllCommand(ChatHandler* handler, char const* args)
    {
        Player* player = nullptr;

        std::string colorBlue = handler->GetSession() ? "|cff00ccff" : "";

        if (handler->GetSession())
            player = handler->GetSession()->GetPlayer();
        else
        {
            char* param1 = strtok((char*)args, " ");

            if (!param1)
            {
                handler->SendSysMessage("Player name not provided.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            std::string name = param1;

            if (normalizePlayerName(name))
                player = ObjectAccessor::FindPlayerByName(name);
        }

        if (!player)
        {
            handler->SendSysMessage("Player not found.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        sQuestCompleter->CompleteAllBuggedQuests(player);

        return true;
    }

    static void ReadQuestCompleterComments(ChatHandler* handler, uint32 questId)
    {
        QuestCompleterCommentMapBounds bounds = sQuestCompleter->GetQuestCompleterCommentMapBounds(questId);

        if (bounds.first == bounds.second)
            return;

        std::string tag = handler->GetSession() ? "|TInterface\\ChatFrame\\UI-ChatIcon-Blizz:12:20:0:0:32:16:4:28:0:16|t" : "";
        std::string colorGreen = handler->GetSession() ? "|cff00ff00" : "";
        std::string colorBlue = handler->GetSession() ? "|cff00ccff" : "";

        for (QuestCompleterCommentContainer::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
        {
            if (sQuestCompleter->HasCommentFlag(itr->second.flags, QC_COMMENT_FLAG_VISIBLE) && !sQuestCompleter->HasCommentFlag(itr->second.flags, QC_COMMENT_FLAG_STAFFONLY))
            {
                handler->PSendSysMessage("%s #%u %s[Developer's Note]: %s%s", tag, itr->second.id, colorGreen, colorBlue, itr->second.text);
                handler->PSendSysMessage("-- %sDate: %s%s. %sAuthor: %s%s", colorGreen, colorBlue, itr->second.date, colorGreen, colorBlue, itr->second.author);
            }
        }
    }

    static bool HandleQuestCompleterAddCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");

        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_ADD);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage("Please enter a quest link.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string questTitle = quest->GetTitle();

        if (!sQuestCompleter->IsCompletableQuest(entry)) // we dont want it in the database
        {
            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_QUESTCOMPLETER);;
            stmt->setUInt32(0, entry);
            LoginDatabase.Execute(stmt);
            sQuestCompleter->AddQuest(entry);
            handler->PSendSysMessage(LANG_COMMAND_QC_ADD_SUCCESS, questTitle.c_str());
        }
        else
            handler->PSendSysMessage(LANG_COMMAND_QC_ADD_EXISTS, questTitle.c_str());

        return true;
    }

    static bool HandleQuestCompleterDelCommand(ChatHandler* handler, char const* args)
    {
        char* cId = handler->extractKeyFromLink((char*)args, "Hquest");

        if (!cId)
        {
            handler->PSendSysMessage(LANG_COMMAND_QC_DEL);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 entry = atol(cId);
        Quest const* quest = sObjectMgr->GetQuestTemplate(entry);
        if (!quest)
        {
            handler->PSendSysMessage("Please enter a quest link.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Change from string to int to prevent crash with query
        std::string questTitle = quest->GetTitle();

        if (!sQuestCompleter->IsCompletableQuest(entry))
            handler->PSendSysMessage(LANG_COMMAND_QC_DEL_ERROR, questTitle.c_str());
        else
        {
            if (entry != 0)
            {
                PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_QUESTCOMPLETER);
                stmt->setUInt32(0, entry);
                LoginDatabase.Execute(stmt);
                sQuestCompleter->RemoveQuest(entry);
                handler->PSendSysMessage(LANG_COMMAND_QC_DEL_SUCCESS, questTitle.c_str());
            }
            else
                handler->PSendSysMessage(LANG_COMMAND_QC_DEL_ERROR_2);
        }
        return true;
    }

    static bool HandleQuestCompleterReloadCommand(ChatHandler* handler, char const* /*args*/)
    {
        sQuestCompleter->LoadQuestCompleterQuests();
        handler->SendGlobalGMSysMessage("Quest completer completable quests reloaded.");
        handler->SendSysMessage("Quest completer completable quests reloaded.");

        sQuestCompleter->LoadQuestCompleterComments();
        handler->SendGlobalGMSysMessage("Quest completer comments reloaded.");
        handler->SendSysMessage("Quest completer comments reloaded");
        return true;
    }

    static bool HandleQuestDeleteComment(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        char* param1 = strtok((char*)args, " ");
        char* param2 = strtok(nullptr, " ");

        if (!param1 || !param2)
        {
            handler->PSendSysMessage(LANG_CMD_SYNTAX);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 questId = atoi(param1);
        uint32 commentId = atoi(param2);

        if (!sQuestCompleter->CommentExists(questId, commentId))
        {
            handler->SendSysMessage("The comment you're trying to delete was not found. If your table has been edited remotely please reload it before trying again.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        sQuestCompleter->DeleteComment(questId, commentId);
        handler->SendSysMessage("Comment deleted successfully.");
        return true;
    }

    static bool HandleQuestCompleterComment(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        char* entryStr = strtok((char*)args, " ");
        if (!entryStr || !atoi(entryStr))
        {
            handler->SendSysMessage("Quest not specified.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        char* flagsStr = strtok(NULL, " ");

        if (!flagsStr)
        {
            handler->SendSysMessage("Flags not specified.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint8 flags = flagsStr ? uint8(atoi(flagsStr)) : 1;

        std::string author;

        if (handler->GetSession())
            author = handler->GetSession()->GetPlayer()->GetName();
        else
        {
            char* authorStr = strtok(NULL, " ");

            if (!authorStr)
            {
                handler->SendSysMessage("Could not recognize author.");
                handler->SetSentErrorMessage(true);
                return false;
            }

            author = authorStr;
        }

        char* commentStr = strtok(NULL, "");
        if (!commentStr)
        {
            handler->SendSysMessage("Comment string not recognized.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 questId = atoi(entryStr);
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);

        if (!quest)
        {
            handler->SendSysMessage("Quest does not exist.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string comment = commentStr;

        sQuestCompleter->AddComment(questId, flags, comment, author);
        handler->SendSysMessage("Comment added successfully.");
        return true;
    }

};

void AddSC_custom_commandscript()
{
    new custom_commandscript();
}
