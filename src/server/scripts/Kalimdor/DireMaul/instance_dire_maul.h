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

    static InstanceEventCondition Default;

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
InstanceEventCondition InstanceEventCondition::Default = InstanceEventCondition();


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
    InstanceEventRegisteration(std::unique_ptr<InstanceEventCondition> condition, InstanceEventInvokable invokable)
        : Invokable(invokable),
        Condition(std::move(condition))
    {

    }

    InstanceEventCondition* GetCondition()
    {
        return Condition.get();
    }

    InstanceEventInvokable GetInvokable() const
    {
        return Invokable;
    }

private:
    std::unique_ptr<InstanceEventCondition> Condition;
    InstanceEventInvokable Invokable;
};


template<typename TEventInputType>
class InstanceEventRegister
{
public:
    //Registers an event with the given registeration data associated with the event input.
    virtual void RegisterEvent(TEventInputType invoker, std::unique_ptr<InstanceEventRegisteration> registerationData) = 0;

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
    void RegisterEvent(TKeyType invoker, std::unique_ptr<InstanceEventRegisteration> registerationData)
    {
        auto i = ListenerMap.find(invoker);
        if (i == ListenerMap.end()) //if there is none
        {
            std::vector<std::unique_ptr<InstanceEventRegisteration>> invokationList;
            invokationList.push_back(std::move(registerationData));
            ListenerMap.emplace(std::make_pair(invoker, std::move(invokationList)));
        }
        else
        {
            //We just put it in the vector of registered events.
            ListenerMap.at(invoker).push_back(std::move(registerationData));
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
            std::vector<std::unique_ptr<InstanceEventRegisteration>>& invokationList = ListenerMap.at(key);

            //TODO: Handle removing finished events
            for (auto eventData = invokationList.begin(); eventData != invokationList.end(); eventData++)
            {
                (*eventData).get()->GetCondition()->Process(potentialInvoker);

                bool ready = (*eventData).get()->GetCondition()->IsEventReady();

                sLog->outCommand(0, "GUID: %u about to Fire Event: (0 False) (1 True) %u. Name: %s", potentialInvoker->GetGUID().GetCounter(), ready, potentialInvoker->GetName());

                if (ready)
                {
                    (*eventData).get()->GetInvokable().Invoke(potentialInvoker->GetGUID());
                }
            }

            //Iterate the invokation list and remove any fired events from the list.
            /*invokationList.erase(std::remove_if(invokationList.begin(), invokationList.end(), [&](std::unique_ptr<InstanceEventRegisteration>& eventData) //careful to capture correctly, copy capture causes crashes
            {
                eventData->GetCondition()->Process(potentialInvoker);

                bool ready = eventData->GetCondition()->IsEventReady();

                sLog->outCommand(0, "GUID: %u about to Fire Event: (0 False) (1 True) %u. Name: %s", potentialInvoker->GetGUID().GetCounter(), ready, potentialInvoker->GetName());

                if (ready)
                {
                    eventData->GetInvokable().Invoke(potentialInvoker->GetGUID());

                    //Don't be clever and return the IsEventReady. We can't rely on implementers to always return what we expect
                    //as well as even if we assume it should still be true we can't expect that it is free to comput. Just return true
                    return true;
                }

                //Don't remove the event
                return false;
            }));*/

            //We should remove the vector if it is empty from the map
            if (invokationList.empty())
                ListenerMap.erase(ListenerMap.find(key));
        }
    }

private:
    std::map<TKeyType, std::vector<std::unique_ptr<InstanceEventRegisteration>>> ListenerMap;
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

//Condition that completes when a list of spawn ids events have been handled.
//DO NOT GIVE THIS A VECTOR YOU WANT TO PRESERVE
class UnitAllSpawnIdListCondition : public InstanceEventCondition
{
public:
    UnitAllSpawnIdListCondition(std::vector<int> spawnIdList)
        : SpawnIdList(std::move(spawnIdList)),
        Count(0)
    {

    }

    //Indicates if the event is ready to be dispatched.
    bool IsEventReady() const override
    {
        //Spawn id lists are only ready when the count matches the number of spawnids in the list
        return Count >= SpawnIdList.size();
    }

    void Process(Unit* invoker) override
    {
        //Spawn ID expects that the unit is a creature. So we should assert on that.
        Creature* c = invoker->ToCreature();
        ASSERT(invoker);
        ASSERT(c);

        //We make assumption that the list values are unique
        //Default is always true so do nothing.
        auto result = std::find_if(SpawnIdList.begin(), SpawnIdList.end(), [=](int id)
        {
            if (c->GetSpawnId() == id)
                return true;
            else
                return false;
        });

        if (result != SpawnIdList.end())
            Count++;

        return;
    }

private:
    std::vector<int> SpawnIdList;
    int Count;

