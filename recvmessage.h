//
// Created by lansnow on 2021/3/8.
//

#ifndef CLOUDSERVER_RECVMESSAGE_H
#define CLOUDSERVER_RECVMESSAGE_H
#include "config.h"
#include "streamsock.h"

/*
C++标准线程
获取sock的数据，服务器端，仅作基础消息交互，不涉及到数据收录
数据收录的入口会是一个比较大的问题。
同时，收录双方数据也会是一个问题。

这里有个问题。耦合从哪里解决。

*/
class recvmessage {

public:
    streamsock *sSock;
public:

    recvmessage(streamsock *sSock);
    ~recvmessage();
    void recvData();
};

#endif //CLOUDSERVER_RECVMESSAGE_H
