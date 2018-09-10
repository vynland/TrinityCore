#ifndef DEF_DIREMAUL_H
#define DEF_DIREMAUL_H

#define DireMaulScriptName "instance_dire_maul"

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "Unit.h"

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


template<typename TEventInvoker>
class InstanceEventInvokable
{
public:
    typedef void (InstanceScript::*InstanceEventInvokerFunction)(TEventInvoker);

    InstanceEventInvokable(const InstanceEventInvokerFunction invokable, InstanceScript* capturedTarget)
    {
        ASSERT(invokable != nullptr);
        ASSERT(capturedTarget != nullptr);
        Invokable = invokable;
        CapturedTarget = capturedTarget;
    }

    void Invoke(TEventInvoker invoker) const
    {
        ASSERT(invoker != nullptr);
        (CapturedTarget->*Invokable)(invoker);
    }

private:
    InstanceEventInvokerFunction Invokable;
    InstanceScript* CapturedTarget;
};

//This version exists because you may want to have a vector of GUID instead of GUID
template<typename StorageType, typename TEntryEnumType>
class ReadonlyInstanceObjectEntryLookupContainer
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    ReadonlyInstanceObjectEntryLookupContainer() { }

    //Returns the StorageType associated with an entry. It should return Empty if none are found.
    virtual StorageType FindByEntry(TEntryEnumType entry) const = 0;
};

template<typename TEntryEnumType>
class ReadonlyInstanceObjectEntryGuidLookupContainer : public ReadonlyInstanceObjectEntryLookupContainer<ObjectGuid, TEntryEnumType>
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    ReadonlyInstanceObjectEntryGuidLookupContainer() { }

    //TODO: We may want to expose a way to iterate all GUIDs.
};

template<typename StorageType, typename TEntryEnumType>
class MutableInstanceObjectEntryLookupContainer
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    MutableInstanceObjectEntryLookupContainer() { }

    virtual bool Insert(TEntryEnumType entry, StorageType storageValue) = 0;
};

template<typename TEntryEnumType>
class MutableInstanceObjectEntryGuidLookupContainer : MutableInstanceObjectEntryLookupContainer<ObjectGuid, TEntryEnumType>
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    MutableInstanceObjectEntryGuidLookupContainer() { }
};

template<typename TEntryEnumType>
class DefaultInstanceObjectEntryGuidLookupContainer : public MutableInstanceObjectEntryGuidLookupContainer<TEntryEnumType>, public ReadonlyInstanceObjectEntryGuidLookupContainer<TEntryEnumType>
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    //We could have gone the specialization route but having simplied/aliased types as interface/purvirtual seemed cleaner.
    ObjectGuid FindByEntry(TEntryEnumType entry) const
    {
        auto f = InternalMap.find(entry);
        if (f != InternalMap.end())
            return (*f).second;
        else
            return ObjectGuid::Empty;
    }

    bool Insert(TEntryEnumType entry, ObjectGuid storageValue)
    {
        //TODO: Do some checking for if it's already present.
        InternalMap.emplace(std::make_pair(entry, storageValue));
        return true;
    }

private:
    std::map<TEntryEnumType, ObjectGuid> InternalMap;
};

template<typename TEntryEnumType>
class DefaultInstanceObjectEntryGuidListLookupContainer : public MutableInstanceObjectEntryLookupContainer<std::vector<ObjectGuid>, TEntryEnumType>, public ReadonlyInstanceObjectEntryLookupContainer<std::vector<ObjectGuid>, TEntryEnumType>
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    //We could have gone the specialization route but having simplied/aliased types as interface/purvirtual seemed cleaner.
    std::vector<ObjectGuid> FindByEntry(TEntryEnumType entry) const
    {
        if (InternalMap.find(entry) != InternalMap.end())
            return InternalMap.at(entry);
        else
            return std::vector<ObjectGuid>(); //ODO: Add a Contains method so callers can check that so we don't have to return empty vectors often.
    }

    //TODO: Any way with templates to not have to duplicate this method?
    bool Insert(TEntryEnumType entry, std::vector<ObjectGuid> storageValue)
    {
        //TODO: Do some checking for if it's already present.
        InternalMap.emplace(std::make_pair(entry, storageValue));
        return true;
    }

    //This allows for the insertion of a single guid. Will create a vector if it doesn't exist.
    bool Insert(TEntryEnumType entry, ObjectGuid singleGuid)
    {
        //TODO: Do some checking for if it's already present.
        //TODO: Extract to a contains method
        if (InternalMap.find(entry) != InternalMap.end())
        {
            InternalMap.at(entry).push_back(singleGuid);
        }
        else
            return Insert(entry, std::vector<ObjectGuid> { singleGuid });

        return true;
    }

private:
    std::map<TEntryEnumType, std::vector<ObjectGuid>> InternalMap;
};

//TODO: Extract into seperate 
template<typename TBossEntryEnumType, typename TGameObjectEntryType, typename TNpcEntryEnumType>
struct ImprovedInstanceScript : public InstanceScript
{
public:
    ImprovedInstanceScript(Map* map) : InstanceScript(map)
    {
       
    }

    // Called when a creature/gameobject is added to map or removed from map.
    // Insert/Remove objectguid to dynamic guid store
    void OnCreatureCreate(Creature* creature) override
    {
        InstanceScript::OnCreatureCreate(creature);

        const int entry = creature->GetEntry();

        //TODO: Is this good enough?
        if (creature->IsDungeonBoss())
            this->BossEntryToGuidMap.Insert(static_cast<TBossEntryEnumType>(entry), creature->GetGUID());
        else
        {
            this->NpcEntryToGuidsMap.Insert(static_cast<TNpcEntryEnumType>(entry), creature->GetGUID());
        }
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        GameObjectEntryToGuidMap.Insert(ConvertToEntry(go), go->GetGUID());
    }

protected:
    const ReadonlyInstanceObjectEntryGuidLookupContainer<TBossEntryEnumType> GetBossEntryContainer() const
    {
        return BossEntryToGuidMap;
    }

    const ReadonlyInstanceObjectEntryGuidLookupContainer<TGameObjectEntryType> GetGameObjectContainer() const
    {
        return GameObjectEntryToGuidMap;
    }

    //TODO: Should these methods be apart of the instancescript? Or another object?
    TGameObjectEntryType ConvertToEntry(GameObject* go)
    {
        ASSERT(go);
        return static_cast<TGameObjectEntryType>(go->GetEntry());
    }

private:
    //TODO: Rename
    DefaultInstanceObjectEntryGuidLookupContainer<TBossEntryEnumType> BossEntryToGuidMap;
    DefaultInstanceObjectEntryGuidLookupContainer<TGameObjectEntryType> GameObjectEntryToGuidMap;

    //NPC entries may be duplicated throughout the instance but may have multiple instances of the NPC. The GUID identifies them, that is why we use a collection.
    DefaultInstanceObjectEntryGuidListLookupContainer<TNpcEntryEnumType> NpcEntryToGuidsMap;
};

#endif
