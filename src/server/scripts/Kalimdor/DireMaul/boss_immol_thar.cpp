/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"

enum
{
    SPELL_TRAMPLE = 5568,
    SPELL_INFECTED_BITE = 16128,
    SPELL_EYE_OF_IMMOL_THAR = 22899,
    SPELL_PORTAL_OF_IMMOL_THAR = 22950,
    SPELL_ENRAGE = 8269,
};

enum ImmotharEvent
{
    Enrage = 0,

    Trample = 1,

    InfectedBite = 2,

    EyeOfImmothar = 3,

    PortalOfImmothar = 4
};

// boss_immol_thar
struct boss_immol_thar : public BossAI
{
public:
    boss_immol_thar(Creature* creature) : BossAI(creature, DireMaulBossIndex::Immol_thar) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        events.ScheduleEvent(ImmotharEvent::Enrage, 50s);
        events.ScheduleEvent(ImmotharEvent::Trample, urandc(5s, 9s));
        events.ScheduleEvent(ImmotharEvent::InfectedBite, urandc(2s, 4s));
        events.ScheduleEvent(ImmotharEvent::EyeOfImmothar, urandc(7s, 12s));
        events.ScheduleEvent(ImmotharEvent::PortalOfImmothar, urandc(10s, 14s));
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandleImmotharEvents(static_cast<ImmotharEvent>(eventId));
    }

    void HandleImmotharEvents(ImmotharEvent eventId)
    {
        switch (eventId)
        {
        case ImmotharEvent::Enrage:
            DoCast(SPELL_ENRAGE);
            break;
        //TODO: Nostalrius manually spawn this, did some dummy object and etc. Do we need to?
        case ImmotharEvent::EyeOfImmothar:
            DoCast(SPELL_EYE_OF_IMMOL_THAR);
            events.Repeat(urandc(15s, 22s));
            break;
        case ImmotharEvent::InfectedBite:
            DoCastVictim(SPELL_INFECTED_BITE);
            events.Repeat(urandc(8s, 12s));
            break;
        case ImmotharEvent::PortalOfImmothar:
            //TODO: Nostalrius manually moved them, do we need to?
            if (Unit* target = this->SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0))
            {
                //Resets the target's threat to 0.
                this->ModifyThreatByPercent(target, -100);
                events.Repeat(urandc(17s, 24s));
            }
            break;
        case ImmotharEvent::Trample:
            DoCast(SPELL_TRAMPLE);
            events.Repeat(urandc(9s, 14s));
            break;
        }
    }
};

void AddSC_boss_immol_thar()
{
    RegisterCreatureAIWithFactory(boss_immol_thar, GetDireMaulAI);
}
