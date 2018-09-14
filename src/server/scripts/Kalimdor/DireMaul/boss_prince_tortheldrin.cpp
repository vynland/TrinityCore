/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"

enum TortheldrinSpells
{
    SPELL_ARCANE_BLAST = 22920,
    SPELL_COUNTERSPELL = 20537,
    SPELL_SUMMON = 22995,
    SPELL_THRASH = 8876,
    SPELL_WHIRLWIND = 15589,
    SPELL_THRASH_EFFECT_AURA = 3391,
};

enum TortheldrinEvent
{
    ArcaneBlast = 1,

    CounterSpell = 2,

    Summoning = 3,

    WhirlWind = 4
};

// Prince Tortheldrin's boss AI.
struct boss_prince_tortheldrin : public BossAI
{
public:
    boss_prince_tortheldrin(Creature* creature) : BossAI(creature, DireMaulBossIndex::Prince_Tortheldrin) { }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);

        events.ScheduleEvent(TortheldrinEvent::ArcaneBlast, urandc(15s, 20s));
        events.ScheduleEvent(TortheldrinEvent::CounterSpell, urandc(10s, 20s));
        events.ScheduleEvent(TortheldrinEvent::Summoning, urandc(0s, 3s));
        events.ScheduleEvent(TortheldrinEvent::WhirlWind, urandc(14s, 22s));

        DoCastSelf(TortheldrinSpells::SPELL_THRASH, CastSpellExtraArgs(TriggerCastFlags::TRIGGERED_FULL_MASK));
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandlePrinceEvents(static_cast<TortheldrinEvent>(eventId));
    }

    void HandlePrinceEvents(TortheldrinEvent eventId)
    {
        switch (eventId)
        {
            //TODO: Mangos offers a way to check if a spell failed. We need a system like that, maybe.
        case TortheldrinEvent::Summoning:
            DoCastVictim(SPELL_SUMMON);
            events.Repeat(urandc(13s, 20s));
            break;
            //Conceptually we want to search for nearby units.
            //And if any are in melee range we want to whirlwind.
            //However, the are MANY ways to search and with conditions in TC.
            //Our minimum requirement is that a unit exist within melee range in combat
            //so we don't have to search the whole grid, only based on the threat list
            //and we don't want just players. Also units like pets.
        case TortheldrinEvent::WhirlWind:
            //We reschedule the whirlwind event if it doesn't go offer, like Nostalrius. But we don't want to check nonstop so check every second.
            if (IsEnemyUnitInMeleeRange(7.0f))
            {
                DoCastSelf(TortheldrinSpells::SPELL_WHIRLWIND);
                events.Repeat(urandc(10s, 20s));
            }
            else
                events.Repeat(500ms); //We want to do this often, but too often would be too much iterating spent since worst case is all casters with ranged pets.
            break;
        case TortheldrinEvent::ArcaneBlast:
            //TODO: We can't actually implement this properly, since TC doesn't indicate if a spell has been successfully cast. But we need to drop threat
            DoCastVictim(TortheldrinSpells::SPELL_ARCANE_BLAST);
            ResetThreatList();
            events.Repeat(urandc(10s, 15s));
            break;
        case TortheldrinEvent::CounterSpell:
            if (Unit* target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, [&](Unit* u) 
            {
                if (u->GetTypeId() == TypeID::TYPEID_PLAYER && u->GetPowerType() == Powers::POWER_MANA)
                {
                    //Do distance calculation second since it's more costly
                    if(27.0f >= u->GetDistance2d(me)) //30 yard range
                        return u->IsNonMeleeSpellCast(true);
                }

                return false;
            }))
            {
                //So if we're here we've found a random player caster within the range of the spell.
                DoCast(target, TortheldrinSpells::SPELL_COUNTERSPELL);
                events.Repeat(urandc(25s, 30s));
            }
            else
                events.Repeat(500ms); //If we can't CS anyone now then we should try to search again until we can.
            break;
        default:
            break;
        }
    }

    bool IsEnemyUnitInMeleeRange(float range)
    {
        //We iterate the threat list until we find a unit that is online AND in range of the specified range.
        auto engagedUnits = this->me->GetThreatManager().GetUnsortedThreatList();
        for (const auto u : engagedUnits)
        {
            if (u->GetOnlineState() == ThreatReference::OnlineState::ONLINE_STATE_ONLINE)
                if (me->IsWithinCombatRange(u->GetOwner(), range))
                    return true;
        }

        return false;
    }
};

void AddSC_boss_prince_tortheldrin()
{
    RegisterCreatureAIWithFactory(boss_prince_tortheldrin, GetDireMaulAI);
}
