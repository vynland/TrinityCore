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

//The default condition is always ready for invokation.
template<typename TEventSourceType>
class InstanceEventCondition
{
public:
    InstanceEventCondition() { }

    static InstanceEventCondition<TEventSourceType> Default;// = InstanceEventCondition<TEventSourceType>();

    //Indicates if the event is ready to be dispatched.
    virtual bool IsEventReady() const
    {
        return true;
    }

    virtual void Process(TEventSourceType* invoker)
    {
        ASSERT(invoker);
        //Default is always true so do nothing.
        return;
    }
};


//TODO: We want to support many types of patterns seen in instances.
class InstanceEventInvokable
{
public:
    typedef std::function<void(ObjectGuid&)> InstanceEventInvokerFunction;

    //The target callback for the event, should accept a guid value, and the target instance.
    InstanceEventInvokable(const InstanceEventInvokerFunction invokable)
        : Invokable(invokable)
    {
        ASSERT(invokable != nullptr);
    }

    //The invoker should be that guid of the object that cause the event to happen.
    void Invoke(ObjectGuid invoker) const
    {
        ASSERT(!invoker.IsEmpty());
        (Invokable)(invoker);
    }

private:
    InstanceEventInvokerFunction Invokable;
};

template<typename TEventSourceType>
class InstanceEventRegisteration
{
public:
    InstanceEventRegisteration(std::unique_ptr<InstanceEventCondition<TEventSourceType>> condition, InstanceEventInvokable invokable)
        : Condition(std::move(condition)), Invokable(invokable)
    {

    }

    InstanceEventCondition<TEventSourceType>* GetCondition()
    {
        return Condition.get();
    }

    InstanceEventInvokable GetInvokable() const
    {
        return Invokable;
    }

private:
    std::unique_ptr<InstanceEventCondition<TEventSourceType>> Condition;
    InstanceEventInvokable Invokable;
};


template<typename TKeyType, typename TEventSourceType>
class InstanceEventRegister
{
public:
    //Registers an event with the given registeration data associated with the event input.
    virtual void RegisterEvent(TKeyType invoker, std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registerationData) = 0;

    //TODO: Support a shared registeration between multiple invokes (like a group of NPCs that all must die before an event fires)
};

template<typename TEventSourceType>
class InstanceEventProcessor
{
public:
    //Should be called to process a potential event.
    //May not actually raise an event if none are registered/listening for an event involving this potential invoker.
    virtual void ProcessEvent(TEventSourceType* potentialInvoker) = 0;
};

//Strategy for producing the key used for an event.
template<typename TKeyType, typename TEventSourceType>
class InstanceEventKeyUnitStrategy
{
    //TODO: Figure out why this static assert doesn't work.
    static_assert(std::is_enum<TKeyType>::value, "Only supported template types are enums and ObjectGuid.");
public:
    TKeyType GetKey(TEventSourceType* u)
    {
        //Assume anything else is the entry
        return static_cast<TKeyType>(u->GetEntry());
    }
};

template<>
class InstanceEventKeyUnitStrategy<ObjectGuid, Unit>
{
public:
    ObjectGuid GetKey(Unit* u)
    {
        return u->GetGUID();
    }
};

template<>
class InstanceEventKeyUnitStrategy<ObjectGuid, GameObject>
{
public:
    ObjectGuid GetKey(Unit* u)
    {
        return u->GetGUID();
    }
};

template<typename TKeyType, typename TEventSourceType>
class InstanceUnitEventListManager : public InstanceEventProcessor<TEventSourceType>, public InstanceEventRegister<TKeyType, TEventSourceType>
{
public:
    void RegisterEvent(TKeyType invoker, std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registerationData) override
    {
        auto i = ListenerMap.find(invoker);
        if (i == ListenerMap.end()) //if there is none
        {
            std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>> invokationList;
            invokationList.push_back(std::move(registerationData));
            ListenerMap.emplace(std::make_pair(invoker, std::move(invokationList)));
        }
        else
        {
            //We just put it in the vector of registered events.
            ListenerMap.at(invoker).push_back(std::move(registerationData));
        }
    }

    void ProcessEvent(TEventSourceType* potentialInvoker)
    {
        ASSERT(potentialInvoker);

        //We invoke by entry for this event manager so we need to get the entry
        TKeyType key = KeyParser.GetKey(potentialInvoker);

        //TODO: Look into using find instead of at everywhere to make this run faster, make sure it doesn't copy.
        if (ListenerMap.find(key) != ListenerMap.end())
        {
            std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>>& invokationList = ListenerMap.at(key);

            //TODO: Handle removing finished events
            for (auto eventData = invokationList.begin(); eventData != invokationList.end(); eventData++)
            {
                //The logic here is that if an event is already finished before we begin this round of processing them it
                //means that the event is completed and we should not consider it further
                //this convention exists mostly to help support linked event registerations
                if ((*eventData).get()->GetCondition()->IsEventReady())
                    continue;

                (*eventData).get()->GetCondition()->Process(potentialInvoker);

                bool ready = (*eventData).get()->GetCondition()->IsEventReady();

                sLog->outCommand(0, "GUID: %u about to Fire Event: (0 False) (1 True) %u. Name: %s", potentialInvoker->GetGUID().GetCounter(), ready, potentialInvoker->GetName());

                if (ready)
                {
                    (*eventData).get()->GetInvokable().Invoke(potentialInvoker->GetGUID());
                }
            }

            //TODO: Might be able to unify this with the above iteration
            //TODO: Is this the best way to handle it? Iterating a second time?
            invokationList.erase(std::remove_if(invokationList.begin(), invokationList.end(), [](const auto& e)
            {
                //TODO: remove if it is expired.
                //The logic here is that if the event is ready, then it should have been fired. If it's been fired when we are not interested in the event anymore
                //and therefore it should be removed.
                return e.get()->GetCondition()->IsEventReady();
            }), invokationList.end());

            //We should remove the vector if it is empty from the map
            if (invokationList.empty())
                ListenerMap.erase(ListenerMap.find(key));
        }
    }

private:
    std::map<TKeyType, std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>>> ListenerMap;
    InstanceEventKeyUnitStrategy<TKeyType, TEventSourceType> KeyParser;
};

