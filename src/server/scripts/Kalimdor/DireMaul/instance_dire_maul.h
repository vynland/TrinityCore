#ifndef DEF_DIREMAUL_H
#define DEF_DIREMAUL_H

#define DireMaulScriptName "instance_dire_maul"

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "Unit.h"
#include "Log.h"
#include "GameObject.h"

enum DireMaulBossIndex
{
    Pusillin = 0,
    Lethtendris = 1,
    Hydrospawn = 2,
    Zevrim_Thornhoof = 3,
    Alzzin_the_Wildshaper = 4,

    Tendris_Warpwood = 5,
    Magister_Kalendris = 6,
    Tsu_zee = 7,
    Illyanna_Ravenoak = 8,
    Immol_thar = 9,
    Prince_Tortheldrin = 10,

    Guard_Mol_dar = 11,
    Stomper_Kreeg = 12,
    Guard_Fengus = 13,
    Guard_Slip_kik = 14,
    Captain_Kromcrush = 15,
    King_Gordok = 16,
};

enum DireMaulGameObjectEntry
{
    // DM East
    GO_CRUMBLE_WALL = 177220,
    GO_FELVINE_SHARD = 179559,
    GO_CORRUPT_VINE = 179502,
    GO_DOOR_ALZZIN_IN = 181496,

    //DM West
    GO_FORCE_FIELD = 179503,
    GO_MAGIC_VORTEX = 179506,
    GO_CRISTAL_1_EVENT = 177259,
    GO_CRISTAL_2_EVENT = 177257,
    GO_CRISTAL_3_EVENT = 177258,
    GO_CRISTAL_4_EVENT = 179504,
    GO_CRISTAL_5_EVENT = 179505,

    //DM North
    GO_BROKEN_TRAP = 179485,
    GO_FIXED_TRAP = 179512,

    GO_GORDOK_TRIBUTE_0 = 179564,
    GO_GORDOK_TRIBUTE_1 = 300400,
    GO_GORDOK_TRIBUTE_2 = 300401,
    GO_GORDOK_TRIBUTE_3 = 300402,
    GO_GORDOK_TRIBUTE_4 = 300403,
    GO_GORDOK_TRIBUTE_5 = 300404,
    GO_GORDOK_TRIBUTE_6 = 300405,
};

enum DireMaulNpcEntry
{
    NPC_RESTE_MANA = 11483,
    NPC_ARCANE_ABERRATION = 11480,
    NPC_TENDRIS_PROTECTOR = 11459,
    NPC_MIZZLE_THE_CRAFTY = 14353,
};

enum DireMaulBossEntry
{
    // DM East
    NPC_OLD_IRONBARK = 11491,
    NPC_ZEVRIM = 11490,
    NPC_ALZZIN = 11492,

    // DM West
    NPC_IMMOL_THAR_GARDIEN = 11466,
    NPC_IMMOL_THAR = 11496,
    NPC_TORTHELDRIN = 11486,
   

    NPC_TENDRIS = 11489,
    //SAY_FREE_IMMOLTHAR = -1900008,

    // DM North
    NPC_GUARD_MOLDAR = 14326,
    NPC_GUARD_FENGUS = 14321,
    NPC_GUARD_SLIPKIK = 14323,
    NPC_CAPTAIN_KROMCRUSH = 14325,
    NPC_CHORUSH = 14324,
    NPC_KING_GORDOK = 11501,
};

int const DIREMAUL_BOSS_COUNT = 17;

//TODO: Unify some of thise stuff in a shared instance header for use throughout the project.
//Based on VMangos
inline void EnableUnit(Unit* const unit)
{
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    unit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
}

//Based on VMangos
inline void DisableUnit(Unit* const unit)
{
    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
}

#endif
