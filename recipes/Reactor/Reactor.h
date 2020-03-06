#ifndef REACTOR_H
#define REACTOR_H
#include"EpollPoller.h"

class Handler;

class Reactor
{
    public:
        Reactor():poller_(new EpollPoller){};
        void registerHandler(Handler*);
        void removeHandler(Handler*);
        void loop();
    private:
        std::unique_ptr<EpollPoller> poller_;
};

#endif // REACTOR_H
