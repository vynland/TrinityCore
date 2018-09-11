#ifndef DEF_DIREMAUL_H
#define DEF_DIREMAUL_H

#define DireMaulScriptName "instance_dire_maul"

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "Unit.h"
#include <boost/stacktrace.hpp>
#include "Log.h"
#include <iostream>

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

//The default condition is always ready for invokation.
class InstanceEventCondition
{
public:
    InstanceEventCondition() { }

    static InstanceEventCondition const Default;

    //Indicates if the event is ready to be dispatched.
    virtual bool IsEventReady() const
    {
        return true;
    }

    virtual void Process(Unit* invoker)
    {
        ASSERT(invoker);
        //Default is always true so do nothing.
        return;
    }
};

//TODO: Should this be here?
InstanceEventCondition const InstanceEventCondition::Default = InstanceEventCondition();


//TODO: We want to support many types of patterns seen in instances.
class InstanceEventInvokable
{
public:
    typedef void (InstanceScript::*InstanceEventInvokerFunction)(ObjectGuid);

    //The target callback for the event, should accept a guid value, and the target instance.
    InstanceEventInvokable(const InstanceEventInvokerFunction invokable, InstanceScript* capturedTarget)
        : Invokable(invokable),
        CapturedTarget(capturedTarget)
    {
        ASSERT(invokable != nullptr);
        ASSERT(capturedTarget != nullptr);
    }

    //The invoker should be that guid of the object that cause the event to happen.
    void Invoke(ObjectGuid invoker) const
    {
        ASSERT(!invoker.IsEmpty());
        (CapturedTarget->*Invokable)(invoker);
    }

private:
    InstanceEventInvokerFunction Invokable;
    InstanceScript* CapturedTarget;
};

class InstanceEventRegisteration
{
public:
    InstanceEventRegisteration(InstanceEventCondition condition, InstanceEventInvokable invokable)
        : Invokable(invokable),
        Condition(condition)
    {

    }

    InstanceEventCondition GetCondition() const
    {
        return Condition;
    }

    InstanceEventInvokable GetInvokable() const
    {
        return Invokable;
    }

private:
    InstanceEventCondition Condition;
    InstanceEventInvokable Invokable;
};


template<typename TEventInputType>
class InstanceEventRegister
{
public:
    //Registers an event with the given registeration data associated with the event input.
    virtual void RegisterEvent(TEventInputType invoker, InstanceEventRegisteration registerationData) = 0;

    //TODO: Support a shared registeration between multiple invokes (like a group of NPCs that all must die before an event fires)
};

template<typename TEventInputType>
class InstanceEventProcessor
{
public:
    //Should be called to process a potential event.
    //May not actually raise an event if none are registered/listening for an event involving this potential invoker.
    virtual void ProcessEvent(TEventInputType potentialInvoker) = 0;
};

//Strategy for producing the key used for an event.
template<typename TKeyType>
class InstanceEventKeyUnitStrategy
{
    //TODO: Figure out why this static assert doesn't work.
    static_assert(std::is_enum<TKeyType>::value, "Only supported template types are enums and ObjectGuid.");
public:
    TKeyType GetKey(Unit* u)
    {
        //Assume anything else is the entry
        return static_cast<TKeyType>(u->GetEntry());
    }
};

template<>
class InstanceEventKeyUnitStrategy<ObjectGuid>
{
public:
    ObjectGuid GetKey(Unit* u)
    {
        return u->GetGUID();
    }
};

template<typename TKeyType>
class InstanceUnitEventListManager : public InstanceEventProcessor<Unit*>, public InstanceEventRegister<TKeyType>
{
public:
    void RegisterEvent(TKeyType invoker, InstanceEventRegisteration registerationData)
    {
        auto i = ListenerMap.find(invoker);
        if (i == ListenerMap.end()) //if there is none
        {
            ListenerMap.emplace(std::make_pair(invoker, std::vector<InstanceEventRegisteration> { registerationData }));
        }
        else
        {
            //We just put it in the vector of registered events.
            ListenerMap.at(invoker).push_back(registerationData);
        }
    }

