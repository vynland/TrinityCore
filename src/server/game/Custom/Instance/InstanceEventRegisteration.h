#ifndef DEF_INSTANCEEVENTREGISTERATION_H
#define DEF_INSTANCEEVENTREGISTERATION_H

#include "InstanceEventCondition.h"
#include "InstanceEventInvokable.h"

template<typename TEventSourceType>
class InstanceEventRegisteration
{
public:
    InstanceEventRegisteration(std::unique_ptr<InstanceEventCondition<TEventSourceType>> condition, InstanceEventInvokable invokable)
        : Condition(std::move(condition)), Invokable(invokable)
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

private:
    std::unique_ptr<InstanceEventCondition<TEventSourceType>> Condition;
    InstanceEventInvokable Invokable;
};

#endif
