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
		std::cout << "[ERRO] ��ʼ��winsock����ʧ�ܣ�" << std::endl;
		exit(1);
	}
}

void mysocket::CreateServerSocket()
{
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "[ERRO] ����TCP�׽���ʧ�ܣ�" << std::endl;
		exit(1);
	}
}

/*---------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------*/

void p2pclient::Getclientslist()
{
	send(server_socket, (char*)&peer.sin_port, sizeof(peer.sin_port), 0);
	clients.mutex_list.lock();
	recv(server_socket, (char*)&clients.clients_num, sizeof(uint), 0);
	recv(server_socket, (char*)clients.addrs_list, sizeof(clients.addrs_list), 0);
	clients.mutex_list.unlock();
	for (int i = 0; i < clients.clients_num; ++i)
	{
		if (clients.addrs_list[i].sin_addr.S_un.S_addr == peer.sin_addr.S_un.S_addr &&
			clients.addrs_list[i].sin_port == peer.sin_port) {
			myid = i;
			break;
		}
	}
	myid = clients.clients_num;
}

void p2pclient::Updateclientslist()
{
	while (true) {
		clients.mutex_list.lock();
		recv(server_socket, (char*)&clients.clients_num, sizeof(uint), 0);
		recv(server_socket, (char*)clients.addrs_list, sizeof(clients.addrs_list), 0);
		clients.mutex_list.unlock();
	}
}

void p2pclient::ShowUI()
{
	int num;
	int sendid;
	std::string msg;
	std::ostringstream temp;
	mutex_showmsg.lock();
	std::cout << "�𾴵�client." << myid << " ����ӭʹ�ã�\n\n";
	std::cout << "1.��ȡ��ǰ�Եȶˣ�\n";
	std::cout << "2.ָ��һ���Եȶ˷�����Ϣ��\n";
	std::cout << "3.�˳���\n\n";
	std::cout << "������һ�����ֲ����س�ѡ���ܣ�";
	current_cmdstatue = 1;
	mutex_showmsg.unlock();

	while(true){
		std::cin >> num;
		switch (num)
		{
		case 1:
			mutex_showmsg.lock();
			for (int i = 0; i < clients.clients_num; ++i)
				std::cout << "client." << i << ' ';
			std::cout << std::endl;
			std::cout << "������һ�����ֲ����س�ѡ���ܣ�";
			current_cmdstatue = 1;
			mutex_showmsg.unlock();
			break;
		case 2:
			mutex_showmsg.lock();
			std::cout << "�����˭����Ϣ: ";
			current_cmdstatue = 2;
			mutex_showmsg.unlock();
			std::cin >> sendid;
			mutex_showmsg.lock();
			std::cout << "[To client." << sendid << "]: ";

			temp << "[To client." << sendid << "]: ";
			recover_cout = temp.str();
			current_cmdstatue = 3;

			mutex_showmsg.unlock();
			std::cin >> msg;
			Sendmsgto(sendid, msg);
			mutex_showmsg.lock();
			std::cout << "������һ�����ֲ����س�ѡ���ܣ�";
			current_cmdstatue = 1;
			mutex_showmsg.unlock();
			break;
		case 3:
			exit(0);
		}
	}
}

void p2pclient::RecvMSGthread()
{
	char MSGbuff[500];
	sockaddr_in MSGfrom;
	int fromID = 999;
	int MSGfromsize = sizeof(MSGfrom);
	while (true) {
		if (recvfrom(local_socket, MSGbuff, sizeof(MSGbuff), 0, (sockaddr*)&MSGfrom, &MSGfromsize)) {
			for (int i = 0; i < clients.clients_num; ++i) {
				if (clients.addrs_list[i].sin_addr.S_un.S_addr == peer.sin_addr.S_un.S_addr &&
					clients.addrs_list[i].sin_port == peer.sin_port) {
					fromID = i;
					break;
				}
			}
			mutex_showmsg.lock();
			std::cout << "\r                                           \r" << MSGbuff << std::endl;
			switch (current_cmdstatue)
			{
			case 1:
				std::cout << "������һ�����ֲ����س�ѡ���ܣ�";
				break;
			case 2:
				std::cout << "�����˭����Ϣ: ";
				break;
			case 3:
				std::cout << recover_cout;
				break;
			default:
				std::cout << '\n';
			}
			mutex_showmsg.unlock();
		}
	}
}

