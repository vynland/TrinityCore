#ifndef DEF_INSTANCEEVENTREGISTERATION_H
#define DEF_INSTANCEEVENTREGISTERATION_H

#include "InstanceEventCondition.h"
#include "InstanceEventInvokable.h"

//The registeration type to register an event with.
enum InstanceEventRegisterationType
{
    //Single use means to unschedule the event once it has fired.
    SingleUse = 0,

    //Presistent means to not unschedule the event.
    Persistent = 1,
};

template<typename TEventSourceType>
class InstanceEventRegisteration
{
public:
    InstanceEventRegisteration(std::unique_ptr<InstanceEventCondition<TEventSourceType>> condition, InstanceEventInvokable invokable, InstanceEventRegisterationType registerationType)
        : Condition(std::move(condition)), Invokable(invokable), RegisterationType(registerationType)
    {

    }

    InstanceEventCondition<TEventSourceType>* GetCondition()
    {
        return Condition.get();
    }

    InstanceEventInvokable GetInvokable() const
    {
        return Invokable;
    }

    InstanceEventRegisterationType GetRegisterationType() const
    {
        return RegisterationType;
    }

private:
    std::unique_ptr<InstanceEventCondition<TEventSourceType>> Condition;
    InstanceEventInvokable Invokable;
    InstanceEventRegisterationType RegisterationType;
};

#endif
