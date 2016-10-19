#include "Server.h"
#include "ServerSocket.h"
#include "PlayerInfo.h"
#include "ServerInfoStruct.h"
#include "Connection.h"
void* serverListener(void *ptr);

Server::Server()
{
	game.minX = -100;
	game.minY = -100;
	game.maxX = 100;
	game.maxY = 100;
	crypt = true;
}

Server::~Server()
{
}

void Server::run()
{
	ServerSocket server(49152);


	server.Listen(1);
	while (true)
	{
		//block at
		int socketConnection = server.Accept();

		ServerInfo* sinfo = new ServerInfo();
		sinfo->connection = socketConnection;
		sinfo->socketHandle = server.socketHandle;
		sinfo->app = this;

		// rc is the number of characters returned.
		// Note this is not typical. Typically one would only specify the number 
		// of bytes to read a fixed header which would include the number of bytes
		// to read. See "Tips and Best Practices" below.

		pthread_t threadHandle;
		int threadId = pthread_create(&threadHandle, NULL, serverListener, (void*)sinfo);
		if (threadId)
		{
			fprintf(stderr, "Error - pthread_create() return code: %d\n", threadId);
			exit(EXIT_FAILURE);
		}
	
		threads.push_back(threadId);
		threadHandles.push_back(threadHandle);
		connections.push_back(socketConnection);
	}
	server.Close();
}

void* serverListener(void *ptr)
{
	ServerInfo* sinfo = (ServerInfo*)ptr;
	Connection con(ptr);
	con.showMessage("\nStarting thread");
	if (con.recJoin() > 0)
	{
		con.whileChatting();
		con.showMessage("\nCommunication ended");
	}
	cout << "\nClearing connection: " << sinfo->connection << endl;
	for (int i = 0; i < sinfo->app->connections.size(); i++)
	{
		if (sinfo->app->connections.at(i) == sinfo->connection)
		{
			int pindex = con.getPlayerIndex(sinfo->connection);
			if (pindex != -1)
			{
				con.showMessage("\nFound player to remove");
				sinfo->app->players.erase(sinfo->app->players.begin() + pindex);
			}
			else
			{
				con.showMessage("\nPlayer was already removed");
			}
			con.showMessage("\nClosing connection");
			close(sinfo->connection);
			sinfo->app->connections.erase(sinfo->app->connections.begin() + i);
			sinfo->app->threadHandles.erase(sinfo->app->threadHandles.begin() + i);
			sinfo->app->threads.erase(sinfo->app->threads.begin() + i);
		}
	}
	cout << "\nPlayers: " << sinfo->app->players.size() << endl;
	cout << "\nConnections: " << sinfo->app->connections.size() << endl;
	cout << "\nThreadHandles: " << sinfo->app->threadHandles.size() << endl;
	cout << "\nThreads: " << sinfo->app->threads.size() << endl;
	con.showMessage("\nServer Listener - Closing thread");
}