void p2pclient::Sendmsgto(int id, std::string& msg) 
{
	sendto(this->local_socket, msg.c_str(), msg.length(), 0, (sockaddr*)&clients.addrs_list[id], sizeof(sockaddr_in));
}

void p2pclient::CreateLocalSocket()
{
	local_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "[ERRO] ����UDP�׽���ʧ�ܣ�" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

void p2pclient::BindPeerPort()
{
	int on = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
		std::cout << "[ERRO] �˿ڸ���ѡ�����" << std::endl;
		closesocket(server_socket);
		exit(1);
	}

	peer.sin_family = AF_INET;
	srand((unsigned)time(NULL));
	peer.sin_port = htons(40000 + (rand() % 10000));
	this->GetIP();

	if (bind(local_socket, (sockaddr*)&peer, sizeof(peer)) == -1) {
		std::cout << "[ERRO] �󶨶˿ں�ʧ�ܣ�" << std::endl;
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

	auto t = new std::thread(&p2pclient::Updateclientslist, this);
	clients.threads.push_back(t);

	t = new std::thread(&p2pclient::RecvMSGthread, this);
	clients.threads.push_back(t);
	//this->Updateclientslist();

	this->ShowUI();

}

p2pclient::~p2pclient()
{
	closesocket(local_socket);
	for (auto i : clients.threads) {
		i->join();
		delete i;
	}
}


void p2pclient::Connect_server(std::string& ip, int& port)
{
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (connect(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] ���������������ʧ�ܣ�" << std::endl;
		closesocket(server_socket);
		closesocket(local_socket);
		exit(1);
	}
	else
		std::cout << "[Client] ��������������ӳɹ���" << std::endl;
}

void p2pclient::GetIP()
{
	PHOSTENT hostinfo;
	char hostname[255] = { 0 };
	gethostname(hostname, sizeof(hostname));
	if (hostinfo = gethostbyname(hostname)) {
		std::cout << "�ɹ���ñ���IP��ַ: " << inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list) << std::endl;
		peer.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;
	}
	else {
		peer.sin_addr.S_un.S_addr = INADDR_ANY;
		std::cout << "δ�ɹ����IP��ַ��ϵͳ���Դ���999�ڱ�����ʾ��ı�š�" << std::endl;;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------*/


void p2pserver::WaitConnec()
{
	std::cout << "waiting for clients..." << std::endl;
	while (true) {
		clients.sockets[clients.clients_num] = accept(server_socket, (sockaddr*)(clients.addrs_list + clients.clients_num), clients.addrs_len + clients.clients_num);

		if (clients.sockets[clients.clients_num] != SOCKET_ERROR) {
			/*��λ���Ķ˿ں��Ƿ��Ӱ��TCP������*/
			recv(clients.sockets[clients.clients_num], (char*)&clients.addrs_list[clients.clients_num].sin_port, sizeof(USHORT), 0);
			clients.mutex_num.lock();
			std::thread* p1 = new std::thread(&p2pserver::Thread_ProcConnec, this, clients.clients_num);
			clients.clients_num++;
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

	for (int i = 0; i <= clients_No; ++i) {
		clients.mutex_list.lock();
		send(clients.sockets[i], (char*)&clients.clients_num, sizeof(uint), 0);
		send(clients.sockets[i], (char*)&clients.addrs_list, sizeof(clients.addrs_list), 0);
		clients.mutex_list.unlock();
	}

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
		std::cout << "[ERRO] �˿ڸ���ѡ�����" << std::endl;
		closesocket(server_socket);
		exit(1);
	}

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (bind(server_socket, (sockaddr*)&serAddr, sizeof(serAddr)) == -1) {
		std::cout << "[ERRO] �󶨶˿ں�ʧ�ܣ�" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

void p2pserver::Listen()
{
	if (listen(server_socket, 10) == -1) {
		std::cout << "[ERRO] �����˿�ʧ�ܣ�" << std::endl;
		closesocket(server_socket);
		exit(1);
	}
}

p2pClientInfo::p2pClientInfo() : clients_num(0)
{
	for (int& i : addrs_len)
		i = sizeof(sockaddr_in);

}
