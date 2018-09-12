#ifndef DEF_INSTANCEEVENTCONDITIONS_H
#define DEF_INSTANCEEVENTCONDITIONS_H

#include "InstanceEventCondition.h"
#include "Creature.h"
#include "Unit.h"
#include "GameObject.h"
#include "InstanceEventInvokable.h"

//TODO: Rename/namesapce/other??
struct TallyGoal //We brought this out of the class because it turned into a template.
{
    int CurrentCount;
    int Goal;

    TallyGoal(int goal)
        : CurrentCount(0), Goal(goal)
    {
        //TODO: Validate goal
    }

    TallyGoal()
        : CurrentCount(0), Goal(0)
    {

    }
};

//Shared tally condition works in such a way that when the tally reaches the
template<typename TEventSourceType>
class SharedTallyConditionDecorator : public InstanceEventCondition<TEventSourceType>
{
public:
    //Value Towards Goal represents the amount of count this tally condition is worth when the decorated condition completes.
    SharedTallyConditionDecorator(std::shared_ptr<TallyGoal> sharedTally, std::unique_ptr<InstanceEventCondition<TEventSourceType>> decoratedCondition, int valueTowardsGoal)
        : SharedTally(sharedTally), DecoratedCondition(std::move(decoratedCondition)), ValueTowardsGoal(valueTowardsGoal)
    {
        //TODO: Validate tally and fired values
        //This uses a shared pointer because this value must be shared between multiple conditions.
    }

    //Indicates if the event is ready to be dispatched.
    bool IsEventReady() const override
    {
        sLog->outCommand(0, "CurrentCount: %u Goal: %u", SharedTally->CurrentCount, SharedTally->Goal);
        //This is only ready when the counter reaches the specified amount.
        return SharedTally->CurrentCount >= SharedTally->Goal;
    }

    void Process(TEventSourceType* invoker) override
    {
        ASSERT(invoker);
        //Default is always true so do nothing.
        DecoratedCondition->Process(invoker);

        if (DecoratedCondition->IsEventReady())
            SharedTally->CurrentCount += ValueTowardsGoal;
    }
private:
    std::shared_ptr<TallyGoal> SharedTally;
    std::unique_ptr<InstanceEventCondition<TEventSourceType>> DecoratedCondition;
    int ValueTowardsGoal;
};

//Condition that completes when a list of spawn ids events have been handled.
//DO NOT GIVE THIS A VECTOR YOU WANT TO PRESERVE
class UnitAllSpawnIdListCondition : public InstanceEventCondition<Unit>
{
public:
    UnitAllSpawnIdListCondition(std::vector<int> spawnIdList)
        : SpawnIdList(std::move(spawnIdList)),
        Count(0)
    {

    }

    //Indicates if the event is ready to be dispatched.
    bool IsEventReady() const override
    {
        //Spawn id lists are only ready when the count matches the number of spawnids in the list
        return Count >= static_cast<int>(SpawnIdList.size());
    }

    void Process(Unit* invoker) override
    {
        //Spawn ID expects that the unit is a creature. So we should assert on that.
        Creature* c = invoker->ToCreature();
        ASSERT(invoker);
        ASSERT(c);

        //We make assumption that the list values are unique
        //Default is always true so do nothing.
        auto result = std::find_if(SpawnIdList.begin(), SpawnIdList.end(), [=](int id)
        {
            //TODO: Switch to uint. May be some that exceed int? I don't know, but we should switch.
            if (static_cast<int>(c->GetSpawnId()) == id)
                return true;
            else
                return false;
        });

        if (result != SpawnIdList.end())
            Count++;

        return;
    }

private:
    std::vector<int> SpawnIdList;
    int Count;

    UnitAllSpawnIdListCondition()
        : Count(0)
    {

    }
};

//TODO: Move this
class GameObjectActiveEventCondition : public InstanceEventCondition<GameObject>
{
public:
    GameObjectActiveEventCondition()
        : isActive(false)
    {

    }

    virtual bool IsEventReady() const override
    {
        return isActive;
    }

    virtual void Process(GameObject* invoker) override
    {
        ASSERT(invoker);
        ///We just check if it's active now.
        isActive = invoker->GetGoState() == GOState::GO_STATE_ACTIVE;
        return;
    }
private:
    bool isActive;
};

#endif
