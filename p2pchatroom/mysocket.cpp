#include "mysocket.h"


mysocket::mysocket() :clients_num(0)
{
	this->Initwinsockenv();
	this->CreateServerSocket();
}

void mysocket::Initwinsockenv()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		std::cout << "[ERRO] ��ʼ��winsock����ʧ�ܣ�" << std::endl;
		exit(1);
	}
}

void mysocket::CreateServerSocket()
{
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "[ERRO] �����׽���ʧ�ܣ�" << std::endl;
		exit(1);
	}
}

void p2pclient::Getclientslist()
{
	recv(server_socket, recvbuff, sizeof(recvbuff), 0);
	memcpy(&clients_num, recvbuff, sizeof(uint));
	if (clients_num == 0) {
		std::cout << "���ǵ�һ�����������ҵĳ�Ա����ȱ������ĵȴ�������Ա����~" << std::endl;
		return;
	}

}

p2pclient::p2pclient(std::string& ip, int& port)
{
	this->Connect_server(ip, port);
}


void p2pclient::Connect_server(std::string& ip, int& port)
{
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (connect(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] ���������������ʧ�ܣ�" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
	else
		std::cout << "[Client] ��������������ӳɹ���" << std::endl;
}

p2pserver::p2pserver(uint port)
{
	this->Bind(port);
}

void p2pserver::Bind(uint port)
{
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] �󶨶˿ں�ʧ�ܣ�" << std::endl;
		exit(1);
	}
}
