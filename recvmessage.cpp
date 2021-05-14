//
// Created by lansnow on 2021/3/8.
//

/*
服务器端
*/



#include "recvmessage.h"
#include <iostream>
#include <map>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>


extern std::vector<User> userList;
extern std::map<int, std::vector<User>> rooms;

recvmessage::recvmessage(streamsock *sSock):sSock(sSock){

}

void recvmessage::recvData()
{

    DataMessage recvbuf;
    struct sockaddr_in sender;
    socklen_t dwSender = sizeof(sender);
    bool bFound = false;

    int indexNumber = 0;
    int roomId;

    std::map<int, std::vector<User>>::iterator it;

    struct sockaddr_in remote;
    struct sockaddr_in remoteDataTest;
    struct sockaddr_in remoteDatax;

    bool isMaster;

    for (;;)
    {

        int ret = recvfrom(sSock->_sSock, (char *)&recvbuf, sizeof(recvbuf), 0, (sockaddr *)&sender, &dwSender);

        if (ret <= 0) {
            std::cout << "get error" << std::endl;
            continue;
        }

        int messageType = recvbuf.messageType;
        switch (messageType)
        {
            case MSGLOGIN:

                roomId = recvbuf.data.userInfo.RoomId;
                it = rooms.find(roomId);
                User newUser;
                if (it != rooms.end()) {//存在房间的情况下

                    for (auto &c : it->second) {	//遍历用户信息
                        //同一个屋子的用户名只能有一个
                        if (strcmp(c.userName, recvbuf.data.userInfo.userName)==0) {//禁止用户二次登录
                            DataMessage returnData;
                            returnData.messageType = MSGLOGINFIAL;
                            sendto(sSock->_sSock, (char*)(&returnData), sizeof(returnData), 0, (const sockaddr*)&sender, sizeof(sender));
			    std::cout<<"has same User"<<std::endl;
                            break;
                        }
                    }

                    //User newUser;
                    memcpy(newUser.userName,recvbuf.data.userInfo.userName, 20);
                    //Linux config
                    newUser.userIp = ntohl(sender.sin_addr.s_addr);

                    newUser.userPort = ntohs(sender.sin_port);
                    newUser.isMaster = it->second.size()>0?false:true;
		    std::cout<<"add User and update Master"<<std::endl;

                    it->second.push_back(newUser);

		    for(auto &d:it->second){
			std::cout<<"Has User:"<<d.userName<< "    UserInfo is:"<<d.isMaster<<std::endl;
		    }

                    DataMessage updateUser;
                    updateUser.messageType = UPDATAUSER;
                    updateUser.data.userInfo = newUser;

                    for (auto &d : it->second) {
                        if (d.isMaster == true) {

                            //bzero(&remote, sizeof(remote));
                            remote.sin_family = AF_INET;
                            remote.sin_port = htons(d.userPort);
                            remote.sin_addr.s_addr = htonl(d.userIp);
                            std::cout << "find Master：" << inet_ntoa(remote.sin_addr) <<"|"<< htons(remote.sin_port) << std::endl;
                            sendto(sSock->_sSock, (char*)(&updateUser), sizeof(updateUser), 0, (const sockaddr*)&remote, sizeof(remote));
                        }
                    }


                }
                else {	//不存在房间的情况下



                    memcpy(newUser.userName, recvbuf.data.userInfo.userName, 20);

                    newUser.userIp = ntohl(sender.sin_addr.s_addr);
                    newUser.userPort = ntohs(sender.sin_port);
                    newUser.isMaster = true;

                    std::cout << "USER:"<< newUser.userName<<" Create room :"<< inet_ntoa(sender.sin_addr)  <<"||"<< ntohs(sender.sin_port) << std::endl;

                    userList.push_back(newUser);
                    rooms[roomId]=userList;

                }


                DataMessage returnData;
                returnData.messageType = MSGLOGINSUCESS;
                returnData.data.userInfo = newUser;
                sendto(sSock->_sSock,(char*)(&returnData),sizeof(returnData),0, (const sockaddr*)&sender, sizeof(sender));

                break;
            case MSGLOGOUT:
		//查询对应房间的用户，同时推送到主播的帐号。
//查询对应房间的用户，同时推送到主播的帐号。
			roomId = recvbuf.data.userInfo.RoomId;
			it = rooms.find(roomId);
			isMaster = recvbuf.data.userInfo.isMaster;

			std::cout << "User LogOut:" << recvbuf.data.userInfo.userName << " and User isMaster:" << isMaster << std::endl;

			//下线通知数据：
			DataMessage logoutData;
			logoutData.messageType = USERLOGOUT;
			logoutData.data.userInfo.RoomId = roomId;
			//memcpy_s(logoutData.data.userInfo.userName, 20, recvbuf.data.userInfo.userName, 20);
			memcpy(logoutData.data.userInfo.userName, recvbuf.data.userInfo.userName, 20);
			logoutData.data.userInfo.isMaster = isMaster;

			if (it != rooms.end()) {//存在房间的情况下

				//先下线当前用户
				for (int i = 0; i < it->second.size(); ++i) {
					if (strcmp(it->second[i].userName, recvbuf.data.userInfo.userName) == 0) {
						(it->second).erase(it->second.begin() + i);
					}
				}

				if (isMaster) {
					for (int i = 0; i < it->second.size(); ++i) {
						sockaddr_in remote;
						remote.sin_family = AF_INET;
						remote.sin_port = htons(it->second[i].userPort);
						remote.sin_addr.s_addr = htonl(it->second[i].userIp);
						sendto(sSock->_sSock, (char*)(&logoutData), sizeof(logoutData), 0, (const sockaddr*)&remote, sizeof(remote));
					}
				}
				else {
					for (int i = 0; i < it->second.size(); ++i) {
						if (it->second[i].isMaster) {
							sockaddr_in remote;
							remote.sin_family = AF_INET;
							remote.sin_port = htons(it->second[i].userPort);
							remote.sin_addr.s_addr = htonl(it->second[i].userIp);
							sendto(sSock->_sSock, (char*)(&logoutData), sizeof(logoutData), 0, (const sockaddr*)&remote, sizeof(remote));
						}
					}
				}

				std::cout << "remove User----" << std::endl;

				for (auto &d : it->second) {
					std::cout << "Has User:" << d.userName << "    UserInfo is:" << d.isMaster << std::endl;

				}
				std::cout << "remove User----" << std::endl;

			}			

std::cout << "user has logout" << std::endl;	

                break;
            case P2PYNC://登录成功后，定时发送数据到服务器，相当于心跳信息
                //std::cout << "heart ync" << std::endl;
                break;

            case P2PACK://第三方响应数据
                std::cout << "p2pack" << std::endl;
                break;

            case P2PRANS://需要穿墙了。。


                roomId = recvbuf.data.userInfo.RoomId;

                it = rooms.find(roomId);

                //std::cout << "need chuan qiang: from" << inet_ntoa(sender.sin_addr) << "|" << ntohs(sender.sin_port) << std::endl;
                //找到指定房间的相关用户后，再给用户发送特定的消息
                if (it != rooms.end()) {
                    //打印服务器里所有用户IP信息
                    if (indexNumber % 20 == 0) {
                        for (auto &c : it->second) {



                            remoteDataTest.sin_family = AF_INET;

                            remoteDataTest.sin_port = htons(c.userPort);
                            remoteDataTest.sin_addr.s_addr = htonl(c.userIp);
                            std::cout << c.userName << " Ipinfo is:" << inet_ntoa(remoteDataTest.sin_addr) << ":" << ntohs(remoteDataTest.sin_port) << std::endl;
                        }
                    }

                    for (auto &c : it->second) {
                        //要给指定的用户发送穿墙信息
                        if (strcmp(c.userName, recvbuf.data.userInfo.userName) == 0) {

                            //std::cout << "Want to call user:" << roomId<<"	"<< c.userName<<":"<< recvbuf.data.userInfo.userName << std::endl;

                            DataMessage transMessage;
                            transMessage.messageType = P2PSOMEONEWANTTOCALLYOU;
                            //sender.sin_addr.S_un.S_addr
                            transMessage.data.userInfo.userIp = htonl(sender.sin_addr.s_addr);
                            transMessage.data.userInfo.userPort = htons(sender.sin_port);
                            transMessage.data.userInfo.isLan = false;
                            if (htonl(sender.sin_addr.s_addr)== htonl(c.userIp)) {
                                transMessage.data.userInfo.isLan = true;
                            }


                            remoteDatax.sin_family = AF_INET;

                            remoteDatax.sin_port = htons(c.userPort);
                            remoteDatax.sin_addr.s_addr = htonl(c.userIp);
                            /*
                            感觉这些有些问题，具体未知
                            remoteData:发送给的对方
                            transMessage：是发起方向的信息
                            */
                            if (indexNumber % 30 == 0) {
                                std::cout << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port) << " want push Data to " << inet_ntoa(remoteDatax.sin_addr) << ":" << ntohs(remoteDatax.sin_port) << std::endl;
                            }
                            indexNumber++;


                            sendto(sSock->_sSock, (const char*)&transMessage, sizeof(transMessage), 0, (const sockaddr *)&remoteDatax, sizeof(remoteDatax));

                        }

                    }

                }

            default:
                break;
        }

    }
    //sSock->_sSock

}
