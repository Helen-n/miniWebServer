#include "Reactor.h"
#include "EpollPoller.h"
#include "Handler.h"

void Reactor::registerHandler(Handler* handler)
{
    poller_->update(handler);
}

void Reactor::removeHandler(Handler* handler)
{
    poller_->remove(handler);
}

void Reactor::loop()
{
    while(1)
    {
        auto activateEventPtrList = poller_->poll();
        if(!activateEventPtrList.empty())
        {
            for(int i=0; i < activateEventPtrList.size(); i++)
            {
                activateEventPtrList[i]->HandleEvent();
            }
        }
        else
        {

        }
    }
}
