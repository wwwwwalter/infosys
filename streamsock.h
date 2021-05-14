//
// Created by lansnow on 2021/3/8.
//

#ifndef CLOUDSERVER_STREAMSOCK_H
#define CLOUDSERVER_STREAMSOCK_H

#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define USHORT int
#define SOCKET int

class streamsock
{
public:
    streamsock();
    ~streamsock();
    void sendData(const char* uName,const char* message);//发送消息
    bool init();
    void setSender(sockaddr_in sc, USHORT port);
public:
    SOCKET _sSock;
    std::vector<User*> userList;

private:
    USHORT serverPort = 6060;
    //远程服务器IP、端口（来访）
    //需要发送的服务器IP、端口（目的地）
    sockaddr_in fromSender, sendTo;
    USHORT formPort, sendToPort;
private:
    void findUser(const char* name);

};

#endif //CLOUDSERVER_STREAMSOCK_H
