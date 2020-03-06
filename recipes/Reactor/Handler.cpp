#include "Handler.h"
#include <sys/epoll.h>

Handler::Handler(int fd):fd_(fd),events_(0),revents_(0),isInEpoll(false),len_(0)
{

}

Handler::~Handler()
{

}

void Handler::HandleEvent()
{
    if(revents_&EPOLLIN)
    {
        readCallback_(fd_);
    }
    if(revents_&EPOLLOUT)
    {
        writeCallback_(fd_);
    }
}

void Handler::setReadCallback(EventCallback cb)
{
    readCallback_ = cb;
}

void Handler::setWriteCallback(EventCallback cb)
{
    writeCallback_ = cb;
}

void Handler::setCloseCallback(EventCallback cb)
{
    closeCallback_ = cb;
}

void Handler::enableRead()
{
    events_ = EPOLLIN;
}

void Handler::enableWrite()
{

    events_ = EPOLLOUT;
}

void Handler::enableAll()
{
    events_ |= EPOLLIN;
    events_ |= EPOLLOUT;
}

void Handler::disableAll()
{
    events_ = 0;
}















