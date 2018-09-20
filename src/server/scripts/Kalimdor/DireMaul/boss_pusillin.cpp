/* Copyright (C) 2018 HelloKitty/AndrewBlakely
MIT LICENSED
*/

#include "ScriptedCreature.h"
#include "instance_dire_maul.h"
#include "Random.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "PathGenerator.h"

enum PusillinState
{
    InitialStart = 0,
    EventPoint_1 = 1,
    EventPoint_2 = 2,
    EventPoint_3 = 3,
    EventPoint_4 = 4,
    CombatState = 5
};

enum PusillinMenuId
{
    Menu_1 = 6877,
    Menu_2 = 6878,
    Menu_3 = 6879,
    Menu_4 = 6880,
    Menu_5 = 6881,
};

enum PusillinText
{
    Response_1 = 9349,
    Response_2 = 9353,
    Response_3 = 9357,
    Response_4 = 9360,
    Response_5 = 9363
};

enum PusillinEvent
{
    RandomCastEvent = 1,
    RANDOM_SPELL_START_VALUE = 2,
    FireBallEvent = 2,
    FireBlastEvent = 3,
    BlastWaveEvent = 4,
    RANDOM_SPELL_END_VALUE = 4,
};

enum PusillinSpell
{
    SPELL_FIREBALL = 15228,
    SPELL_FIREBLAST = 14145,
    SPELL_BLASTWAVE = 22424,
    SPELL_RUNNTUM = 22735
};

int const IMP_CREATURE_SPAWN_COUNT = 5;
float const MOVE_SPEED = 20.0f;

Position const pusillinImpSpawnPositions[] =
{
    Position(6.562f, -712.43f, -12.64f),
    Position(23.994f, -697.89f, -12.64f),
    Position(22.216f, -688.01f, -12.64f),
    Position(17.943f, -679.68f, -12.64f),
    Position(9.540f, -671.08f, -12.64f),
};

Movement::PointsArray const pusillinWayPoints1 =
{
    //TODO: Create static PointsArray
    G3D::Vector3(86.144, -198.02, -4.13), //spawn point
    G3D::Vector3(-163.36f, -201.44f, -4.2f),
    //Arrival point
    G3D::Vector3(-145.0f, -296.9f, -4.12f) //final point
};

Movement::PointsArray const pusillinWayPoints2 =
{
    G3D::Vector3(-145.0f, -296.9f, -4.12f), //point from previous
    G3D::Vector3(-133.93f, -351.13, -4.08f),
    G3D::Vector3(112.7f, -353.87f, -4.12f) //final point
};

Movement::PointsArray const pusillinWayPoints3 =
{
    G3D::Vector3(112.7f, -353.87f, -4.12f), //point from previous
    G3D::Vector3(114.46f, -354.853f, -4.13f), //before lump
    G3D::Vector3(111.66f, -353.45f, -2.71f), //top lump
    G3D::Vector3(111.46f, -380.33f, -2.71f), //lump edge
    G3D::Vector3(111.46f, -382.79f, -3.27f), //past lump
    G3D::Vector3(111.83f, -469.59f, -2.75f), //hallway end
    G3D::Vector3(111.9f, -485.68f, -6.88f), //ramp end1
    G3D::Vector3(111.92f, -515.42f, -6.91f), //hallway end2
    G3D::Vector3(111.94f, -530.92f, -11.08f), //ramp end2
    G3D::Vector3(106.80f, -539.52f, -11.08f), //ramp2 curve
    G3D::Vector3(92.84f, -544.80f, -11.08f), //ramp2 curve2
    G3D::Vector3(78.83f, -545.14f, -15.21f), //ramp3
    G3D::Vector3(67.56f, -548.077f, -15.24f), //ramp3
    G3D::Vector3(54.66f, -557.31f, -19.41f), //ramp3 end
    G3D::Vector3(50.48f, -566.30f, -19.43f), //ramp3 curve
    G3D::Vector3(50.17f, -586.06f, -23.57f), //ramp3 bottom?
    G3D::Vector3(50.32f, -588.73f, -23.59f), //ramp3 bottom forward
    G3D::Vector3(50.26f, -594.25f, -25.03f), //ramp3 miniramp bottom
    G3D::Vector3(50.99f, -632.68f, -25.12f) //final point
};

