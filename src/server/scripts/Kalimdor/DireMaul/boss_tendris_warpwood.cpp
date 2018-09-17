/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"

enum TendrisEvents
{
    Trample = 1,
    Teleport = 2,
    Uppercut = 3,
    GraspingVines = 4,
    Enrage
};

enum TendrisSpells
{
    SAY_TENDRIS_AGGRO = -1900168,

    SPELL_TRAMPLE = 5568,
    SPELL_UPPERCUT = 22916,
    SPELL_GRASPING_VINES = 22924,
    SPELL_ENTANGLE = 22994,
    SPELL_ENRAGE = 8269,

    //NPC_IRONBARK_PROTECTOR = 11459,
    //NPC_ANCIENT_EQUINE_SPIRIT = 14566,
};

// Tendris Warpwood's boss AI.
struct boss_tendris_warpwood : public BossAI
{
public:
    boss_tendris_warpwood(Creature* creature)
        : BossAI(creature, DireMaulBossIndex::Tendris_Warpwood), isEnraged(false)
    {
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        isEnraged = false;

        events.ScheduleEvent(TendrisEvents::Trample, urandc(5s, 9s));
        events.ScheduleEvent(TendrisEvents::Uppercut, urandc(2s, 4s));
        events.ScheduleEvent(TendrisEvents::GraspingVines, urandc(9s, 12s));
        events.ScheduleEvent(TendrisEvents::Teleport, 0);

        //TODO: Add aggro say, it's not in the database. DB based on packet logs according to documentation, we may have to manually add it.
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandleTendrisEvents(static_cast<TendrisEvents>(eventId));
    }

    //Used to determine if the boss should enrage
    //We track enrage state because it's cheaper than checking an aura
    void DamageTaken(Unit* /*attacker*/, uint32 &damage) override
    {
        //Check enrage first because HealthBelowPctDamaged had to cast and reinterpret UNIT_FIELDs
        //so this is much faster and called often when damage is dealt.
        if (!isEnraged && me->HealthBelowPctDamaged(30.0f, damage))
        {
            events.ScheduleEvent(TendrisEvents::Enrage, 0);
            isEnraged = true; //even though it's not actually cast yet we should mark it so only 1 event is scheduled.
        }
    }

    void HandleTendrisEvents(TendrisEvents eventId)
    {
        switch (eventId)
        {
        case TendrisEvents::Trample:
            DoCastSelf(TendrisSpells::SPELL_TRAMPLE);
            events.Repeat(urandc(9s, 14s));
            break;
        case TendrisEvents::GraspingVines:
            DoCastSelf(TendrisSpells::SPELL_GRASPING_VINES);
            events.Repeat(urandc(17s, 22s));
            break;
        case TendrisEvents::Uppercut:
            DoCastVictim(TendrisSpells::SPELL_UPPERCUT);
            events.Repeat(12s, 15s);
            break;
        case TendrisEvents::Teleport:
            if (Unit* victim = this->me->GetVictim())
            {
                if (!this->me->IsWithinMeleeRange(victim))
                {
                    DoCastVictim(TendrisSpells::SPELL_ENTANGLE);
                    events.Repeat(urandc(10s, 15s));
                }
                else
                    events.Repeat(500ms); //recheck if someone is in melee range every half second, don't go on CD for it.
            }
            else
                events.Repeat(500ms); //recheck if someone is in melee range every half second, don't go on CD for it.
            break;
        case TendrisEvents::Enrage:
            //Don't check aura or anything, just cast. It shouldn't have one or else logic is faulty elsewhere
            DoCastSelf(TendrisSpells::SPELL_ENRAGE);
            //Don't repeat enrages
            break;
        default:
            break; //TODO: Log error event
        }
    }

private:
    bool isEnraged;
};

void AddSC_boss_tendris_warpwood()
{
    RegisterCreatureAIWithFactory(boss_tendris_warpwood, GetDireMaulAI);
}
