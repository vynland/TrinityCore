#ifndef DEF_INSTANCEEVENTCONDITION_H
#define DEF_INSTANCEEVENTCONDITION_H

//The default condition is always ready for invokation.
template<typename TEventSourceType>
class InstanceEventCondition
{
public:
    InstanceEventCondition() { }

    static InstanceEventCondition<TEventSourceType> Default;

    //Indicates if the event is ready to be dispatched.
    virtual bool IsEventReady() const
    {
        return true;
    }

    //Process the event source.
    virtual void Process(TEventSourceType* invoker)
    {
        ASSERT(invoker);
        //Default is always true so do nothing.
        return;
    }
};

#endif
