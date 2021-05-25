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



void printMessage(DataMessage &recvbuf){
        std::cout << "********recvbuf***********"<<std::endl;
        std::cout << "messageType:\t"<<recvbuf.messageType<<std::endl;
        std::cout << "userName:\t"<<recvbuf.data.userInfo.userName<<std::endl;
	sockaddr_in temp;
	temp.sin_addr.s_addr = htonl(recvbuf.data.userInfo.userIp);
        std::cout << "userIp:\t\t"<<inet_ntoa(temp.sin_addr)<<std::endl;
        std::cout << "userPort:\t"<<recvbuf.data.userInfo.userPort<<std::endl;
        std::cout << "isMaster:\t"<<recvbuf.data.userInfo.isMaster<<std::endl;
        std::cout << "roomId:\t\t"<<recvbuf.data.userInfo.RoomId << std::endl;
        std::cout << "channelId:\t"<<recvbuf.data.userInfo.channelId << std::endl;
        std::cout << "isLan:\t\t"<<recvbuf.data.userInfo.isLan<<std::endl;
        std::cout << "********recvbuf end*******"<<std::endl;

}

void printSock(sockaddr_in &sockin){
	std::cout << "******sockaddr_in*********" << std::endl;
	std::cout << "sockinIp:\t" << inet_ntoa(sockin.sin_addr) << std::endl;
	std::cout << "sockinPort:\t" << ntohs(sockin.sin_port) << std::endl;
	std::cout << "***sockaddr_in end *******" << std::endl;
}

void printUserList(int roomId, std::vector<User> &userlist){
	printf("\nroomid\tuserName\tmaster\tuserip\t\tuserport\n");
	for(auto &d:userlist){
		sockaddr_in temp;
		temp.sin_addr.s_addr = htonl(d.userIp);
		printf("%d\t%s\t%d\t%s\t%d\n",roomId,d.userName,d.isMaster,inet_ntoa(temp.sin_addr),d.userPort);
	}
	printf("\n");
}

void copyDataMessage(DataMessage &outbuf, DataMessage &recvbuf){
	memcpy(outbuf.data.userInfo.userName, recvbuf.data.userInfo.userName, 20);
       	outbuf.data.userInfo.userIp = recvbuf.data.userInfo.userIp;
        outbuf.data.userInfo.userPort = recvbuf.data.userInfo.userPort;
	outbuf.data.userInfo.isMaster = recvbuf.data.userInfo.isMaster;
	outbuf.data.userInfo.RoomId = recvbuf.data.userInfo.RoomId;
        outbuf.data.userInfo.channelId = recvbuf.data.userInfo.channelId;
        outbuf.data.userInfo.isLan = recvbuf.data.userInfo.isLan;
}

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
    bool isMaster;

    std::map<int, std::vector<User>>::iterator it;

    struct sockaddr_in remote;
    struct sockaddr_in remoteDataTest;
    struct sockaddr_in remoteDatax;

    std::map<int,std::vector<User>> rooms;


    for (;;)
    {
        std::vector<User> userList;

        int ret = recvfrom(sSock->_sSock, (char *)&recvbuf, sizeof(recvbuf), 0, (sockaddr *)&sender, &dwSender);

        if (ret <= 0) {
            std::cout << "get error" << std::endl;
            continue;
        }

	if(recvbuf.messageType!=P2PYNC){
	    printSock(sender);
	    printMessage(recvbuf);
	}



    	roomId = recvbuf.data.userInfo.RoomId;
    	it = rooms.find(roomId);
    	isMaster = recvbuf.data.userInfo.isMaster;
	

        int messageType = recvbuf.messageType;
        switch (messageType)
        {
            case MSGLOGIN:
                User newUser;
                if (it != rooms.end()) {//存在房间的情况下
			for(auto &c:it->second){
			    if(strcmp(c.userName,recvbuf.data.userInfo.userName)==0){
                            DataMessage returnData;
                            returnData.messageType = MSGLOGINFIAL;
                            sendto(sSock->_sSock, (char*)(&returnData), sizeof(returnData), 0, (const sockaddr*)&sender, sizeof(sender));
			    std::cout<<"user "<< newUser.userName << " socket " << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port) << " repeat login room "<< roomId <<std::endl;
                            goto end;
                        }
                    }

                    memcpy(newUser.userName,recvbuf.data.userInfo.userName, 20);
                    newUser.userIp = ntohl(sender.sin_addr.s_addr);
                    newUser.userPort = ntohs(sender.sin_port);
                    newUser.isMaster = it->second.size()>0?false:true;
		    newUser.RoomId = recvbuf.data.userInfo.RoomId;
                    newUser.channelId = recvbuf.data.userInfo.channelId;
		    std::cout<<"add User and update Master"<<std::endl;

                    it->second.push_back(newUser);
			
                    
		    DataMessage updateUser;
                    updateUser.messageType = UPDATAUSER;
                    updateUser.data.userInfo = newUser;
			
			printMessage(updateUser);

		    //send new user to master
                    for (auto &d : it->second) {
                        if (d.isMaster == true) {
                            //bzero(&remote, sizeof(remote));
                            remote.sin_family = AF_INET;
                            remote.sin_port = htons(d.userPort);
                            remote.sin_addr.s_addr = htonl(d.userIp);
                            sendto(sSock->_sSock, (char*)(&updateUser), sizeof(updateUser), 0, (const sockaddr*)&remote, sizeof(remote));
                            std::cout << "send newuser " << newUser.userName << " socket " << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port) << " to master " \
                                      <<inet_ntoa(remote.sin_addr)<<":"<<htons(remote.sin_port)<<std::endl;
                            break;
                        }
                    }
                }
                else {	//不存在房间的情况下
		    if(recvbuf.data.userInfo.isMaster == true){
                    	memcpy(newUser.userName, recvbuf.data.userInfo.userName, 20);

                    	newUser.userIp = ntohl(sender.sin_addr.s_addr);
                    	newUser.userPort = ntohs(sender.sin_port);
                    	newUser.isMaster = true;

                    	std::cout << "USER "<< newUser.userName<<" socket "<< inet_ntoa(sender.sin_addr) <<":"<< ntohs(sender.sin_port) << " create room " << roomId << std::endl;

                    	userList.push_back(newUser);
                    	rooms[roomId]=userList;
		    	it = rooms.find(roomId);
		    }
		    else{
			std::cout << "guest are not allowed to create rooms" << std::endl;
			DataMessage returnData;
			returnData.messageType=MSGLOGINFIAL;
			sendto(sSock->_sSock,(char*)(&returnData),sizeof(returnData),0,(const sockaddr*)&sender,sizeof(sender));
			goto end;
		    }

                }
                

		//return log success to self
                DataMessage returnData;
                returnData.messageType = MSGLOGINSUCESS;
		copyDataMessage(returnData,recvbuf);
		returnData.data.userInfo = newUser;
                sendto(sSock->_sSock,(char*)(&returnData),sizeof(returnData),0, (const sockaddr*)&sender, sizeof(sender));
                std::cout << "USER "<< newUser.userName<<" socket " << inet_ntoa(sender.sin_addr) << ":" << ntohs(sender.sin_port) << " login room " << roomId << std::endl;
		
		printUserList(roomId, it->second);
		
