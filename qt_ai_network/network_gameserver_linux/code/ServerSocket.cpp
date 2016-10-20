#include "ServerSocket.h"

ServerSocket::ServerSocket(int port)
{
	char sysHost[MAXHOSTNAME + 1];  // Hostname of this computer we are running on
	struct hostent *hPtr;

	cout << "\nclearing structure sockaddr_in named socketInfo" << endl;
	bzero(&socketInfo, sizeof(sockaddr_in));  // Clear structure memory

	// Get system information
	cout << "\ngetting system info" << endl;
	gethostname(sysHost, MAXHOSTNAME);  // Get the name of this computer we are running on
	if ((hPtr = gethostbyname(sysHost)) == NULL)
	{
		cerr << "System hostname misconfigured." << endl;
		exit(EXIT_FAILURE);
	}
	cout << "\n" << sysHost << " host name" << endl;

	// create socket
	cout << "\ncreating socket and getting handle" << endl;
	if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		close(socketHandle);
		exit(EXIT_FAILURE);
	}
	cout << "\n" << socketHandle << " socket handle" << endl;

	cout << "\nload info into socket structure socketInfo" << endl;
	// Load system information into socket data structures

	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = htonl(INADDR_ANY); // Use any address available to the system
	socketInfo.sin_port = htons(port);      // Set port number
	cout << inet_ntoa(socketInfo.sin_addr) << " adress"<< endl;
	cout << "\n" << socketInfo.sin_port << " port" << endl;

	// Bind the socket to a local socket address
	cout << "\nbinding socket to the local socket address" << endl;
	if (bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(socketInfo)) < 0)
	{
		close(socketHandle);
		perror("bind");
		exit(EXIT_FAILURE);
	}

	int flag = 1;
	if (setsockopt(socketHandle, IPPROTO_TCP, TCP_NODELAY, (const  char *)&flag, sizeof(int)) == -1)
	{
		cerr << "Set TCP_NODELAY option failed >>Server<<" << endl;
	}
	int one = 1;
	setsockopt(socketHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one));
}

ServerSocket::~ServerSocket()
{
}

int ServerSocket::Accept()
{
	cout << "\nblock at accept" << endl;
	int socketConnection;
	if ((socketConnection = accept(socketHandle, NULL, NULL)) < 0)
	{
		exit(EXIT_FAILURE);
	}
	cout << "\nconnection established " << socketConnection << endl;
	return socketConnection;
	//close(socketHandle);
}

void ServerSocket::Close()
{
	cout << "\nclosing server socket" << endl;
	close(socketHandle);
}

void ServerSocket::Listen(int maxQueNum)
{
	cout << "\ninitializing listening to the connections, max queue 1" << endl;
	listen(socketHandle, maxQueNum);
}