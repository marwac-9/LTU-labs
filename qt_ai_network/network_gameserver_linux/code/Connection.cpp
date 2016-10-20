#include "Connection.h"
#include "ServerInfoStruct.h"
#include "ServerSocket.h"
#include "Server.h"
#include "PlayerInfo.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
Connection::Connection(void* info)
{
	this->info = (ServerInfo*)info;
	this->app = this->info->app;
	srand(time(NULL));
}

Connection::~Connection()
{
	delete info;
}

void Connection::showMessage(string message)
{
	char messageBuf[512];
	string messageToChar = message;
	strcpy(messageBuf, messageToChar.c_str());
	cout << messageBuf << endl;
}

int Connection::recJoin()
{
	int rc = 0;  // Actual number of bytes read
	unsigned char message[1024];
	rc = recv(info->connection, (char*)message, 1024, 0);
	if (rc <= 0)
	{
		showMessage("well client aborted connection(maybe disconnected) but nothing to worry about\nbecause connection will be closed by server and it's thread\nplayer will be removed as well when thread closes in a sec");
	}
	else
	{
		message[rc] = (char)NULL;        // Null terminate string
		cout << "Number of bytes read: " << rc << endl;
		
		MsgHead* msgHead;

		//decrypt RC4
		if (app->crypt == true)
		{
			//decrypt RC4
			//Buffer to store the decrypted data
			unsigned char decryptedmessage[1024];
			crypt(message, decryptedmessage, rc, 0);
			msgHead = (MsgHead*)&decryptedmessage[0];
		}
		else
		{
			msgHead = (MsgHead*)&message[0];
		}

		cout << "\nMessage type: " << msgHead->type << endl;
		if (msgHead->type == Join)
		{
			showMessage("Received JoinMsg");
			JoinMsg* oldJoinMsg = (JoinMsg*)msgHead;

			JoinMsg joinMsg;

			joinMsg.head.length = sizeof(JoinMsg);
			joinMsg.head.seq_no = 1;
			joinMsg.head.id = this->info->connection;
			joinMsg.head.type = Join;

			joinMsg.desc = oldJoinMsg->desc;
			joinMsg.form = oldJoinMsg->form;
			strcpy(joinMsg.name, oldJoinMsg->name);
			cout << "\nPlayer Name: " << oldJoinMsg->name << endl;

			//Buffer to store the data
			unsigned char buffer[sizeof(JoinMsg)];

			//Add the message to buffer
			memcpy(&buffer, &joinMsg, sizeof(JoinMsg));
			showMessage("\nSending join to the player");
			int result = cryptAndSend(this->info->connection, buffer, sizeof(JoinMsg));
			cout << "\nSend join result: " << result << endl;
			if (result > 0)
			{
				this->player = addPlayer(&joinMsg);
				sendNewPlayerToAll(player);
				sendAllConnectedPlayersAndCurrentPosToPlayer();
				sendNewPlayerPosToAll(player);
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	return rc;
}

PlayerInfo* Connection::addPlayer(JoinMsg* joinMsg)
{
	cout << "\nAdding player: " << joinMsg->name << endl;
	PlayerInfo* player = new PlayerInfo();
	player->id = joinMsg->head.id;
	player->desc = joinMsg->desc;
	player->form = joinMsg->form;
	player->pos = generatePlayerRandomPos();
	strcpy(player->name, joinMsg->name);
	app->players.push_back(player);
	return player;
}

void Connection::sendAllConnectedPlayersAndCurrentPosToPlayer()
{
	for (int i = 0; i < app->players.size(); i++)
	{
		if (app->players.at(i)->id != this->info->connection)
		{
			NewPlayerMsg newPlayerMsg;
			newPlayerMsg.msg.head.id = app->players.at(i)->id;
			newPlayerMsg.msg.head.length = sizeof(NewPlayerMsg);
			newPlayerMsg.msg.head.seq_no = 1;
			newPlayerMsg.msg.head.type = Change;

			newPlayerMsg.msg.type = NewPlayer;

			newPlayerMsg.desc = app->players.at(i)->desc;
			newPlayerMsg.form = app->players.at(i)->form;
			strcpy(newPlayerMsg.name, app->players.at(i)->name);

			//Buffer to store the data
			unsigned char buffer[sizeof(NewPlayerMsg)];

			//Add the message to buffer
			memcpy(&buffer, &newPlayerMsg, sizeof(NewPlayerMsg));
			cout << "\nSending new player: " << newPlayerMsg.name << " " << app->players.at(i)->id << endl;
			cryptAndSend(this->info->connection, buffer, sizeof(NewPlayerMsg));

			NewPlayerPositionMsg newPlayerPosMsg;
			newPlayerPosMsg.msg.head.id = app->players.at(i)->id;
			newPlayerPosMsg.msg.head.length = sizeof(NewPlayerPositionMsg);
			newPlayerPosMsg.msg.head.seq_no = 1;
			newPlayerPosMsg.msg.head.type = Change;

			newPlayerPosMsg.msg.type = NewPlayerPosition;

			newPlayerPosMsg.pos.x = app->players.at(i)->pos.x;
			newPlayerPosMsg.pos.y = app->players.at(i)->pos.y;

			//Buffer to store the data
			buffer[sizeof(NewPlayerPositionMsg)];

			//Add the message to buffer
			memcpy(&buffer, &newPlayerPosMsg, sizeof(NewPlayerPositionMsg));
			cout << "\nSending new player POS: " << newPlayerMsg.name << " " << app->players.at(i)->id << endl;
			cryptAndSend(this->info->connection, buffer, sizeof(NewPlayerPositionMsg));
		}
	}
}

void Connection::sendNewPlayerToAll(PlayerInfo* player)
{
	cout << "\nSending new player to all: " << player->name << endl;
	NewPlayerMsg newPlayerMsg;
	newPlayerMsg.msg.head.id = player->id;
	newPlayerMsg.msg.head.length = sizeof(NewPlayerMsg);
	newPlayerMsg.msg.head.seq_no = 1;
	newPlayerMsg.msg.head.type = Change;

	newPlayerMsg.msg.type = NewPlayer;

	newPlayerMsg.desc = player->desc;
	newPlayerMsg.form = player->form;
	strcpy(newPlayerMsg.name, player->name);

	//Buffer to store the data
	unsigned char buffer[sizeof(NewPlayerMsg)];

	//Add the message to buffer
	memcpy(&buffer, &newPlayerMsg, sizeof(NewPlayerMsg));

	cryptAndSendToAll(buffer, sizeof(NewPlayerMsg));
}

void Connection::sendNewPlayerPosToAll(PlayerInfo* player)
{
	cout << "\nSending new player pos to all: " << player->pos.x << " " << player->pos.y << endl;
	NewPlayerPositionMsg newPlayerPosMsg;
	newPlayerPosMsg.msg.head.id = player->id;
	newPlayerPosMsg.msg.head.length = sizeof(NewPlayerPositionMsg);
	newPlayerPosMsg.msg.head.seq_no = 1;
	newPlayerPosMsg.msg.head.type = Change;

	newPlayerPosMsg.msg.type = NewPlayerPosition;

	newPlayerPosMsg.pos.x = player->pos.x;
	newPlayerPosMsg.pos.y = player->pos.y;

	//Buffer to store the data
	unsigned char buffer[sizeof(NewPlayerPositionMsg)];

	//Add the message to buffer
	memcpy(&buffer, &newPlayerPosMsg, sizeof(NewPlayerPositionMsg));

	cryptAndSendToAll(buffer, sizeof(NewPlayerPositionMsg));

}

void Connection::sendPlayerLeaveToAll(int pid)
{
	cout << "\nSending player leave to all (pid): " << pid << endl;
	PlayerLeaveMsg playerLeaveMsg;
	playerLeaveMsg.msg.head.length = sizeof(PlayerLeaveMsg);
	playerLeaveMsg.msg.head.seq_no = 1;
	playerLeaveMsg.msg.head.id = pid;
	playerLeaveMsg.msg.head.type = Change;

	playerLeaveMsg.msg.type = PlayerLeave;

	//Buffer to store the data
	unsigned char buffer[sizeof(PlayerLeaveMsg)];

	//Add the message to buffer
	memcpy(&buffer, &playerLeaveMsg, sizeof(PlayerLeaveMsg));

	cryptAndSendToAll(buffer, sizeof(PlayerLeaveMsg));
}

int Connection::cryptAndSend(int connection, unsigned char* buffer, int size)
{
	if (app->crypt == true)
	{
		cout << "\ncrypting and sending" << endl;
		unsigned char cryptBuffer[1024];
		crypt(buffer, cryptBuffer, size, 0);
		//Send the buffer with encrypted data
		int result = Send(connection, (char*)cryptBuffer, size);
		return result;
	}
	else
	{
		cout << "\nsending size: " << size << endl;
		//Send the buffer with encrypted data
		int result = Send(connection, (char*)buffer, size);
		return result;
	}
}

void Connection::cryptAndSendToAll(unsigned char* buffer, int size)
{
	for (int i = 0; i < app->connections.size(); i++)
	{
		cryptAndSend(app->connections.at(i), buffer, size);
	}
}

void Connection::whileChatting()
{
	showMessage("\nNow in while chatting");
	while (info->connection)
	{
		unsigned char recvbuf[1024];
		int iResult = recv(info->connection, (char*)recvbuf, 1024, 0);
		if (iResult <= 0)
		{
			showMessage("\nwell client aborted connection(maybe disconnected) but nothing to worry about\n because connection will be closed by server and it's thread\n player will be removed as well at finally when thread closes in a sec");
			return;
		}
		else
		{
			//recvbuf[iResult] = (char)NULL;        // Null terminate string
			cout << "Number of bytes read: " << iResult << endl;
			if (!executeMessage(recvbuf, iResult))
			{
				showMessage("\nClient left or chatting aborted");
				return;
			}
		}
	}
}

bool Connection::executeMessage(unsigned char* message, int size)
{
	int startIndexOfMsg = 0;
	MsgHead* msgHead;
	while (startIndexOfMsg <= size - 1)
	{
		cout << "\nReading message at index: " << startIndexOfMsg << endl;
		if (app->crypt == true)
		{
			//decrypt RC4
			//Buffer to store the decrypted data
			unsigned char decryptedmessage[1024];
			crypt(message, decryptedmessage, size, startIndexOfMsg);
			msgHead = (MsgHead*)&decryptedmessage[0];
		}
		else
		{
			msgHead = (MsgHead*)&message[startIndexOfMsg];
		}
		startIndexOfMsg = startIndexOfMsg + msgHead->length;

		if (msgHead->type == Event)
		{
			EventMsg* moveMsg = (EventMsg*)msgHead;
			if (moveMsg->type == Move)
			{
				NewPlayerPositionMsg* newPlayerPosMsg = (NewPlayerPositionMsg*)msgHead;
				showMessage("received new player position message");
				PlayerInfo*  player = getPlayer(newPlayerPosMsg->msg.head.id);
				if (player != NULL)
				{
					if (isPlayerAbleToMove(player, newPlayerPosMsg))
					{
						player->pos = newPlayerPosMsg->pos;
						sendNewPlayerPosToAll(player);
					}
				}
			}
		}
		else if (msgHead->type == Leave)
		{
			LeaveMsg* leaveMsg = (LeaveMsg*)msgHead;
			showMessage("received player leave message");
			//remove player from list
			removePlayer(leaveMsg->head.id);
			sendPlayerLeaveToAll(leaveMsg->head.id);
			return false;
		}
		else if (msgHead->type == TextMessage)
		{
			TextMessageMsg* textMessageMsg = (TextMessageMsg*)msgHead;
			showMessage("received text message");
			//for chat
		}
	}
	return true;
}

bool Connection::isPlayerAbleToMove(PlayerInfo* player, NewPlayerPositionMsg* newPlayerPosMsg)
{
	if (isPlayerNotMovingTooFast(player, newPlayerPosMsg))
	{
		if (isPlayerInWorldBorders(newPlayerPosMsg))
		{
			if (isPlayerNotInsideAnotherPlayer(newPlayerPosMsg))
			{
				return true;
			}
		}
	}
	return false;
}

bool Connection::isPlayerNotMovingTooFast(PlayerInfo* player, NewPlayerPositionMsg* newPlayerPosMsg)
{
	if (newPlayerPosMsg->pos.x - player->pos.x > 1 || newPlayerPosMsg->pos.y - player->pos.y > 1)
	{
		showMessage("Player moves too fast, too large step distance");
		return false;
	}
	return true;
}

bool Connection::isPlayerInWorldBorders(NewPlayerPositionMsg* newPlayerPosMsg)
{
	//if player is in x range of the game world
	if (newPlayerPosMsg->pos.x >= app->game.minX && newPlayerPosMsg->pos.x <= app->game.maxX)
	{
		//if player is in y range of the world
		if (newPlayerPosMsg->pos.y >= app->game.minY && newPlayerPosMsg->pos.y <= app->game.maxY)
			return true;
	}
	showMessage("Player at world border");
	return false;
}

bool Connection::isPlayerNotInsideAnotherPlayer(NewPlayerPositionMsg* newPlayerPosMsg)
{
	int psize = app->players.size();
	for (int i = 0; i < psize; i++)
	{
		if (i != newPlayerPosMsg->msg.head.id)
		{
			//if new pos x of investigated player is the same as the position x of another player
			if (newPlayerPosMsg->pos.x == app->players.at(i)->pos.x)
			{
				//if new pos y of investigated player is the same as the position y of another player
				if (newPlayerPosMsg->pos.y == app->players.at(i)->pos.y)
				{
					showMessage("Position occupied by another player");
					return false;
				}
			}
		}
	}
	return true;
}

void Connection::removePlayer(int pid)
{
	int index = getPlayerIndex(pid);
	if (index != -1)
	{
		app->players.erase(app->players.begin() + index);
	}
}

PlayerInfo* Connection::getPlayer(int pid)
{
	int psize = app->players.size();
	for (int i = 0; i < psize; i++)
	{
		if (app->players.at(i)->id == pid)
		{
			return app->players.at(i);
		}
	}
	showMessage("\nPlayer not found");
	return NULL;
}

int Connection::getPlayerIndex(int pid)
{
	int psize = app->players.size();
	for (int i = 0; i < psize; i++)
	{
		if (app->players.at(i)->id == pid)
		{
			return i;
		}
	}
	showMessage("\nPlayer not found");
	return -1;
}

Coordinate Connection::generatePlayerRandomPos()
{
	int psize = app->players.size();
	Coordinate randPos;
	randPos.y = 0;
	//generates number between -100 and 100
	randPos.x = (rand() % 200 - 100);
	randPos.y = (rand() % 200 - 100);
	if (psize > 1)
	{
		while (true)
		{
			for (int i = 0; i < psize; i++)
			{
				if (randPos.x == app->players.at(i)->pos.x)
				{
					randPos.x = (rand() % 200 - 100);
					break;
				}
				else if (app->players.at(i)->pos.y == randPos.y)
				{
					randPos.y = (rand() % 200 - 100);
					break;
				}
				else
				{
					return randPos;
				}
			}
		}
	}
	return randPos;
}

int Connection::Send(char* message, int size)
{
	int sendTotal = 0;
	int loopCount = 0;
	cout << "\nSize of msg to send: " << size << endl;
	while (sendTotal < size)
	{
		//Sends the data, returns the amount that has been sent
		int bytesSend = send(info->connection, message, size, 0);
		loopCount++;
		cout << "\nByte sent: " << bytesSend << endl;
		cout << "\nLoop count" << loopCount << endl;
		//Updates the variable with the amount sent
		sendTotal = sendTotal + bytesSend;
	}
	return sendTotal;
}

int Connection::Send(int connection, char* message, int size)
{
	int sendTotal = 0;
	int loopCount = 0;
	cout << "\nSize of msg to send: " << size << endl;
	while (sendTotal < size)
	{
		//Sends the data, returns the amount that has been sent
		int bytesSend = send(connection, message, size, 0);
		loopCount++;
		cout << "\nByte sent: " << bytesSend << endl;
		cout << "\nLoop count" << loopCount << endl;
		//Updates the variable with the amount sent
		sendTotal = sendTotal + bytesSend;
	}
	return sendTotal;
}


void Connection::Recv(char* message, int size)
{

}

void Connection::crypt(unsigned char* input, unsigned char* output, int inputSize, int inputIndex)
{
	//Key array
	unsigned char key[256];
	//Copy the key to the array
	strncpy((char*)key, RC4Key, 256);
	//Encrypting data
	rc4(key, input, output, inputSize, inputIndex);
}

void Connection::rc4(unsigned char* key, unsigned char* input, unsigned char* output, int inputSize, int inputIndex)
{
	//State array used to generate the keystream
	unsigned char state[256];
	//Pointer to the keystream array
	unsigned char* keystream;

	int j = 0;
	//Init the state array and scramble its data
	ksa(state, key);
	//Generate the keystream and return it
	keystream = prng(state, inputSize);

	//XOR the input data with the keystream data and put it in the output array
	for (int i = inputIndex; i<inputSize; i++){
		output[j] = input[i] ^ keystream[j];
		j++;
	}
	delete keystream;
}

void Connection::ksa(unsigned char* state, unsigned char* key)
{
	//Init state array
	for (int i = 0; i<256; i++){
		state[i] = i;
	}

	//Stores the amount of elements the key array has
	int keylength;
	int j = 0;
	//Get the amount of element of the key, null-terminated
	keylength = strlen((char*)key);

	//Scrambles the state array based on the provided key
	for (int i = 0; i<256; i++){
		j = (j + state[i] + key[i%keylength]) % 256;
		swapArrayElements(state, i, j);
	}
}

unsigned char* Connection::prng(unsigned char* state, int inputSize)
{
	int i = 0;
	int j = 0;
	//Creates a keystream array with the size of the array to be encrypted
	unsigned char* keystream = new unsigned char[inputSize];

	//Generates the keystream. Every loop generates one byte of the keystream
	//For every loop, swap elements in the state to make it harder to crack the security
	for (int k = 0; k<inputSize; k++){
		i = (i + 1) % 256;
		j = (j + state[i]) % 256;

		swapArrayElements(state, i, j);

		keystream[k] = state[(state[i] + state[j]) % 256];
	}
	return keystream;
}

void Connection::swapArrayElements(unsigned char* marray, int i, int j)
{
	//Temp variable
	unsigned char tempElement;

	//Temporary stores the element
	tempElement = marray[i];
	marray[i] = marray[j];
	marray[j] = tempElement;
}