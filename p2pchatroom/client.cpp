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
	cout << "\n1. ����127.0.0.1����ģʽ��(��������ip���)\n";
	cout << "2. �����������δNATת������������ַ�����߲��ԡ�\n\n";
	cout << "\n��������Ӧ����ѡ��ģʽ���س���";
	cin >> mode;

	switch (mode) 
	{
	case 1:
		authorized_ip = "127.0.0.1";
		break;
	case 2:
		cout << "[authorized server] �����������IP: ";
		cin >> authorized_ip;
		break;
	default:
		cout << "��������" << endl;
		goto pos_switch;
	}
	system("cls");
	p2pclient chatroom(authorized_ip, authorized_port, mode);

	return 0;
}


