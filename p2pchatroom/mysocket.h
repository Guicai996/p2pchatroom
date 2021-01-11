#pragma once
#include <iostream>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <vector>
#include <windows.h>
#include <random>

typedef unsigned int uint;
constexpr auto MAX_CLIENT = 10;

struct p2pClientInfo
{
	SOCKET sockets[MAX_CLIENT];
	sockaddr_in addrs_list[MAX_CLIENT];
	int addrs_len[MAX_CLIENT];
	uint clients_num;
	std::mutex mutex_num;
	std::mutex mutex_list;
	std::vector<std::thread*> threads;

	p2pClientInfo();
};

class mysocket
{
private:
	void Initwinsockenv();
	void CreateServerSocket();
protected:
	sockaddr_in serAddr;
	SOCKET server_socket;
	p2pClientInfo clients;

	char recvbuff[512];
	char sendbuff[512];
public:
	mysocket();
	~mysocket();
};

class p2pclient :
	public mysocket
{
private:
	void CreateLocalSocket();
	void BindPeerPort();
	void Connect_server(std::string& ip, int& port);
	void Getclientslist();
	void ShowUI();
	void Sendmsgto(int id, std::string& msg);
	void Updateclientslist();
	void RecvMSGthread();
	SOCKET local_socket;
	sockaddr_in peer;
	uint myid;
	std::mutex mutex_showmsg;

	/*1 代表ui界面， 2 你想给谁发消息: 3 调用recover_cout*/
	int current_cmdstatue;
	std::string recover_cout;
public:
	p2pclient(std::string& ip, int& port);
	~p2pclient();
};

class p2pserver :
	public mysocket
{
private:
	void Bind(uint port);
	void Listen();
	void WaitConnec();
	void Thread_ProcConnec(uint Client_No);

public:
	p2pserver(uint port);
	
	~p2pserver();


};