end:
                break;
            case MSGLOGOUT:
		DataMessage logoutData;
		logoutData.messageType = USERLOGOUT;
		copyDataMessage(logoutData,recvbuf);

		if (it != rooms.end()) {//存在房间的情况下

			if (isMaster) {
                                //current master send to all clients include master
				for (uint i = 0; i < it->second.size(); ++i) {
					sockaddr_in remote;
					remote.sin_family = AF_INET;
					remote.sin_port = htons(it->second[i].userPort);
					remote.sin_addr.s_addr = htonl(it->second[i].userIp);
					sendto(sSock->_sSock, (char*)(&logoutData), sizeof(logoutData), 0, (const sockaddr*)&remote, sizeof(remote));
                                        std::cout<<"sendto client "<<it->second[i].userName << std::endl;
				}
				//delete the room
				{	
					std::vector<User>().swap(it->second);//empty userlist
					rooms.erase(it);
					std::cout<< "kill the room: "<< roomId << std::endl;				
				}

			}
			else {
                                //current client to master
				for (uint i = 0; i < it->second.size(); ++i) {
					if (it->second[i].isMaster) {
						sockaddr_in remote;
						remote.sin_family = AF_INET;
						remote.sin_port = htons(it->second[i].userPort);
						remote.sin_addr.s_addr = htonl(it->second[i].userIp);
						sendto(sSock->_sSock, (char*)(&logoutData), sizeof(logoutData), 0, (const sockaddr*)&remote, sizeof(remote));
                                                std::cout<<"sendto master "<<it->second[i].userName << std::endl;
                                                break;
					}
			    	}
                                //current client to self
                                {
                                	sendto(sSock->_sSock,(char*)(&logoutData),sizeof(logoutData),0,(const sockaddr*)&sender,sizeof(sender));
                                }

				//delete logout user from userlist
				for (uint i = 0; i < it->second.size(); ++i) {
					if (strcmp(it->second[i].userName, recvbuf.data.userInfo.userName) == 0) {
						(it->second).erase(it->second.begin() + i);
                                        	std::cout<< "erase "<<it->second[i].userName << std::endl;
					}
				}
                        	std::cout<< "username " <<logoutData.data.userInfo.userName<<" has logout"<<std::endl;
				
				printUserList(roomId,it->second);
                }

		
                break;
            case P2PYNC://登录成功后，定时发送数据到服务器，相当于心跳信息
                //std::cout << "heart ync" << std::endl;
                break;

            case P2PACK:
                std::cout << "p2pack" << std::endl;
                break;

            case P2PRANS:
                roomId = recvbuf.data.userInfo.RoomId;
                it = rooms.find(roomId);

                std::cout << "need chuan qiang: from" << inet_ntoa(sender.sin_addr) << "|" << ntohs(sender.sin_port) << std::endl;
                //找到指定房间的相关用户后，再给用户发送特定的消息
                if (it != rooms.end()) {

                    for (auto &c : it->second) {
                        //要给指定的用户发送穿墙信息
                        if (strcmp(c.userName, recvbuf.data.userInfo.userName) == 0) {

                            std::cout << "Want to call user:" << recvbuf.data.userInfo.userName << std::endl;

                            DataMessage transMessage;
                            transMessage.messageType = P2PSOMEONEWANTTOCALLYOU;
                            transMessage.data.userInfo.userIp = ntohl(sender.sin_addr.s_addr);
                            transMessage.data.userInfo.userPort = ntohs(sender.sin_port);
                            transMessage.data.userInfo.isLan = false;
                            if (htonl(sender.sin_addr.s_addr)== htonl(c.userIp)) {
                                transMessage.data.userInfo.isLan = true;
                            }


                            remoteDatax.sin_family = AF_INET;
                            remoteDatax.sin_port = htons(c.userPort);
                            remoteDatax.sin_addr.s_addr = htonl(c.userIp);

                            sendto(sSock->_sSock, (const char*)&transMessage, sizeof(transMessage), 0, (const sockaddr *)&remoteDatax, sizeof(remoteDatax));

                        }

                    }

               }
            }
		break; 

            default:
                break;
        }

    }
    //sSock->_sSock

}
//
// Created by lansnow on 2021/3/8.
//
