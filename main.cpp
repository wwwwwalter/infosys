#include <iostream>
#include "config.h"
#include "streamsock.h"
#include "recvmessage.h"
#include <thread>
#include <vector>
#include <map>
std::vector<User> userList;
std::map<int, std::vector<User>> rooms;

int main() {

    /*
		1：创建一个空置的用户列表
			创建一个消息队列，上行的用户较多，下行可以使用线程来处理
		2：创建SOCKET，监听本地固定端口(SEND)

		3：创建一个获取数据的线程
			1）：获取不同的数据类型，同作出不同的反馈

	*/

    //userList;

    std::vector<User*> userList;

    streamsock *sSock = new streamsock();
    bool creactS = sSock->init();
    if (!creactS) {
        std::cout << "Create Sock error" << std::endl;
        return 0;
    }
    sSock->userList = userList;

    recvmessage *recv = new recvmessage(sSock);
    //recv->sSock = sSock;
    std::thread t2(&recvmessage::recvData,recv);
    t2.join();




    return 0;
}