Movement::PointsArray const pusillinWayPoints4 =
{
    G3D::Vector3(50.99f, -632.68f, -25.12f), //previous point
    G3D::Vector3(35.83f, -689.0f, -25.16f), //pillar
    G3D::Vector3(2.57f, -702.62f, -25.16f), //bottom ramp
    G3D::Vector3(2.88f, -694.69f, -25.16f), //bottom ramp closer
    G3D::Vector3(2.88f, -671.70f, -12.64f), //bottom ramp top
    G3D::Vector3(17.43f, -669.50f, -12.64f), //ramp side
    G3D::Vector3(19.091084f, -704.739746f, -12.642583f) //final point
};

struct boss_pusillin : public BossAI
{
public:
    boss_pusillin(Creature* creature)
        : BossAI(creature, DMDataTypes::Pusillin), State(PusillinState::InitialStart)
    {
    }

    // Called when a player opens a gossip dialog with the creature.
    virtual bool GossipHello(Player* player)
    {
        switch (State)
        {
            //TODO: Handle Gossip better
        case PusillinState::InitialStart:
            SendPusillinGossip(player, PusillinMenuId::Menu_1);
            break;
        case PusillinState::EventPoint_1:
            SendPusillinGossip(player, PusillinMenuId::Menu_2);
            break;
        case PusillinState::EventPoint_2:
            SendPusillinGossip(player, PusillinMenuId::Menu_3);
            break;
        case PusillinState::EventPoint_3:
            SendPusillinGossip(player, PusillinMenuId::Menu_4);
            break;
        case PusillinState::EventPoint_4:
            SendPusillinGossip(player, PusillinMenuId::Menu_5);
            break;
        default:
            break;
        }

        //TODO: What is this?
        return true;
    }

    //basically BossAI::Reset without summon despawn
    void Reset() override
    {
        if (!me->IsAlive())
            return;

        me->SetCombatPulseDelay(0);
        me->ResetLootMode();
        events.Reset();

        //We don't despawn all the summons because that would change the META on him, to pull and then feign death or something.
        //summons.DespawnAll();
        scheduler.CancelAll();
        if (instance && instance->GetBossState(DMDataTypes::Pusillin) != DONE)
            instance->SetBossState(DMDataTypes::Pusillin, NOT_STARTED);

        //Cast recast Runn Tumm. He should have it all the time I think.
        SetPusillinCombatStates();
    }

    //Only does anything if in combat state
    void SetPusillinCombatStates()
    {
        if (State >= PusillinState::CombatState)
        {
            me->SetObjectScale(0.7f);
            DoCastSelf(PusillinSpell::SPELL_RUNNTUM, CastSpellExtraArgs(true));
        }
    }

    void JustReachedHome() override
    {
        BossAI::JustReachedHome();

        //Cast recast Runn Tumm. He should have it all the time I think.
        SetPusillinCombatStates();
    }

    //basically BossAI::JustDied without summon despawn
    void JustDied(Unit* /*killer*/) override
    {
        events.Reset();

        //Don't remove imps
        //summons.DespawnAll();
        scheduler.CancelAll();
        if (instance)
            instance->SetBossState(DMDataTypes::Pusillin, DONE);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);

        events.ScheduleEvent(PusillinEvent::RandomCastEvent, 0);

        SetPusillinCombatStates();

