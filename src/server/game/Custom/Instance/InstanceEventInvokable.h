#ifndef DEF_INSTANCEEVENTINVOKABLE_H
#define DEF_INSTANCEEVENTINVOKABLE_H

#include <functional>
#include "Errors.h"
#include "ObjectGuid.h"

//TODO: We want to support many types of patterns seen in instances.
class InstanceEventInvokable
{
public:
    typedef std::function<void(ObjectGuid&)> InstanceEventInvokerFunction;

    //The target callback for the event, should accept a guid value, and the target instance.
    InstanceEventInvokable(const InstanceEventInvokerFunction invokable)
        : Invokable(invokable)
    {
        ASSERT(invokable != nullptr);
    }

    //The invoker should be that guid of the object that cause the event to happen.
    void Invoke(ObjectGuid invoker) const
    {
        ASSERT(!invoker.IsEmpty());
        (Invokable)(invoker);
    }

    template<typename TType, typename TFunctionPointerType> //I couldn't figure out the proper signature, C++ is annoying.
    static InstanceEventInvokerFunction MakeMemberFunctionEvent(TType* thisPointer, TFunctionPointerType memberFunction)
    {
        //Example: [&](ObjectGuid o) { this->OnPylonGuardiansGroupDeath(DireMaulGameObjectEntry::GO_CRISTAL_4_EVENT); };
        auto func = std::bind(memberFunction, thisPointer, std::placeholders::_1);
        return static_cast<InstanceEventInvokerFunction>(func);
    }

private:
    InstanceEventInvokerFunction Invokable;
};

#endif
