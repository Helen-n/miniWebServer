#ifndef HANDLER_H
#define HANDLER_H

#include<iostream>
#include<boost/noncopyable.hpp>
#include "Callback.h"

#define MAX_BUFFLEN 1024

class Handler:boost::noncopyable
{
    friend class EpollPoller;
public:
    Handler(int fd);
    ~Handler();
    void HandleEvent();
    void setReadCallback(EventCallback);
    void setWriteCallback(EventCallback);
    void setCloseCallback(EventCallback);
    void enableRead();
    void enableWrite();
    void enableAll();
    void disableAll();

    int fd() const {return fd_;}
    int events() const {return events_;}
    int revents() const {return revents_;}
    char *buff() {return buff_;}
    void setLen(int len){len_=len;}
    int length() const {return len_;}
private:
    int fd_;
    int events_;
    int revents_;
    bool isInEpoll;
    char buff_[MAX_BUFFLEN];
    int len_;
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};


#endif // HANDLER_H
