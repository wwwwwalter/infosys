//
// Created by lansnow on 2021/3/8.
//

#ifndef CLOUDSERVER_CONFIG_H
#define CLOUDSERVER_CONFIG_H

#include <vector>
struct User
{
    char userName[20];
    int userIp;
    unsigned short userPort;
    bool isMaster;
    int RoomId;
    int channelId;
    bool isLan;//局域网内的信息不需要调整
};



#define MSGLOGIN		101
#define MSGLOGINSUCESS	1011
#define MSGLOGINFIAL	1012
#define UPDATAUSER		1013
#define SETMASTER		1014 //对于非主播，设置主播信息，用来跟主播交换信息的
#define MSGLOGOUT		102

#define USERLOGOUT	1015 //用户下线通知


#define P2PMESSAGE	200//p2p消息，发送给到对方
#define P2PYNC		201//P2P同步，发送给服务器，然后再发送到客户端，对方需要发送确认信息
#define P2PACK		202 //P2P响应消息类型，客户端收到后，可以响应服务器端，用来保证数据传送完成，

#define P2PRANS		203 //穿墙信息？
#define P2PSOMEONEWANTTOCALLYOU 204 //服务器发给客户端，有人要给他发送P2P数据
#define P2PRASH 205 //告诉请求P2P的用户，我同意你发送消息


#define USERMASTER 301//master用户
#define USERORDINARY	302//普通用户

#define PACKSIZE 4096 //单数据块大小，传输切片时使用
/*
发送给第三方的数据
*/
struct SendData {
    int iPkgSize; //文件总大小
    int slice;	//一共切了多少块
    int pkgSize; //当前片的大小
    int indexNumber; //当前是第几块
    char msg[PACKSIZE]; //内容
};

/*
给哪个服务器发送什么样的消息
*/
struct DataMessage
{
    int messageType;
    union
    {
        User userInfo;
        SendData messageData;
    }data;
};



#endif //CLOUDSERVER_CONFIG_H