    UnitAllSpawnIdListCondition()
        : Count(0)
    {

    }
};

//Shared tally condition works in such a way that when the tally reaches the
//WARNING: When using shared tally the event you register will be called for each condition met. Not only once. Register dummy events or keep track of this.
class SharedTallyConditionDecorator : public InstanceEventCondition
{
public:
    struct TallyGoal
    {
        int CurrentCount;
        int Goal;

        TallyGoal(int goal)
            : CurrentCount(0), Goal(goal)
        {
            //TODO: Validate goal
        }

        TallyGoal()
            : CurrentCount(0), Goal(0)
        {

        }
    };

    //Value Towards Goal represents the amount of count this tally condition is worth when the decorated condition completes.
    SharedTallyConditionDecorator(std::shared_ptr<TallyGoal> sharedTally, std::unique_ptr<InstanceEventCondition> decoratedCondition, int valueTowardsGoal)
        : SharedTally(sharedTally), DecoratedCondition(std::move(decoratedCondition)), ValueTowardsGoal(valueTowardsGoal)
    {
        //TODO: Validate tally and fired values
        //This uses a shared pointer because this value must be shared between multiple conditions.
    }

    //Indicates if the event is ready to be dispatched.
    bool IsEventReady() const override
    {
        sLog->outCommand(0, "CurrentCount: %u Goal: %u", SharedTally->CurrentCount, SharedTally->Goal);
        //This is only ready when the counter reaches the specified amount.
        return SharedTally->CurrentCount >= SharedTally->Goal;
    }

    void Process(Unit* invoker) override
    {
        ASSERT(invoker);
        //Default is always true so do nothing.
        DecoratedCondition->Process(invoker);

        if (DecoratedCondition->IsEventReady())
            SharedTally->CurrentCount += ValueTowardsGoal;
    }
private:
    std::shared_ptr<TallyGoal> SharedTally;
    std::unique_ptr<InstanceEventCondition> DecoratedCondition;
    int ValueTowardsGoal;
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
            else
                NpcDeathEventManager.ProcessEvent(u);
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

        RegisterEvent(BossDeathEventManager, entry, functionPointer);
    }

    //Registers an event to be dispatched when a Boss Creature with the provided entry dies.
    template<typename TFunctionPointerType>
    void RegisterOnNpcDeathEvent(DireMaulNpcEntry entry, TFunctionPointerType functionPointer, std::unique_ptr<InstanceEventCondition> condition)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering NPC Entry: %u with an event callback.", entry);

        RegisterEvent(NpcDeathEventManager, entry, functionPointer, std::move(condition));
    }

    //TODO: Should these methods be apart of the instancescript? Or another object?
    TGameObjectEntryType ConvertToEntry(GameObject* go)
    {
        ASSERT(go);
        return static_cast<TGameObjectEntryType>(go->GetEntry());
    }

private:
    //TODO: Bosses might have multiple GUIDs per entry too like Shkerm from AQ. Maybe?
    ObjectEntryLookupContainer<TBossEntryEnumType, ObjectGuid> BossEntryToGuidMap;
    ObjectEntryLookupContainer<TGameObjectEntryType, ObjectGuid> GameObjectEntryToGuidMap;

    //NPC entries may be duplicated throughout the instance but may have multiple instances of the NPC. The GUID identifies them, that is why we use a collection.
    ObjectEntryLookupContainer<TNpcEntryEnumType, std::vector<ObjectGuid>> NpcEntryToGuidsMap;

    //Event managers. Concept here is we have multiple managers and one for each event type.
    InstanceUnitEventListManager<TBossEntryEnumType> BossDeathEventManager;
    InstanceUnitEventListManager<TNpcEntryEnumType> NpcDeathEventManager;

    template<typename TEntryType, typename TFunctionPointerType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType>& manager, TEntryType& entry, TFunctionPointerType functionPointer, std::unique_ptr<InstanceEventCondition> condition)
    {
        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer), this);
        std::unique_ptr<InstanceEventRegisteration> registeration(new InstanceEventRegisteration(std::move(condition), invokable));
        manager.RegisterEvent(entry, std::move(registeration));
    }

    //We have a second template with no condition parameter because we can't pass Default to the other one. It does a move.
    template<typename TEntryType, typename TFunctionPointerType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType>& manager, TEntryType& entry, TFunctionPointerType functionPointer)
    {
        sLog->outCommand(0, "Registering event with a default condition.");

        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer), this);
        std::unique_ptr<InstanceEventRegisteration> registeration(new InstanceEventRegisteration(InstanceEventCondition::Default, invokable));
        //manager.RegisterEvent(entry, std::move(registeration));
    }
};

#endif
