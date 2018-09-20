/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

enum ZevrimEvent
{
    CastPainEvent = 1,
    SacrificeEvent = 2
};

enum ZevrimSpell
{
    SPELL_INTENSIVE_PAIN = 22478,
    SPELL_SACRIFICE = 22651,
};

struct boss_zevrim : public BossAI
{
public:
    boss_zevrim(Creature* creature)
        : BossAI(creature, DMDataTypes::Zevrim_Thornhoof)
    {

    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);

        /*m_uiIntensePainTimer    = urand(5000, 9000);
        m_uiSacrificeTimer      = urand(9000, 12000);*/
        events.ScheduleEvent(ZevrimEvent::CastPainEvent, urandc(5s, 9s));
        events.ScheduleEvent(ZevrimEvent::SacrificeEvent, urandc(9s, 12s));
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandleZevrimEvents(static_cast<ZevrimEvent>(eventId));
    }

    void HandleZevrimEvents(ZevrimEvent eventId)
    {
        switch (eventId)
        {
        case ZevrimEvent::CastPainEvent:
            DoCast(ZevrimSpell::SPELL_INTENSIVE_PAIN);
            events.Repeat(urandc(20s, 26s));
            break;
        case ZevrimEvent::SacrificeEvent:
            if (Unit* sacTarget = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 37.0f, true))
            {
                DoCast(sacTarget, ZevrimSpell::SPELL_SACRIFICE);
                events.Repeat(urandc(15s, 18s));
            }
            else
                events.Repeat(300ms); //if we found no target somehow then we should try repeating in 300ms.
            break;
        default:
            break;
        }
    }
};

void AddSC_boss_zevrim()
{
    RegisterCreatureAIWithFactory(boss_zevrim, GetDireMaulAI);
}
