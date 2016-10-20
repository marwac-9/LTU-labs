#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <vector>
#include <pthread.h>
#define MAXHOSTNAME 256
using namespace std;

class ServerSocket
{
public:
	ServerSocket(int port);
	~ServerSocket();
	struct sockaddr_in socketInfo;
	int socketHandle;
	int Accept();
	void Close(); 
	void Listen(int maxQueNum = SOMAXCONN);
private:
	
};

