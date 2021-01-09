#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

typedef unsigned int uint;
constexpr auto MAX_CLIENT = 10;

struct ip_port
{
	char ip[15];
	uint16_t port;
};

class mysocket
{
private:
	void Initwinsockenv();
	void CreateServerSocket();
protected:
	sockaddr_in serAddr;
	SOCKET server_socket;
	SOCKET p2pclient_sockets[MAX_CLIENT];
	ip_port client_listp[MAX_CLIENT];
	uint clients_num;
	char recvbuff[512];
	char sendbuff[512];
public:
	mysocket();
};

class p2pclient :
	public mysocket
{
private:
	void Getclientslist();
	void Connect_server(std::string& ip, int& port);
public:
	p2pclient(std::string& ip, int& port);
};

class p2pserver :
	public mysocket
{
private:
	void Bind(uint port);
	//void process_accept();
	std::thread p1();

public:
	p2pserver(uint port);
	
	//~p2pserver();


};