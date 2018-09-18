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
    Volley = 1,
    ArcaneBlast = 2,
    ConcussiveShot = 3,
    ImmolationTrap = 4,
    AimedShot = 5,

    //Non-spell events
    FerraCombatCheck = 6
};

enum FerraSpells
{
    SPELL_CHARGE = 22911,
    SPELL_MAUL = 17156
};

enum FerraEvents
{
    Charge = 1,

    Maul = 2,

    //Non-spell events
    IllyanaCombatCheck = 3
};

// boss_ill_raven
struct boss_illyanna_ravenoak : public BossAI
{
public:
    boss_illyanna_ravenoak(Creature* creature) : BossAI(creature, DMDataTypes::Illyanna_Ravenoak) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();

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
        BossAI::Reset();

        //If we reset we should check Ferra state.
        //Ferra will need to be respawned if we are alive and ferra is dead
        if (Creature* ferra = FindFerra())
        {
            //If ferra is dead we want to respawn her, ferra is not summoned or a pet so it's not handled automatically.
            if (ferra->isDead())
            {
                ferra->Respawn(true);
            }
        }
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
            //TODO: Consolidate duplicate code shared between this hack to make linked bosses stay in combat no matter what.
            if (Creature* ferra = FindFerra())
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

    Creature* FindFerra()
    {
        return this->instance->GetCreature(DMDataTypes::Ferra);
    }
};

struct boss_ferra : public BossAI
{
public:
    boss_ferra(Creature* creature) : BossAI(creature, DMDataTypes::Ferra) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();

        //Nost charges instantly
        events.ScheduleEvent(FerraEvents::Charge, 0);
        events.ScheduleEvent(FerraEvents::Maul, urandc(5s, 10s));

        this->DoZoneInCombat(); //this will help mitigate against split pulling ferra and illyana
        me->SetNoCallAssistance(true); //this is what Nost does
        events.ScheduleEvent(FerraEvents::IllyanaCombatCheck, 300ms); //we should check as soon as we are pulled
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandleFerraEvents(static_cast<FerraEvents>(eventId));
    }

    void HandleFerraEvents(FerraEvents eventId)
    {
        switch (eventId)
        {
            //DMDataTypes::Illyanna_Ravenoak
        case FerraEvents::Charge:
            //Vmangos/nost charges at current target, then reshecules the event to happen about 8 seconds later
            DoCastVictim(FerraSpells::SPELL_CHARGE);
            events.Repeat(urandc(6s, 10s));
            break;
        case FerraEvents::Maul:
            DoCastVictim(FerraSpells::SPELL_MAUL);
            events.Repeat(urandc(15s, 20s));
            break;
        case FerraEvents::IllyanaCombatCheck:
            //Like illyana we check to make sure illyana is in combat if ferra is
            //TODO: Consolidate duplicate code shared between this hack to make linked bosses stay in combat no matter what.
            if (Creature* ill = this->instance->GetCreature(DMDataTypes::Illyanna_Ravenoak))
            {
                //Check if in combat to prevent split pulling exploits
                if (!ill->IsInCombat())
                    ill->AI()->DoZoneInCombat();

                //Reschedule a check to make sure Ferra stays in combat the whole time.
                events.Repeat(1s);
            }
            else
            {
                sLog->outCommand(0, "Unable to find creature ill.");
                events.Repeat(300ms);
            }
            break;
        }
    }
};

void AddSC_boss_illyanna_ravenoak()
{
    RegisterCreatureAIWithFactory(boss_illyanna_ravenoak, GetDireMaulAI);
    RegisterCreatureAIWithFactory(boss_ferra, GetDireMaulAI);
}
