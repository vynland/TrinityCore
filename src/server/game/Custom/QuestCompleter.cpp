#include "QuestCompleter.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ReputationMgr.h"

/* ####################################################################
#
# QUEST COMPLETER - EXTENDED
# - Useful knowledge library where players help each other by leaving comments on quests 
# and sharing information on how to complete them.
# - Additionally, broken quests can be completed automatically for players when checking for comments.
#
# Author: Nyeriah (NoxGaming)
#
####################################################################### */

QuestCompleter::QuestCompleter() { }

QuestCompleter::~QuestCompleter()
{
    _questCompleterStore.clear();
}

bool QuestCompleter::CommentExists(uint32 questId, uint32 commentId) const
{
    QuestCompleterCommentMapBounds bounds = GetQuestCompleterCommentMapBounds(questId);

    if (bounds.first == bounds.second)
        return false;

    for (QuestCompleterCommentContainer::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second.id == commentId)
            return true;
    }

    return false;
}

uint32 QuestCompleter::GetCommentCountForQuest(uint32 questId)
{
    QuestCompleterCommentMapBounds bounds = GetQuestCompleterCommentMapBounds(questId);

    if (bounds.first == bounds.second)
        return 0;

    uint32 count = 0;

    for (QuestCompleterCommentContainer::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
        count++;

    return count;
}

uint32 QuestCompleter::GetNextCommentId(uint32 questId) const
{
    QuestCompleterCommentMapBounds bounds = GetQuestCompleterCommentMapBounds(questId);

    if (bounds.first == bounds.second)
        return 0;

    uint32 count = 0;

    for (QuestCompleterCommentContainer::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
        count++;

    return count++;
}

void QuestCompleter::LoadQuestCompleterQuests()
{
    uint32 oldMSTime = getMSTime();
    _questCompleterStore.clear();

    QueryResult result = LoginDatabase.Query("SELECT id FROM quest_completer ORDER BY id ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 quest completer entries. DB table `quest_completer` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 quest_ID = fields[0].GetUInt32();

        Quest const* quest = sObjectMgr->GetQuestTemplate(quest_ID);

        if (!quest)
        {
            TC_LOG_ERROR("sql.sql", "Table `quest_completer` has record for not existing quest %u", quest_ID);
            continue;
        }

        _questCompleterStore.insert(quest_ID);

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u quest_completer completable quests in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void QuestCompleter::LoadQuestCompleterComments()
{
    uint32 oldMSTime = getMSTime();
    _questCompleterCommentStore.clear();

    //                                                 0       1     2             3              4     5 
    QueryResult result = LoginDatabase.Query("SELECT questId, id, comment, FROM_UNIXTIME(date), flags, author FROM quest_completer_comments ORDER BY questId ASC");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 quest completer comments. DB table `quest_completer_comments` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        uint32 quest_ID = fields[0].GetUInt32();
        uint32 id       = fields[1].GetUInt32();
        uint8 flags     = fields[4].GetUInt8();

        Quest const* quest = sObjectMgr->GetQuestTemplate(quest_ID);

        if (!quest)
        {
            TC_LOG_ERROR("sql.sql", "Table `quest_completer_comments` has record for not existing quest %u", quest_ID);
            continue;
        }

        if (flags > QC_COMMENT_MAX_FLAGS)
        {
            TC_LOG_ERROR("sql.sql", "Bad flag value detected for `quest_completer_comments` entry (quest: %u, comment: %u). Loading as 0.", quest_ID, id);
            id = 0;
        }

        QuestCompleterComment comment;

        comment.questId = quest_ID;
        comment.id      = id;
        comment.text    = fields[2].GetCString();
        comment.date    = fields[3].GetCString();
        comment.flags   = flags;
        comment.author  = fields[5].GetCString();

        _questCompleterCommentStore.insert(QuestCompleterCommentContainer::value_type(comment.questId, comment));
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u quest_completer_comments in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void QuestCompleter::AddComment(uint32 questId, uint8 flags, std::string comment, std::string author)
{
    uint32 id = GetNextCommentId(questId);
    time_t timeNow = time(NULL);

    comment.erase(std::remove(comment.begin(), comment.end(), '"'), comment.end());

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_QUESTCOMPLETER_COMMENT);
    stmt->setUInt32(0, questId);
    stmt->setUInt32(1, id);
    stmt->setString(2, comment);
    stmt->setUInt8(3, flags);
    stmt->setUInt32(4, timeNow);
    stmt->setString(5, author);
    LoginDatabase.Execute(stmt);

    QuestCompleterComment commentItem;
    
    commentItem.questId = questId;
    commentItem.id = id;
    commentItem.text = comment;
    commentItem.flags = flags;
    commentItem.date = "<Unavailble, commented recently>";
    commentItem.author = author;

    _questCompleterCommentStore.insert(QuestCompleterCommentContainer::value_type(commentItem.questId, commentItem));
}

void QuestCompleter::DeleteComment(uint32 questId, uint32 commentId)
{
    QuestCompleterCommentMapBounds bounds = GetQuestCompleterCommentMapBounds(questId);

    if (bounds.first == bounds.second)
        return;

    for (QuestCompleterCommentContainer::const_iterator itr = bounds.first; itr != bounds.second;)
    {
        if (itr->second.id == commentId)
            itr = _questCompleterCommentStore.erase(itr);
        else
            ++itr;
    }

    LoginDatabase.PExecute("DELETE FROM quest_completer_comments WHERE questId = %u AND id = %u", questId, commentId);
}

void QuestCompleter::CompleteAllBuggedQuests(Player* player)
{
    QuestStatusMap questMap = player->getQuestStatusMap();

    uint8 counter = 0;
    bool foundComment = false;

    ChatHandler handler(player->GetSession());

    std::string colorBlue = handler.GetSession() ? "|cff00ccff" : "";

    for (QuestStatusMap::iterator itr = questMap.begin(); itr != questMap.end(); ++itr)
    {
        if (player->GetQuestStatus(itr->first) != QUEST_STATUS_INCOMPLETE)
            continue;

        if (sQuestCompleter->IsCompletableQuest(itr->first))
        {
            CheckAndCompleteQuest(player, itr->first);
            ++counter;
        }
        else
        {
            uint32 commentCount = sQuestCompleter->GetCommentCountForQuest(itr->first);

            if (commentCount && player->GetSession())
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(itr->first);

                if (!quest)
                    continue;

                foundComment = true;
                handler.PSendSysMessage("%s%u comments found for quest %s.", colorBlue, commentCount, quest->GetTitle());
            }
        }
    }

    if (foundComment && handler.GetSession())
        handler.SendSysMessage("Type '.qc <quest link>' to read comments.");

    if (!counter)
    {
        handler.SendSysMessage("No bugged were found in your quest log.");
        handler.SetSentErrorMessage(true);
    }
    else
        handler.PSendSysMessage("%u quest(s) completed. If you did not wish them to be complete, please abandon them and take them again.", counter);
}

void QuestCompleter::CheckAndCompleteQuest(Player* player, uint32 entry)
{
    // actual code for completing
    Quest const* quest = sObjectMgr->GetQuestTemplate(entry);

    if (!player)
        return;

    ChatHandler handler(player->GetSession());

    if (!quest)
    {
        handler.PSendSysMessage("Invalid quest request. The quest does not exist.");
        handler.SetSentErrorMessage(true);
        return;
    }

    //If player doesnt have the quest
    if (player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
    {
        handler.PSendSysMessage("Quest not in your quest log.");
        handler.SetSentErrorMessage(true);
        return;
    }

    // Add quest items for quests that require items
    for (uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x)
    {
        uint32 id = quest->RequiredItemId[x];
        uint32 count = quest->RequiredItemCount[x];
        if (!id || !count)
            continue;

        uint32 curItemCount = player->GetItemCount(id, true);

        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
        if (msg == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, id, true);
            player->SendNewItem(item, count - curItemCount, true, false);
        }
    }

    // All creature/GO slain/cast (not required, but otherwise it will display "Creature slain 0/10")
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        int32 creature = quest->RequiredNpcOrGo[i];
        uint32 creatureCount = quest->RequiredNpcOrGoCount[i];

        if (creature > 0)
        {
            if (CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate(creature))
                for (uint16 z = 0; z < creatureCount; ++z)
                    player->KilledMonster(creatureInfo, ObjectGuid::Empty);
        }
        else if (creature < 0)
            for (uint16 z = 0; z < creatureCount; ++z)
                player->KillCreditGO(creature, ObjectGuid::Empty);
    }
    // If the quest requires reputation to complete
    if (uint32 repFaction = quest->GetRepObjectiveFaction())
    {
        uint32 repValue = quest->GetRepObjectiveValue();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue)
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
                player->GetReputationMgr().SetReputation(factionEntry, repValue);
    }

    // If the quest requires a SECOND reputation to complete
    if (uint32 repFaction = quest->GetRepObjectiveFaction2())
    {
        uint32 repValue2 = quest->GetRepObjectiveValue2();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue2)
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
                player->GetReputationMgr().SetReputation(factionEntry, repValue2);
    }

    // If the quest requires money
    int32 ReqOrRewMoney = quest->GetRewOrReqMoney();
    if (ReqOrRewMoney < 0)
        player->ModifyMoney(-ReqOrRewMoney);

    handler.PSendSysMessage(LANG_COMMAND_QC_COMPLETE, quest->GetTitle().c_str());
    player->CompleteQuest(entry);
}
