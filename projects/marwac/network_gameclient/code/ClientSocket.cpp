#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ClientSocket.h"

ClientSocket::ClientSocket(char* hostname, int port)
{
	ConnectSocket = INVALID_SOCKET;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = inet_addr(hostname); // Use any address available to the system
	serverInfo.sin_port = htons(port);      // Set port number
	cout << inet_ntoa(serverInfo.sin_addr) << " adress" << endl;
	cout << "\n" << serverInfo.sin_port << " port" << endl;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
	}
	else
	{
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
}

ClientSocket::~ClientSocket()
{
}

int ClientSocket::Close()
{
	int iResult;
	// shutdown the connection 
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	return NULL;
}

int ClientSocket::Connect()
{
	int iResult = connect(ConnectSocket, (SOCKADDR*)&serverInfo, sizeof(serverInfo));
	if (iResult == SOCKET_ERROR) {
		printf("Socket error: %ld\n", WSAGetLastError());
	}
	else if (iResult == 0)
	{
		cout << "\nconnected" << endl;
	}
	else
	{
		cout << "\nunexpected behavior while connecting" << endl;
	}
	return iResult;
}

