#ifndef DEF_INSTANCEEVENTLISTMANAGER_H
#define DEF_INSTANCEEVENTLISTMANAGER_H

#include "InstanceEventInvokable.h"
#include "ObjectGuid.h"
#include "Unit.h"
#include "GameObject.h"
#include "InstanceEventRegisteration.h"

template<typename TKeyType, typename TEventSourceType>
class InstanceEventRegister
{
public:
    //Registers an event with the given registeration data associated with the event input.
    virtual void RegisterEvent(TKeyType invoker, std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registerationData) = 0;

    //TODO: Support a shared registeration between multiple invokes (like a group of NPCs that all must die before an event fires)
};

template<typename TEventSourceType>
class InstanceEventProcessor
{
public:
    //Should be called to process a potential event.
    //May not actually raise an event if none are registered/listening for an event involving this potential invoker.
    virtual void ProcessEvent(TEventSourceType* potentialInvoker) = 0;
};

//Strategy for producing the key used for an event.
template<typename TKeyType, typename TEventSourceType>
class InstanceEventKeyUnitStrategy
{
    //TODO: Figure out why this static assert doesn't work.
    static_assert(std::is_enum<TKeyType>::value, "Only supported template types are enums and ObjectGuid.");
public:
    TKeyType GetKey(TEventSourceType* u)
    {
        //Assume anything else is the entry
        return static_cast<TKeyType>(u->GetEntry());
    }
};

template<>
class InstanceEventKeyUnitStrategy<ObjectGuid, Unit>
{
public:
    ObjectGuid GetKey(Unit* u)
    {
        return u->GetGUID();
    }
};

template<>
class InstanceEventKeyUnitStrategy<ObjectGuid, GameObject>
{
public:
    ObjectGuid GetKey(Unit* u)
    {
        return u->GetGUID();
    }
};

template<typename TKeyType, typename TEventSourceType>
class InstanceUnitEventListManager : public InstanceEventProcessor<TEventSourceType>, public InstanceEventRegister<TKeyType, TEventSourceType>
{
public:
    void RegisterEvent(TKeyType invoker, std::unique_ptr<InstanceEventRegisteration<TEventSourceType>> registerationData) override
    {
        auto i = ListenerMap.find(invoker);
        if (i == ListenerMap.end()) //if there is none
        {
            std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>> invokationList;
            invokationList.push_back(std::move(registerationData));
            ListenerMap.emplace(std::make_pair(invoker, std::move(invokationList)));
        }
        else
        {
            //We just put it in the vector of registered events.
            ListenerMap.at(invoker).push_back(std::move(registerationData));
        }
    }

    void ProcessEvent(TEventSourceType* potentialInvoker)
    {
        ASSERT(potentialInvoker);

        //We invoke by entry for this event manager so we need to get the entry
        TKeyType key = KeyParser.GetKey(potentialInvoker);

        //TODO: Look into using find instead of at everywhere to make this run faster, make sure it doesn't copy.
        if (ListenerMap.find(key) != ListenerMap.end())
        {
            std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>>& invokationList = ListenerMap.at(key);

            invokationList.erase(std::remove_if(invokationList.begin(), invokationList.end(), [&](const auto& eventData)
            {
                //TODO: This is kinda a hack, to support linked events. We check if they're ready, which they shouldn't be, because we haven't processed anything yet
                //if they're ready it means they are likely a linked event
                if (eventData.get()->GetRegisterationType() == InstanceEventRegisterationType::SingleUse && eventData->GetCondition()->IsEventReady())
                    return true;

                eventData->GetCondition()->Process(potentialInvoker);

                bool ready = eventData.get()->GetCondition()->IsEventReady();

                sLog->outCommand(0, "GUID: %u about to Fire Event: (0 False) (1 True) %u. Name: %s", potentialInvoker->GetGUID().GetCounter(), ready, potentialInvoker->GetName());

                if (ready)
                {
                    eventData.get()->GetInvokable().Invoke(potentialInvoker->GetGUID());

                    if ((eventData.get()->GetRegisterationType() == InstanceEventRegisterationType::SingleUse))
                        return true;
                    else
                    {
                        //TODO: Implement a reset for conditions.
                        //it is persistent if it's not single use
                        //therefore we must call reset on the event and not remove it
                        return false;
                    }
                }
                else
                    return false;

            }), invokationList.end());

            //We should remove the vector if it is empty from the map
            if (invokationList.empty())
                ListenerMap.erase(ListenerMap.find(key));
        }
    }

private:
    std::map<TKeyType, std::vector<std::unique_ptr<InstanceEventRegisteration<TEventSourceType>>>> ListenerMap;
    InstanceEventKeyUnitStrategy<TKeyType, TEventSourceType> KeyParser;
};

#endif
