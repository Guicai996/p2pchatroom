#include <iostream>
#include <thread>
#include <winsock2.h>
#include "mysocket.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main(int argc, char* argv[])
{
	p2pserver chatserver(8888);


	return 0;
}