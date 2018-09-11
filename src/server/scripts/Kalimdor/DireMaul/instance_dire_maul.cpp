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

            //THis tally goal is shared between them because we have multiple types of NPCs that contribute to the tally for this event
            std::shared_ptr<SharedTallyConditionDecorator::TallyGoal> goal(new SharedTallyConditionDecorator::TallyGoal(4));

            //don
            //TODO: Use static vectors since they never change value.
            //Registers a group of NPCs associated with a pylon. The shared goal of killing all of them represented by the tally goal
            //Once the shared goal is reached the event will dispatch.
            auto pylonOneEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_1_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143510, 143524 }), std::move(std::vector<int> { 143511, 143509 }), pylonOneEvent);

            auto pylonTwoEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_2_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143512, 143524 }), std::move(std::vector<int> { 143513, 143515 }), pylonTwoEvent);

            auto pylonThreeEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_3_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143506, 143505 }), std::move(std::vector<int> { 143508, 143507 }), pylonThreeEvent);

            auto pylonFourEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_4_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143519, 143517 }), std::move(std::vector<int> { 143516, 143518 }), pylonFourEvent);

            auto pylonFiveEvent = [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_5_EVENT); };
            CreateAndRegisterPylonEvent(4, std::move(std::vector<int> { 143523, 143520 }), std::move(std::vector<int> { 143522, 143521 }), pylonFiveEvent);
        }

        void CreateAndRegisterPylonEvent(int goalTotal, std::vector<int> abberationSpawnIds, std::vector<int> manaElementalSpawnIds, InstanceEventInvokable::InstanceEventInvokerFunction callback)
        {
            //THis tally goal is shared between them because we have multiple types of NPCs that contribute to the tally for this event
            std::shared_ptr<SharedTallyConditionDecorator::TallyGoal> goal(new SharedTallyConditionDecorator::TallyGoal(goalTotal));

            //TODO: Use static vectors since they never change value.
            //Registers a group of NPCs associated with a pylon. The shared goal of killing all of them represented by the tally goal
            //Once the shared goal is reached the event will dispatch.
            RegisterPylonEvent(DireMaulNpcEntry::NPC_ARCANE_ABERRATION, std::move(abberationSpawnIds), goal, callback);
            RegisterPylonEvent(DireMaulNpcEntry::NPC_RESTE_MANA, std::move(manaElementalSpawnIds), goal, callback);
        }

        void RegisterPylonEvent(DireMaulNpcEntry entry, std::vector<int> spawnIds, std::shared_ptr<SharedTallyConditionDecorator::TallyGoal> goal, InstanceEventInvokable::InstanceEventInvokerFunction callback)
        {
            std::unique_ptr<UnitAllSpawnIdListCondition> spawnIdCondition(new UnitAllSpawnIdListCondition(std::move(spawnIds)));
            std::unique_ptr<SharedTallyConditionDecorator> tallyCondition(new SharedTallyConditionDecorator(goal, std::move(spawnIdCondition), 2));
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

void AddSC_instance_dire_maul()
{
    new instance_dire_maul();
}
