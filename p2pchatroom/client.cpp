#include <iostream>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include "mysocket.h"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main(int argc, char* argv[])
{
	string authorized_ip;
	int authorized_port = 8888;
	int mode;

pos_switch:
	cout << "\n1. 本地127.0.0.1测试模式。(禁用网卡ip检测)\n";
	cout << "2. 输入局域网（未NAT转换）服务器地址，在线测试。\n\n";
	cout << "\n请输入相应数字选择模式并回车：";
	cin >> mode;

	switch (mode) 
	{
	case 1:
		authorized_ip = "127.0.0.1";
		break;
	case 2:
		cout << "[authorized server] 请输入服务器IP: ";
		cin >> authorized_ip;
		break;
	default:
		cout << "输入有误！" << endl;
		goto pos_switch;
	}
	system("cls");
	p2pclient chatroom(authorized_ip, authorized_port, mode);

	return 0;
}


