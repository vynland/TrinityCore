#ifndef DEF_IMPROVEDINSTANCE_H
#define DEF_IMPROVEDINSTANCE_H

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "Unit.h"
#include "Log.h"
#include "GameObject.h"
#include "InstanceEventListManager.h"
#include "InstanceEventConditions.h"
#include "ObjectEntryLookupContainer.h"

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
    ObjectEntryLookupContainer<TBossEntryEnumType, ObjectGuid>& GetBossEntryContainer()
    {
        return BossEntryToGuidMap;
    }

    ObjectEntryLookupContainer<TGameObjectEntryType, ObjectGuid>& GetGameObjectEntryContainer()
    {
        return GameObjectEntryToGuidMap;
    }

    ObjectEntryLookupContainer<TNpcEntryEnumType, std::vector<ObjectGuid>>& GetNpcEntryContainer()
    {
        return NpcEntryToGuidsMap;
    }

    //Registers an event to be dispatched when a Boss Creature with the provided entry dies.
    template<typename TFunctionPointerType>
    void RegisterOnBossCreatureDeathEvent(TBossEntryEnumType entry, TFunctionPointerType functionPointer, InstanceEventRegisterationType type)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering Boss Entry: %u with an event callback.", entry);

        RegisterEvent(BossDeathEventManager, entry, functionPointer, type);
    }

    //TODO: Maybe ditch function pointer template type
    //Registers an event to be dispatched when a Boss Creature with the provided entry dies.
    void RegisterOnNpcDeathEvent(TNpcEntryEnumType entry, std::function<void(ObjectGuid)> functionPointer, std::unique_ptr<InstanceEventCondition<Unit>> condition, InstanceEventRegisterationType type)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering NPC Entry: %u with an event callback.", entry);

        RegisterEvent<TNpcEntryEnumType, std::function<void(ObjectGuid)>, Unit>(NpcDeathEventManager, entry, functionPointer, std::move(condition), type);
    }

    //Registers an event to be dispatched when a Boss Creature is engaged.
    void RegisterOnBossEngagedEvent(TBossEntryEnumType entry, std::function<void(ObjectGuid)> functionPointer, std::unique_ptr<InstanceEventCondition<Unit>> condition, InstanceEventRegisterationType type)
    {
        //If the GUID is the empty guid then we don't have the mapping from entry to guid right now
        //therefore we must push this into a map and wait for the entry/guid map to be registered.
        sLog->outCommand(entry, "Registering NPC Entry: %u with an event callback.", entry);

        RegisterEvent<TBossEntryEnumType, std::function<void(ObjectGuid)>, Unit>(BossEngagedEventManager, entry, functionPointer, std::move(condition), type);
    }

    void RegisterOnGameObjectStateChangeEvent(TGameObjectEntryType entry, std::function<void(ObjectGuid)> functionPointer, std::unique_ptr<InstanceEventCondition<GameObject>> condition, InstanceEventRegisterationType type)
    {
        RegisterEvent<TGameObjectEntryType, std::function<void(ObjectGuid)>, GameObject>(GameObjectStateChangeEventManager, entry, functionPointer, std::move(condition), type);
    }

    void OnUnitEngaged(Unit* engaged, Unit* engager) override
    {
        if (Creature* c = engaged->ToCreature())
        {
            if (c->IsDungeonBoss() || c->IsDungeonBoss())
                BossEngagedEventManager.ProcessEvent(engaged);
        }
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
    InstanceUnitEventListManager<TBossEntryEnumType, Unit> BossEngagedEventManager;

    template<typename TEntryType, typename TFunctionPointerType, typename TEventSourceType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType, TEventSourceType>& manager, TEntryType& entry, TFunctionPointerType functionPointer, std::unique_ptr<InstanceEventCondition<TEventSourceType>> condition, InstanceEventRegisterationType type)
    {
        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer));
        std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registeration(new InstanceEventRegisteration<TEventSourceType>(std::move(condition), invokable, type));
        manager.RegisterEvent(entry, std::move(registeration));
    }

    //We have a second template with no condition parameter because we can't pass Default to the other one. It does a move.
    template<typename TEntryType, typename TFunctionPointerType, typename TEventSourceType>
    void RegisterEvent(InstanceUnitEventListManager<TEntryType, TEventSourceType>& manager, TEntryType& entry, TFunctionPointerType functionPointer, InstanceEventRegisterationType type)
    {
        sLog->outCommand(0, "Registering event with a default condition.");

        InstanceEventInvokable invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(functionPointer));
        std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registeration(new InstanceEventRegisteration(InstanceEventCondition::Default, invokable, type));
        manager.RegisterEvent(entry, std::move(registeration));
    }
};
#endif
