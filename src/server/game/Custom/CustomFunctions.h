#include "Unit.h"

class TC_GAME_API CustomFunctions
{
public:

    static CustomFunctions* instance()
    {
        static CustomFunctions instance;
        return &instance;
    }

    bool IsSpellAffectedByHealingNerf(uint32 spell) const;
    float GetHealingNerfPercent(Unit* target) const;
};

#define sCustomFunctions CustomFunctions::instance()
