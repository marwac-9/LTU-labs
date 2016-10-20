
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using namespace std;

class ClientSocket
{
public:
	ClientSocket(char* hostname, int port);
	~ClientSocket();
	int Close();
	int Connect();
	WSADATA wsaData;
	SOCKET ConnectSocket;
	SOCKADDR_IN serverInfo;

private:

};