        //To prevent split pulling we should engage all of his summoned imps
        summons.DoZoneInCombat();
    }

    void ExecuteEvent(uint32 eventId) override
    {
        HandlePusillinEvents(static_cast<PusillinEvent>(eventId));
    }

    void HandlePusillinEvents(PusillinEvent eventId)
    {
        //TODO: Current behavior means the cast at the last second can be LoS'd, and it won't recast and will still be put on CD. How should we deal with that?
        switch (eventId)
        {
        case PusillinEvent::RandomCastEvent:
            //The way Pusillin's casts work is we randomly schedule a cast event based on what we roll
            //this event only calculates what we will cast, it does not cast it.
            events.ScheduleEvent(urand(PusillinEvent::RANDOM_SPELL_START_VALUE, PusillinEvent::RANDOM_SPELL_END_VALUE), 0s);
            break;
        case PusillinEvent::FireBallEvent:
            PusillinSpellCast(PusillinSpell::SPELL_FIREBALL, 6s);
            break;
        case PusillinEvent::FireBlastEvent:
            PusillinSpellCast(PusillinSpell::SPELL_FIREBLAST, 4s);
            break;
        case PusillinEvent::BlastWaveEvent:
            PusillinSpellCast(PusillinSpell::SPELL_BLASTWAVE, 9s);
            break;
        default:
            break;
        }
    }

    void PusillinSpellCast(PusillinSpell spell, std::chrono::seconds cooldownTime)
    {
        DoCastVictim(spell);
        events.ScheduleEvent(PusillinEvent::RandomCastEvent, cooldownTime);
    }

    void SendPusillinGossip(Player* player, PusillinMenuId menu)
    {
        AddGossipItemFor(player, menu, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, menu, me->GetGUID()); //TODO: What is the second parameter?
    }

    // Called when a player selects a gossip item in the creature's gossip menu.
    virtual bool GossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/)
    {
        //We don't need to know anything but the current state
        switch (State)
        {
        case PusillinState::InitialStart:
            PusillinMoveEvent(PusillinText::Response_1, pusillinWayPoints1, PusillinState::EventPoint_1);
            break;
        case PusillinState::EventPoint_1:
            PusillinMoveEvent(PusillinText::Response_2, pusillinWayPoints2, PusillinState::EventPoint_2);
            break;
        case PusillinState::EventPoint_2:
            PusillinMoveEvent(PusillinText::Response_3, pusillinWayPoints3, PusillinState::EventPoint_3);
            break;
        case PusillinState::EventPoint_3:
            PusillinMoveEvent(PusillinText::Response_4, pusillinWayPoints4, PusillinState::EventPoint_4);
            break;
        case PusillinState::EventPoint_4:
            me->SetHomePosition(18.19f, -701.15f, -12.64f, 0);
            me->Say(PusillinText::Response_5);
            me->SetFaction(14);
            SetPusillinCombatStates();
            SpawnImps();
            SetAllImpsAttackTargetPlayer(player);
            me->Attack(player, true); //make the imp attack right away.
            State = PusillinState::CombatState;
            break;
        }

        //TODO: What is this?
        return false;
    }

    void PusillinMoveEvent(PusillinText text, const Movement::PointsArray& points, PusillinState newState)
    {
        me->Say(text);

        //Manually define the spline
        Movement::MoveSplineInit init(me);
        init.MovebyPath(points);
        init.SetVelocity(MOVE_SPEED);
        me->GetMotionMaster()->LaunchMoveSpline(std::move(init), 0, MOTION_PRIORITY_NORMAL, POINT_MOTION_TYPE);

        State = newState;
    }

    void SpawnImps()
    {
        for (int i = 0; i < IMP_CREATURE_SPAWN_COUNT; i++)
        {
            SpawnImpGuard(pusillinImpSpawnPositions[i]);
        }
    }

    void SpawnImpGuard(const Position& pos)
    {
        //TODO: Log if fails?
        if (Creature* tmp = me->SummonCreature(NPC_IMP_SPAWN, pos.m_positionX, pos.m_positionY, pos.m_positionZ, 4.25f, TEMPSUMMON_DEAD_DESPAWN, 0))
        {
            tmp->SetWalk(false);
            tmp->SetHomePosition(pos.m_positionX, pos.m_positionY, pos.m_positionZ, 0);
        }
    }

    void SetAllImpsAttackTargetPlayer(Player* player)
    {
        //Orginally we are going to set combat for the player gossiping
        //however this was exploitable since you could technically feign death and drap Pussilin away
        //so to avoid an exploit such as that we just set them all in combat with the instance.
        this->summons.DoZoneInCombat();
    }

    // Called when a player selects a gossip with a code in the creature's gossip menu.
    //virtual bool GossipSelectCode(Player* /*player*/, uint32 /*menuId*/, uint32 /*gossipListId*/, char const* /*code*/) { return false; }

private:
    PusillinState State;
};


void AddSC_boss_pusillin()
{
    RegisterCreatureAIWithFactory(boss_pusillin, GetDireMaulAI);
}
