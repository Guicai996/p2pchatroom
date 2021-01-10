#include "mysocket.h"


mysocket::mysocket()
{
	this->Initwinsockenv();
	this->CreateServerSocket();
}

mysocket::~mysocket()
{
	closesocket(server_socket);
}

void mysocket::Initwinsockenv()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		std::cout << "[ERRO] 初始化winsock环境失败！" << std::endl;
		exit(1);
	}
}

void mysocket::CreateServerSocket()
{
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "[ERRO] 创建TCP套接字失败！" << std::endl;
		exit(1);
	}
}

void p2pclient::Getclientslist()
{
	send(server_socket, (char*)&peer.sin_port, sizeof(peer.sin_port), 0);
	recv(server_socket, (char*)&clients.clients_num, sizeof(uint), 0);
	recv(server_socket, (char*)clients.addrs_list, sizeof(clients.addrs_list), 0);
	while (true) {
		Sleep(1000);
		recv(server_socket, (char*)&clients.clients_num, sizeof(uint), 0);
		recv(server_socket, (char*)clients.addrs_list, sizeof(clients.addrs_list), 0);
	}
}

void p2pclient::CreateLocalSocket()
{
	local_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "[ERRO] 创建UDP套接字失败！" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

void p2pclient::BindPeerPort()
{
	int on = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
		std::cout << "[ERRO] 端口复用选项错误！" << std::endl;
		closesocket(server_socket);
		exit(1);
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons(40000 + rand() % 10000);
	peer.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(local_socket, (sockaddr*)&peer, sizeof(peer)) == -1) {
		std::cout << "[ERRO] 绑定端口号失败！" << std::endl;
		closesocket(server_socket);
		closesocket(local_socket);
		exit(1);
	}
}

p2pclient::p2pclient(std::string& ip, int& port)
{
	this->CreateLocalSocket();
	this->BindPeerPort();
	this->Connect_server(ip, port);
	this->Getclientslist();
}

p2pclient::~p2pclient()
{
	closesocket(local_socket);
}


void p2pclient::Connect_server(std::string& ip, int& port)
{
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (connect(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] 与服务器建立连接失败！" << std::endl;
		closesocket(server_socket);
		closesocket(local_socket);
		exit(1);
	}
	else
		std::cout << "[Client] 与服务器建立连接成功！" << std::endl;
}

/*--------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------*/


void p2pserver::WaitConnec()
{
	std::cout << "waiting for clients..." << std::endl;
	while (true) {
		clients.sockets[clients.clients_num] = accept(server_socket, (sockaddr*)(clients.addrs_list + clients.clients_num), clients.addrs_len + clients.clients_num);
		if (clients.sockets[clients.clients_num] != SOCKET_ERROR) {
			/*坑位，改端口号是否会影响TCP的链接*/
			recv(server_socket, (char*)&clients.addrs_list[clients.clients_num].sin_port, sizeof(USHORT), 0);
			clients.mutex_num.lock();
			std::thread* p1 = new std::thread(&Thread_ProcConnec, this, clients.clients_num++);
			clients.mutex_num.unlock();
			clients.threads.push_back(p1);
		}
	}
}

void p2pserver::Thread_ProcConnec(uint clients_No)
{
	SYSTEMTIME localtime;
	GetLocalTime(&localtime);
	char msg[100];
	sprintf(msg, "Client%d login %d-%d-%d,ip:%s port:%d", clients_No, 
		localtime.wYear, localtime.wMonth, localtime.wDay, 
		inet_ntoa(clients.addrs_list[clients_No].sin_addr), 
		clients.addrs_list[clients_No].sin_port);
	std::cout << msg << std::endl;
	clients.mutex_num.lock();
	send(clients.sockets[clients_No], (char*)&clients_No, sizeof(uint), 0);
	send(clients.sockets[clients_No], (char*)&clients.addrs_list, sizeof(clients.addrs_list), 0);
	clients.mutex_num.unlock();
}

p2pserver::p2pserver(uint port)
{
	this->Bind(port);
	this->Listen();
	this->WaitConnec();


}

p2pserver::~p2pserver()
{
	for (auto i : clients.threads) {
		i->join();
		delete i;
	}
}

void p2pserver::Bind(uint port)
{

	int on = 1; 
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
		std::cout << "[ERRO] 端口复用选项错误！" << std::endl;
		closesocket(server_socket);
		exit(1);
	}

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] 绑定端口号失败！" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

void p2pserver::Listen()
{
	if (listen(server_socket, 10) == -1) {
		std::cout << "[ERRO] 监听端口失败！" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

p2pClientInfo::p2pClientInfo() : clients_num(0)
{
	for (int& i : addrs_len)
		i = sizeof(sockaddr_in);

}
