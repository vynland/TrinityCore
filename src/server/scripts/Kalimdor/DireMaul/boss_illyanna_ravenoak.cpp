/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"

enum IllyannaSpells
{
    SPELL_VOLLEY = 22908,
    SPELL_ARCANEBLAST = 22940,
    SPELL_CONCUSSIVESHOT = 22914,
    SPELL_IMMOLATIONTRAP = 22910,
    SPELL_AIMEDSHOT = 20904,
};

enum IllyannaEvents
{
    Volley,
    ArcaneBlast,
    ConcussiveShot,
    ImmolationTrap,
    AimedShot,

    //Non-spell events
    FerraCombatCheck
};

// boss_ill_raven
struct boss_illyanna_ravenoak : public BossAI
{
public:
    boss_illyanna_ravenoak(Creature* creature) : BossAI(creature, DMDataTypes::Illyanna_Ravenoak) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        //events.ScheduleEvent(ImmotharEvent::Enrage, 50s);

        //These timings are based on smart scripts from TC. No Nost data available.
        events.ScheduleEvent(IllyannaEvents::Volley, urandc(7s, 12s));
        events.ScheduleEvent(IllyannaEvents::ArcaneBlast, urandc(1s, 3s));
        events.ScheduleEvent(IllyannaEvents::ConcussiveShot, urandc(5s, 9s));
        events.ScheduleEvent(IllyannaEvents::ImmolationTrap, urandc(3s, 5s));
        events.ScheduleEvent(IllyannaEvents::FerraCombatCheck, 300ms); //we should check as soon as we are pulled

        this->DoZoneInCombat(); //this will help mitigate against split pulling ferra and illyana
    }

    void Reset() override
    {
        //If we reset we should check Ferra state.
        //Ferra will need to be respawned if we are alive and ferra is dead
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandleIllyanaEvents(static_cast<IllyannaEvents>(eventId));
    }

    void HandleIllyanaEvents(IllyannaEvents eventId)
    {
        switch (eventId)
        {
        case IllyannaEvents::Volley:
            //TODO: This may be exploitable by LoS. Need mangos SpellCastResult system in TC.
            //Smart script targets victim with volley, and rarely repeats every 1 minute
            DoCastVictim(IllyannaSpells::SPELL_VOLLEY);
            events.Repeat(urandc(60s, 65s));
            break;
        case IllyannaEvents::ArcaneBlast:
            //TODO: Same potential exploit as above.
            //Smart script targets victim with arcane blast.
            DoCastVictim(IllyannaSpells::SPELL_ARCANEBLAST);
            events.Repeat(urandc(6s, 9s));
            break;
        case IllyannaEvents::ConcussiveShot:
            //smart scripts target victim
            DoCastVictim(IllyannaSpells::SPELL_CONCUSSIVESHOT);
            events.Repeat(urandc(12s, 15s));
            break;
        case IllyannaEvents::ImmolationTrap:
            //Immolation trap is abit different, it casts the spell on herself (placing it under here)
            //But for some reason smart scripts also checks that the aura is not present. Does this even make sense??
            //The target is self, seems very odd.
            if (!me->HasAura(IllyannaSpells::SPELL_IMMOLATIONTRAP))
            {
                DoCastSelf(IllyannaSpells::SPELL_IMMOLATIONTRAP);
                events.Repeat(urandc(25s, 35s));
            }
            else
                events.Repeat(500ms); //if it fails we should retry, don't put on CD.
            break;
        case IllyannaEvents::FerraCombatCheck:
            if (Creature* ferra = this->instance->GetCreature(DMDataTypes::Ferra))
            {
                //Check if in combat to prevent split pulling exploits
                if (!ferra->IsInCombat())
                    ferra->AI()->DoZoneInCombat();

                //Reschedule a check to make sure Ferra stays in combat the whole time.
                events.Repeat(1s);
            }
            else
            {
                sLog->outCommand(0, "Unable to find creature ferra.");
                events.Repeat(300ms);
            }
            break;
        }
    }
};

void AddSC_boss_illyanna_ravenoak()
{
    RegisterCreatureAIWithFactory(boss_illyanna_ravenoak, GetDireMaulAI);
}