    void ProcessEvent(Unit* potentialInvoker)
    {
        ASSERT(potentialInvoker);

        //We invoke by entry for this event manager so we need to get the entry
        TKeyType key = KeyParser.GetKey(potentialInvoker);

        //TODO: Look into using find instead of at everywhere to make this run faster, make sure it doesn't copy.
        if (ListenerMap.find(key) != ListenerMap.end())
        {
            std::vector<InstanceEventRegisteration>& invokationList = ListenerMap.at(key);

            //Iterate the invokation list and remove any fired events from the list.
            invokationList.erase(std::remove_if(invokationList.begin(), invokationList.end(), [=](InstanceEventRegisteration& eventData)
            {
                eventData.GetCondition().Process(potentialInvoker);

                if (eventData.GetCondition().IsEventReady())
                {
                    eventData.GetInvokable().Invoke(potentialInvoker->GetGUID());

                    //Don't be clever and return the IsEventReady. We can't rely on implementers to always return what we expect
                    //as well as even if we assume it should still be true we can't expect that it is free to comput. Just return true
                    return true;
                }

                //Don't remove the event
                return false;
            }));

            //We should remove the vector if it is empty from the map
            if (invokationList.empty())
                ListenerMap.erase(ListenerMap.find(key));
        }
    }

private:
    std::map<TKeyType, std::vector<InstanceEventRegisteration>> ListenerMap;
    InstanceEventKeyUnitStrategy<TKeyType> KeyParser;
};

template<typename TEntryEnumType, typename TStorageType>
class ObjectEntryLookupContainer
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    ObjectEntryLookupContainer() { }

    bool Insert(TEntryEnumType entry, TStorageType storageValue)
    {
        //TODO: Do some checking for if it's already present.
        InternalMap.emplace(std::make_pair(entry, storageValue));
        return true;
    }
    
    TStorageType FindByEntry(TEntryEnumType entry) const
    {
        auto f = InternalMap.find(entry);
        if (f != InternalMap.end())
            return (*f).second;

        //TODO: Handle ObjectGuid case better somehow
        return TStorageType();
    }
private:
    std::map<TEntryEnumType, TStorageType> InternalMap;
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
        //std::cout << boost::stacktrace::stacktrace();

        InstanceScript::OnCreatureCreate(creature);

        const int entry = creature->GetEntry();

        //TODO: Is this good enough?
        if (creature->IsDungeonBoss())
            this->BossEntryToGuidMap.Insert(static_cast<TBossEntryEnumType>(entry), creature->GetGUID());
        else
        {
            this->NpcEntryToGuidsMap.Insert(static_cast<TNpcEntryEnumType>(entry), std::vector<ObjectGuid> { creature->GetGUID() });
        }
    }

    void OnUnitDeath(Unit* u) override
    {
        //When a unit dies we must alert the related event managers
        //They may or may not dispatch an event related to it but it's constant time to check if one can be dispatched.
        if (u->GetTypeId() == TYPEID_UNIT)
        {
            //It's a creature 100%
            Creature* c = u->ToCreature();

            //If it's a boss then we should dispatch it to the boss event manager
            //so that any listeners listening for the death of this boss can be notified.
            if (c->IsDungeonBoss() || c->isWorldBoss())
                BossDeathEventManager.ProcessEvent(u);
        }
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        GameObjectEntryToGuidMap.Insert(ConvertToEntry(go), go->GetGUID());
    }

protected:
    const ObjectEntryLookupContainer<TBossEntryEnumType, ObjectGuid> GetBossEntryContainer() const
    {
        return BossEntryToGuidMap;
    }

    const ObjectEntryLookupContainer<TGameObjectEntryType, ObjectGuid> GetGameObjectEntryContainer() const
    {
        return GameObjectEntryToGuidMap;
    }

    const ObjectEntryLookupContainer<TNpcEntryEnumType, std::vector<ObjectGuid>> GetNpcEntryContainer() const
    {
        return NpcEntryToGuidsMap;
    }

    //Registers an event to be dispatched when a Boss Creature with the provided entry dies.
    template<typename TFunctionPointerType>
    void RegisterOnBossCreatureDeathEvent(DireMaulBossEntry entry, TFunctionPointerType functionPointer)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering Boss Entry: %u with an event callback.", entry);

        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer), this);
        InstanceEventRegisteration registeration = InstanceEventRegisteration(InstanceEventCondition::Default, invokable);
        BossDeathEventManager.RegisterEvent(entry, registeration);
    }

    //TODO: Should these methods be apart of the instancescript? Or another object?
    TGameObjectEntryType ConvertToEntry(GameObject* go)
    {
        ASSERT(go);
        return static_cast<TGameObjectEntryType>(go->GetEntry());
    }

private:
    //TODO: Rename
    ObjectEntryLookupContainer<TBossEntryEnumType, ObjectGuid> BossEntryToGuidMap;
    ObjectEntryLookupContainer<TGameObjectEntryType, ObjectGuid> GameObjectEntryToGuidMap;

    //NPC entries may be duplicated throughout the instance but may have multiple instances of the NPC. The GUID identifies them, that is why we use a collection.
    ObjectEntryLookupContainer<TNpcEntryEnumType, std::vector<ObjectGuid>> NpcEntryToGuidsMap;

    //Event managers. Concept here is we have multiple managers and one for each event type.
    InstanceUnitEventListManager<TBossEntryEnumType> BossDeathEventManager;
};

#endif
