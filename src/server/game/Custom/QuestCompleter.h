#include "DatabaseEnv.h"
#include "World.h"
#include "QuestDef.h"

struct QuestCompleterComment
{
    uint32 questId;
    uint32 id;
    std::string text;
    std::string date;
    uint8 flags;
    std::string author;
};

enum QCCommentFlags
{
    QC_COMMENT_FLAG_NONE       = 0,
    QC_COMMENT_FLAG_VISIBLE    = 1, // This comment is visible, it'll be displayed to players once they query it.
    QC_COMMENT_FLAG_STAFF      = 2, // NYI - Commented by Staff
    QC_COMMENT_FLAG_STAFFONLY  = 4, // Only staff members can see this comment.

    QC_COMMENT_MAX_FLAGS       = 7
};

// Quest Completer
typedef std::set<uint32> QuestCompleterContainer;

// Quest Completer extended
typedef std::multimap<uint32, QuestCompleterComment> QuestCompleterCommentContainer;
typedef std::pair<QuestCompleterCommentContainer::const_iterator, QuestCompleterCommentContainer::const_iterator> QuestCompleterCommentMapBounds;

class TC_GAME_API QuestCompleter
{
    private:
        QuestCompleter();
        ~QuestCompleter();

    public:
        static QuestCompleter* instance()
        {
            static QuestCompleter instance;
            return &instance;
        }

        void LoadQuestCompleterQuests();
        void LoadQuestCompleterComments();

        QuestCompleterCommentMapBounds GetQuestCompleterCommentMapBounds(uint32 questId) const { return _questCompleterCommentStore.equal_range(questId); }

        uint32 GetCommentCountForQuest(uint32 questId);
        uint32 GetNextCommentId(uint32 questId) const;

        bool CommentExists(uint32 questId, uint32 commentId) const;

        QuestCompleterContainer _questCompleterStore;
        QuestCompleterCommentContainer _questCompleterCommentStore;

        bool IsCompletableQuest(uint32 questId) const { return _questCompleterStore.find(questId) != _questCompleterStore.end(); }

        void AddQuest(uint32 questId) { _questCompleterStore.insert(questId); }
        void RemoveQuest(uint32 questId) { _questCompleterStore.erase(questId); }

        bool HasCommentFlag(uint8 c, uint8 f) const { return (c & f) != 0; }
        void AddComment(uint32 questId, uint8 flags, std::string comment, std::string author);
        void DeleteComment(uint32 questId, uint32 commentId);

        void CompleteAllBuggedQuests(Player* player);
        void CheckAndCompleteQuest(Player* player, uint32 entry);
};

#define sQuestCompleter QuestCompleter::instance()
