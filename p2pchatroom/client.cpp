#include <iostream>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include "mysocket.h"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

mutex mutex1;

void process_connection(p2pclient chatroom);

int main(int argc, char* argv[])
{
	string authorized_ip;
	int authorized_port;
	cout << "[authorized server] 请输入服务器IP和Port，以空格分开: ";
	cin >> authorized_ip >> authorized_port;
	p2pclient chatroom(authorized_ip, authorized_port);

	thread p1(process_connection, chatroom);
	thread* p2 = new thread(process_connection);

	return 0;
}


