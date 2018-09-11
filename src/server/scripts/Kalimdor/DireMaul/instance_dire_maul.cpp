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

            //TODO: Use static vectors since they never change value.
            //Registers a group of NPCs associated with a pylon. The shared goal of killing all of them represented by the tally goal
            //Once the shared goal is reached the event will dispatch.
            RegisterPylonEvent(DireMaulNpcEntry::NPC_ARCANE_ABERRATION, std::move(std::vector<int> { 143510, 143524 }), goal);
            RegisterPylonEvent(DireMaulNpcEntry::NPC_RESTE_MANA, std::move(std::vector<int> { 143511, 143509  }), goal);
        }

        void RegisterPylonEvent(DireMaulNpcEntry entry, std::vector<int> spawnIds, std::shared_ptr<SharedTallyConditionDecorator::TallyGoal> goal)
        {
            std::unique_ptr<UnitAllSpawnIdListCondition> spawnIdCondition(new UnitAllSpawnIdListCondition(std::move(spawnIds)));
            std::unique_ptr<SharedTallyConditionDecorator> tallyCondition(new SharedTallyConditionDecorator(goal, std::move(spawnIdCondition), 2));
            RegisterOnNpcDeathEvent(entry, &instance_dire_maul_InstanceMapScript::OnPylonOneGuardainsDead, std::move(tallyCondition));
        }

        void OnPylonOneGuardainsDead(ObjectGuid last)
        {
            this->DoCastSpellOnPlayers(5);
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
