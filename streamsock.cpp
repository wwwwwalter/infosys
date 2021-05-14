//
// Created by lansnow on 2021/3/8.
//
#include "streamsock.h"
#include "config.h"
#include <iostream>
#include <map>
#include <error.h>

#include <sys/socket.h>
#include <sys/types.h>

extern std::vector<User> userList;
extern std::map<int, std::vector<User>> rooms;


streamsock::streamsock()
{

}

streamsock::~streamsock()
{


}

//发送消息到指定客户
void streamsock::sendData(const char* uName, const char* message)
{
    //查下用户
    findUser(uName);
    if (this->sendToPort != 0) {
        //这里是找到用户后，需要给处理的消息

    }

    //如果没有找到用户，则进行下面的操作。

}

bool streamsock::init()
{
    /*
     * Linux not need this config
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Windows sockets 2.2 startup");
        return false;
    }
    */
    _sSock = socket(AF_INET, SOCK_DGRAM, 0);//这里固定下来是UDP的形式
    if (_sSock < 0)
    {
        printf("create socket error");
        return false;
    }

    //完成本地监听
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(serverPort);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    int nResult = bind(_sSock, (sockaddr*)&local, sizeof(sockaddr));
    if (nResult == -1) {
        printf("bind socket error");
        return false;
    }
    std::cout<<"Create and bind is ok!"<<std::endl;
    return true;


}

void streamsock::setSender(sockaddr_in  sc, USHORT port)
{

    fromSender = sc;
    formPort = port;
}

void streamsock::findUser(const char * name)
{

    for (auto &c : userList) {
        /*
        if (strcmp(c->userName, name) == 0) {
            //this->sendTo = c->roomId;
            //this->sendToPort = c->roomId;
        }
        */
    }
    this->sendToPort = 0;

}


