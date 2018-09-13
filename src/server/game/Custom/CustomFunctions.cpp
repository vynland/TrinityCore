#include "CustomFunctions.h"
#include "Map.h"

bool CustomFunctions::IsSpellAffectedByHealingNerf(uint32 spell) const
{
    switch (spell)
    {
        case 746:  // First Aid (Rank 1)
        case 1159: // First Aid (Rank 2)
        case 3267: // First Aid (Rank 3)
        case 3268: // First Aid (Rank 4)
        case 7926: // First Aid (Rank 5)
        case 7927: // First Aid (Rank 6)
        case 10838: // First Aid (Rank 7)
        case 10839: // First Aid (Rank 8)
        case 18608: // First Aid (Rank 9)
        case 18610: // First Aid (Rank 10)
        case 23696: // First Aid (Rank 10)
        case 27030: // First Aid (Rank 11)
        case 27031: // First Aid (Rank 12)
        case 45543: // First Aid (Rank 13)
        case 45544: // First Aid (Rank 15)
        case 51803: // First Aid (Rank 16)
        case 51827: // First Aid (Rank 14)
        case 5723: // Greater Healthstone
        case 23474:
        case 23475:
        case 5720: // Healthstone
        case 23472:
        case 23473:
        case 6263: // Lesser Healthstone
        case 23470:
        case 23471:
        case 11732: // Major Healthstone
        case 23476:
        case 23477:
        case 27235:
        case 27236:
        case 27237:
        case 47872:
        case 47873:
        case 47874:
        case 47875:
        case 47876:
        case 47877:
        case 6262: // Minor Healthstone
        case 23468:
        case 23469:
            return false;
        default:
            break;
    }

    return true;
}

enum AreaIds
{
    AREA_AZSHARA = 16,
    AREA_TAINTED_SCAR = 73,
    AREA_SERADANE = 356,
    AREA_BOUGH_SHADOW = 438,
    AREA_TWILIGHT_GROVE = 856,
    AREA_DREAM_BOUGH = 1111,
    AREA_JADEMIR_LAKE = 1112,
    AREA_ZUL_GURUB = 1977,
    AREA_ONYXIA_LAIR = 2159,
    AREA_BLACKWING_LAIR = 2677,
    AREA_MOLTEN_CORE = 2717,
    AREA_AQ = 3428,
    AREA_RUINS_OF_AQ = 3429,

    AREA_NAXXRAMAS = 3456,

    AREA_KARAZHAN = 3457
};

float CustomFunctions::GetHealingNerfPercent(Unit* target) const
{

    if (target->GetMap()->IsBattlegroundOrArena())
        return sWorld->getFloatConfig(CONFIG_FLAT_PVP_HEALING_NERF_PERCENT);

    if (target->GetMap()->IsDungeon())
        return sWorld->getFloatConfig(CONFIG_FLAT_PVE_HEALING_NERF_PERCENT);

    /*switch (target->GetAreaId())
    {
        
        case AREA_AZSHARA:
        case AREA_TAINTED_SCAR:
        case AREA_SERADANE:
        case AREA_BOUGH_SHADOW:
        case AREA_TWILIGHT_GROVE:
        case AREA_DREAM_BOUGH:
        case AREA_JADEMIR_LAKE:
        case AREA_ZUL_GURUB:
        case AREA_ONYXIA_LAIR:
        case AREA_BLACKWING_LAIR:
        case AREA_MOLTEN_CORE:
        case AREA_AQ:
        case AREA_RUINS_OF_AQ:
            return 67.0f;
        case AREA_NAXXRAMAS:
            return 50.0f;
        case AREA_KARAZHAN:
            return 25.0f;
    }*/

    return 0.0f;
}
