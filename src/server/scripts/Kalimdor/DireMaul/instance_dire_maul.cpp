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

            //InstanceEventInvokable<Unit*> invokable(static_cast<InstanceEventInvokable::InstanceEventInvokerFunction>(&instance_dire_maul_InstanceMapScript::Test), this);
            this->RegisterOnBossCreatureDeathEvent(DireMaulBossEntry::NPC_TENDRIS, &instance_dire_maul_InstanceMapScript::OnTendrisDeath);
        }

        void OnTendrisDeath(ObjectGuid guid)
        {
            //Guid should be tendris'
            //Kill the player for test purposes.
            this->DoCastSpellOnPlayers(5); //death touch.
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
