#include "EpollPoller.h"
#include "Handler.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>

EpollPoller::EpollPoller():activateEventList_(kInitEventListSize)
{
    epfd_ = epoll_create1(EPOLL_CLOEXEC);
}

EpollPoller::~EpollPoller()
{
    close(epfd_);
}

void EpollPoller::update(Handler* handler)
{
    epoll_event epEvent;
    epEvent.events = handler->events();
    epEvent.data.ptr = handler;

    int op;
    if(handler->isInEpoll)
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_ADD;
    }
    handler->isInEpoll = true;

    int ret = epoll_ctl(epfd_, op, handler->fd(), &epEvent);

    if(ret<0)
    {
        perror("epoll_ctl_ADD");
        printf("epoll_ctl_ error on fd = %d\n", handler->fd());
    }
    else
    {
        printf("epoll_ctl on fd = %d success\n", handler->fd());
    }
}

void EpollPoller::remove(Handler* handler)
{
    assert(handler->isInEpoll);
    handler->isInEpoll = false;
    int op = EPOLL_CTL_DEL;
    int ret = epoll_ctl(epfd_, op, handler->fd(), NULL);
    if(ret < 0)
    {
        perror("epoll_ctl DEL");
        printf("epoll_ctl error on fd = %d\n", handler->fd());
    }
    handler->isInEpoll = false;
}

std::vector<Handler*> EpollPoller::fillActivateChannels(int activateNums)
{
    std::vector<Handler*> activateEventPtrList;
    for(int i=0; i < activateNums; ++i)
    {
        Handler* handlerPtr = static_cast<Handler*>(activateEventList_[i].data.ptr);
        if(activateEventList_[i].events & EPOLLIN)
        {
            handlerPtr->revents_ = EPOLLIN;
        }
        if(activateEventList_[i].events & EPOLLOUT)
        {
            handlerPtr->revents_ = EPOLLOUT;
        }
        activateEventPtrList.push_back(handlerPtr);
    }
    return activateEventPtrList;
}

std::vector<Handler*> EpollPoller::poll()
{
    std::vector<Handler*> activateEvenPtrtList;
    int nreadys = epoll_wait(epfd_, activateEventList_.data(), activateEventList_.size(), -1);
    if(nreadys > 0)
    {
        // 将就绪的eventListc传递到Reactor
        activateEvenPtrtList = fillActivateChannels(nreadys);
        if(nreadys == activateEventList_.size())
        {
            activateEventList_.resize(activateEventList_.size()*2);
        }
    }
    else if(nreadys == 0)
    {

    }
    else
    {
        printf("nready = %d, error\n", nreadys);
    }
    return activateEvenPtrtList;
}












