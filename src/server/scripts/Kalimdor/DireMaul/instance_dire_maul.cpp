/*
* Copyright (C) 2018 HelloKitty/Glader/AndrewBlakely
*
* Licensed under MIT.
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Map.h"
#include "instance_dire_maul.h"
#include "Log.h"
#include "GameObject.h"

class instance_dire_maul : public InstanceMapScript
{
public:
    const static DireMaulGameObjectEntry PylonEntries[5];

    instance_dire_maul() : InstanceMapScript(DireMaulScriptName, 429) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_dire_maul_InstanceMapScript(map);
    }

    struct instance_dire_maul_InstanceMapScript : public ImprovedInstanceScript<DireMaulBossEntry, DireMaulGameObjectEntry, DireMaulNpcEntry>
    {
        instance_dire_maul_InstanceMapScript(Map* map) : ImprovedInstanceScript(map)
        {
            SetBossNumber(DIREMAUL_BOSS_COUNT);

            //RegisterOnBossCreatureDeathEvent(DireMaulBossEntry::NPC_TENDRIS, &instance_dire_maul_InstanceMapScript::OnTendrisDeath);

            //To handle the logic of a pylon we need to compose conditions based around SpawnIds and
            //register seperate linked events between the two Elemental entries

            //don
            //TODO: Use static vectors since they never change value.
            //Registers a group of NPCs associated with a pylon. The shared goal of killing all of them represented by the tally goal
            //Once the shared goal is reached the event will dispatch.
            auto pylonOneEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_1_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143510, 143524 }), std::move(std::vector<int> { 143511, 143509 }), pylonOneEvent);

            auto pylonTwoEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_2_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143512, 143514 }), std::move(std::vector<int> { 143513, 143515 }), pylonTwoEvent);

            auto pylonThreeEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_3_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143506, 143505 }), std::move(std::vector<int> { 143508, 143507 }), pylonThreeEvent);

            auto pylonFourEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_4_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143519, 143517 }), std::move(std::vector<int> { 143516, 143518 }), pylonFourEvent);

            auto pylonFiveEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_5_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143523, 143520 }), std::move(std::vector<int> { 143522, 143521 }), pylonFiveEvent);

            //Now we register the event that we want to happen once all the pylons are done.
            //THis tally goal is shared between them because we have multiple types of NPCs that contribute to the tally for this event
            std::shared_ptr<TallyGoal> deactivateAllPylonsGoal(new TallyGoal(5));
            auto unlockTheImmotharBarrierEvent = [&](ObjectGuid o) { this->UnlockedTheThing(o); };

            //TODO: We don't have to register this right away, we can actually wait until something dies first for performance reasons
            for (DireMaulGameObjectEntry pylonEntry : PylonEntries)
            {
                //TODO: Can we avoid creating N of these?
                std::unique_ptr<InstanceEventCondition<GameObject>> activeCondition(new GameObjectActiveEventCondition());
                std::unique_ptr<SharedTallyConditionDecorator<GameObject>> pylonTallyActivate(new SharedTallyConditionDecorator<GameObject>(deactivateAllPylonsGoal, std::move(activeCondition), 1));

                //Register it
                RegisterOnGameObjectStateChangeEvent(pylonEntry, unlockTheImmotharBarrierEvent, std::move(pylonTallyActivate));
            }
        }

        void UnlockedTheThing(ObjectGuid last)
        {
            sLog->outCommand(0, "Unlocked immothar.");
        }

        void CreateAndRegisterPylonEvent(int goalTotal, std::vector<int> abberationSpawnIds, std::vector<int> manaElementalSpawnIds, InstanceEventInvokable::InstanceEventInvokerFunction callback)
        {
            //THis tally goal is shared between them because we have multiple types of NPCs that contribute to the tally for this event
            std::shared_ptr<TallyGoal> goal(new TallyGoal(goalTotal));

            //TODO: Use static vectors since they never change value.
            //Registers a group of NPCs associated with a pylon. The shared goal of killing all of them represented by the tally goal
            //Once the shared goal is reached the event will dispatch.
            RegisterPylonEvent(DireMaulNpcEntry::NPC_ARCANE_ABERRATION, std::move(abberationSpawnIds), goal, callback);
            RegisterPylonEvent(DireMaulNpcEntry::NPC_RESTE_MANA, std::move(manaElementalSpawnIds), goal, callback);
        }

        void RegisterPylonEvent(DireMaulNpcEntry entry, std::vector<int> spawnIds, std::shared_ptr<TallyGoal> goal, InstanceEventInvokable::InstanceEventInvokerFunction callback)
        {
            std::unique_ptr<UnitAllSpawnIdListCondition> spawnIdCondition(new UnitAllSpawnIdListCondition(std::move(spawnIds)));
            std::unique_ptr<SharedTallyConditionDecorator<Unit>> tallyCondition(new SharedTallyConditionDecorator<Unit>(goal, std::move(spawnIdCondition), 2));
            RegisterOnNpcDeathEvent(entry, callback, std::move(tallyCondition));
        }

        void OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry pylonEntry)
        {
            GameObject* pylon1 = instance->GetGameObject(GetGameObjectEntryContainer().FindByEntry(pylonEntry));
            this->HandleGameObject(ObjectGuid::Empty, true, pylon1);
        }
    };

    void OnPlayerEnter(InstanceMap* /*map*/, Player* /*player*/) override
    {
        //Lighthope has some recommended stuff they do on enter and leave to prevent some exploits.
    }

    // Called when a player leaves the map.
    void OnPlayerLeave(InstanceMap* /*map*/, Player* /*player*/) override
    {
        //Lighthope has some recommended stuff they do on enter and leave to prevent some exploits.
    }
};


const DireMaulGameObjectEntry instance_dire_maul::PylonEntries[5]
{
    DireMaulGameObjectEntry::GO_CRISTAL_1_EVENT,
    DireMaulGameObjectEntry::GO_CRISTAL_2_EVENT,
    DireMaulGameObjectEntry::GO_CRISTAL_3_EVENT,
    DireMaulGameObjectEntry::GO_CRISTAL_4_EVENT,
    DireMaulGameObjectEntry::GO_CRISTAL_5_EVENT
};

void AddSC_instance_dire_maul()
{
    new instance_dire_maul();
}