template<typename TEntryEnumType, typename TStorageType>
class ObjectEntryLookupContainer
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    ObjectEntryLookupContainer() { }

    bool ContainsKey(TEntryEnumType entry)
    {
        return InternalMap.find(entry) != InternalMap.end();
    }

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
class UnitAllSpawnIdListCondition : public InstanceEventCondition<Unit>
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
        return Count >= static_cast<int>(SpawnIdList.size());
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
            //TODO: Switch to uint. May be some that exceed int? I don't know, but we should switch.
            if (static_cast<int>(c->GetSpawnId()) == id)
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

//TODO: Move this
class GameObjectActiveEventCondition : public InstanceEventCondition<GameObject>
{
public:
    GameObjectActiveEventCondition()
        : isActive(false)
    {

    }

    virtual bool IsEventReady() const override
    {
        return isActive;
    }

    virtual void Process(GameObject* invoker) override
    {
        ASSERT(invoker);
        ///We just check if it's active now.
        isActive = invoker->GetGoState() == GOState::GO_STATE_ACTIVE;
        return;
    }
private:
    bool isActive;
};

//TODO: Rename
//We brought this out of the class because it turned into a template.
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

//Shared tally condition works in such a way that when the tally reaches the
//WARNING: When using shared tally the event you register will be called for each condition met. Not only once. Register dummy events or keep track of this.
template<typename TEventSourceType>
class SharedTallyConditionDecorator : public InstanceEventCondition<TEventSourceType>
{
public:
    //Value Towards Goal represents the amount of count this tally condition is worth when the decorated condition completes.
    SharedTallyConditionDecorator(std::shared_ptr<TallyGoal> sharedTally, std::unique_ptr<InstanceEventCondition<TEventSourceType>> decoratedCondition, int valueTowardsGoal)
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

    void Process(TEventSourceType* invoker) override
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
            TNpcEntryEnumType castedEntry = static_cast<TNpcEntryEnumType>(entry);

            //If it already contains an entry we must add it to the vector
            if (NpcEntryToGuidsMap.ContainsKey(castedEntry))
            {
                std::vector<ObjectGuid>& v = NpcEntryToGuidsMap.FindByEntry(castedEntry);
                v.push_back(creature->GetGUID());
            }
            else
                this->NpcEntryToGuidsMap.Insert(castedEntry, std::move(std::vector<ObjectGuid> { creature->GetGUID() }));
        }
    }

    void OnGameObjectStateChange(GameObject* go, GOState state) override
    {
        sLog->outCommand(state, "GO: %s State: %u", go->GetName(), state);
        GameObjectStateChangeEventManager.ProcessEvent(go);
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
    void RegisterOnBossCreatureDeathEvent(TBossEntryEnumType entry, TFunctionPointerType functionPointer)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering Boss Entry: %u with an event callback.", entry);

        RegisterEvent(BossDeathEventManager, entry, functionPointer);
    }

    //TODO: Maybe ditch function pointer template type
    //Registers an event to be dispatched when a Boss Creature with the provided entry dies.
    void RegisterOnNpcDeathEvent(TNpcEntryEnumType entry, std::function<void(ObjectGuid)> functionPointer, std::unique_ptr<InstanceEventCondition<Unit>> condition)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering NPC Entry: %u with an event callback.", entry);

        RegisterEvent<TNpcEntryEnumType, std::function<void(ObjectGuid)>, Unit>(NpcDeathEventManager, entry, functionPointer, std::move(condition));
    }

    void RegisterOnGameObjectStateChangeEvent(TGameObjectEntryType entry, std::function<void(ObjectGuid)> functionPointer, std::unique_ptr<InstanceEventCondition<GameObject>> condition)
    {
        RegisterEvent<TGameObjectEntryType, std::function<void(ObjectGuid)>, GameObject>(GameObjectStateChangeEventManager, entry, functionPointer, std::move(condition));
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
    InstanceUnitEventListManager<TBossEntryEnumType, Unit> BossDeathEventManager;
    InstanceUnitEventListManager<TNpcEntryEnumType, Unit> NpcDeathEventManager;
    InstanceUnitEventListManager<TGameObjectEntryType, GameObject> GameObjectStateChangeEventManager;

    template<typename TEntryType, typename TFunctionPointerType, typename TEventSourceType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType, TEventSourceType>& manager, TEntryType& entry, TFunctionPointerType functionPointer, std::unique_ptr<InstanceEventCondition<TEventSourceType>> condition)
    {
        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer));
        std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registeration(new InstanceEventRegisteration<TEventSourceType>(std::move(condition), invokable));
        manager.RegisterEvent(entry, std::move(registeration));
    }

    //We have a second template with no condition parameter because we can't pass Default to the other one. It does a move.
    template<typename TEntryType, typename TFunctionPointerType, typename TEventSourceType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType, TEventSourceType>& manager, TEntryType& entry, TFunctionPointerType functionPointer)
    {
        sLog->outCommand(0, "Registering event with a default condition.");

        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer));
        std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registeration(new InstanceEventRegisteration(InstanceEventCondition::Default, invokable));
        manager.RegisterEvent(entry, std::move(registeration));
    }
};

#endif
