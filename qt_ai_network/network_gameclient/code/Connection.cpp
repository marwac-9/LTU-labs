#define _CRT_SECURE_NO_WARNINGS
#include "networklabb4.h"
#include "Connection.h"
#include "ClientSocket.h"
#include "Protocol.h"
#include "PlayerInfo.h"

Connection::Connection(void* arg)
{
	app = (NetworkLabb4*)arg;
}

Connection::~Connection()
{
}

void Connection::whileChatting()
{
	while (true)
	{
		unsigned char recvbuf[1024];
		int iResult = recv(socket->ConnectSocket, (char*)recvbuf, 1024, 0);
		if (iResult > 0) {
			printf("\nWhile chatting - Bytes received: %d\n", iResult);
			executeMessage(recvbuf, iResult);
		}
		else if (iResult == 0) {
			printf("\nWhile chatting - Connection closed\n");
			return;
		}
		else {
			printf("\nWhile chatting - recv failed with error: %d\n", WSAGetLastError());
			return;
		}
		
	}
}

MsgHead* Connection::processMessage(unsigned char* message, int size, int index)
{
	MsgHead* msgHead = new MsgHead;
	if (app->crypt == true)
	{
		//decrypt RC4
		//Buffer to store the decrypted data
		unsigned char decryptedmessage[1024];
		crypt(message, decryptedmessage, size, index);
		msgHead = (MsgHead*)&decryptedmessage[index];
	}
	else
	{
		msgHead = (MsgHead*)&message[index];
	}
	return msgHead;
}

void Connection::executeMessage(unsigned char* message, int size)
{
	int startIndexOfMsg = 0;
	MsgHead* msgHead;
	
	while (startIndexOfMsg <= size - 1 && startIndexOfMsg > -1)
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

		if (msgHead->type == Change)
		{
			ChangeMsg* changeMsg = (ChangeMsg*)msgHead;
			if (changeMsg->type == NewPlayerPosition)
			{
				NewPlayerPositionMsg* newPlayerPosMsg = (NewPlayerPositionMsg*)msgHead;
				showMessage("received new player position message");
				app->newPlayerPos(newPlayerPosMsg->msg.head.id, newPlayerPosMsg->pos.x, newPlayerPosMsg->pos.y, newPlayerPosMsg->dir.x, newPlayerPosMsg->dir.y);
			}
			else if (changeMsg->type == NewPlayer)
			{
				showMessage("received new player message");
				NewPlayerMsg* newPlayerMsg = (NewPlayerMsg*)msgHead;
				cout << "New player id: " << newPlayerMsg->msg.head.id << endl;
				//check if player already exists
				if (app->playerAlreadyExists(newPlayerMsg->msg.head.id))
				{
					cout << "player already exists!" << endl;
				}
				else
				{
					PlayerInfo* playerInfo = new PlayerInfo();

					playerInfo->id = newPlayerMsg->msg.head.id;
					playerInfo->desc = newPlayerMsg->desc;
					playerInfo->form = newPlayerMsg->form;
					strcpy(playerInfo->name, newPlayerMsg->name);

					showMessage("adding new player");
					app->addPlayer(playerInfo);
				}
			}
			else if (changeMsg->type == PlayerLeave)
			{
				PlayerLeaveMsg* playerLeaveMsg = (PlayerLeaveMsg*)message;
				showMessage("received player leave message");
				app->removePlayer(playerLeaveMsg->msg.head.id);
			}
		}
		else if (msgHead->type == TextMessage)
		{
			TextMessageMsg* textMessageMsg = (TextMessageMsg*)message;
			showMessage("received text message");
			//for chat
		}
		else if (msgHead->type == Join)
		{
			if (app->id == NULL)
			{
				showMessage("received join message");
				JoinMsg* joinMsg = (JoinMsg*)msgHead;
				//join has no relevant content
				app->id = joinMsg->head.id;
				//app->addPlayer(playerInfo);
			}
		}
	}
}

void Connection::showMessage(string message)
{
	cout << message << endl;
}

int Connection::Send(char* message, int size)
{
	//send message
	int iResult = send(socket->ConnectSocket, message, size, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket->ConnectSocket);
		WSACleanup();
	}
	return iResult;
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

int Connection::Recv(char* message, int size)
{
	int iResult = recv(socket->ConnectSocket, message, size, 0);
	return iResult;
}

int Connection::reqJoin(QString name)
{
	//Creates a JoinMsg
	JoinMsg joinMsg;

	joinMsg.head.length = sizeof(JoinMsg);
	joinMsg.head.seq_no = 1;
	joinMsg.head.id = 0;
	joinMsg.head.type = Join;

	joinMsg.desc = Human;
	joinMsg.form = Sphere;

	QByteArray ba = name.toLocal8Bit();
	const char *c_str2 = ba.data();
	strcpy(joinMsg.name, c_str2);
	cout << "\nJoin.name: " << joinMsg.name << endl;

	//Buffer to store the data
	unsigned char buffer[sizeof(JoinMsg)];
	
	//Add the message to buffer
	memcpy(&buffer, &joinMsg, sizeof(JoinMsg));
	
	return cryptAndSend(buffer, sizeof(JoinMsg));
}

void Connection::reqMove(float posX, float posY, float dirX, float dirY)
{
	showMessage("\nrequesting move");
	MoveEvent moveEventMsg;

	moveEventMsg.event.head.id = app->id;
	moveEventMsg.event.head.length = sizeof(MoveEvent);
	moveEventMsg.event.head.seq_no = 1;
	moveEventMsg.event.head.type = Event;

	moveEventMsg.event.type = Move;


	moveEventMsg.pos.x = posX;
	moveEventMsg.pos.y = posY;
	moveEventMsg.dir.x = dirX;
	moveEventMsg.dir.y = dirY;

	//Buffer to store the data
	unsigned char buffer[sizeof(MoveEvent)];
	
	//Add the message to buffer
	memcpy(&buffer, &moveEventMsg, sizeof(MoveEvent));

	cryptAndSend(buffer, sizeof(MoveEvent));
}

void Connection::reqLeave()
{
	LeaveMsg leaveMsg;
	//Stores the client's ID
	leaveMsg.head.id = app->id;
	leaveMsg.head.length = sizeof(LeaveMsg);
	leaveMsg.head.seq_no = 1;
	leaveMsg.head.type = Leave;

	//Buffer to store the data
	unsigned char buffer[sizeof(LeaveMsg)];

	//Add the message to buffer
	memcpy(&buffer, &leaveMsg, sizeof(LeaveMsg));

	cryptAndSend(buffer, sizeof(LeaveMsg));
}

int Connection::cryptAndSend(unsigned char* buffer, int size)
{
	if (app->crypt == true)
	{
		unsigned char cryptBuffer[1024];
		crypt(buffer, cryptBuffer, size, 0);
		//Send the buffer with encrypted data
		int result = Send((char*)cryptBuffer, size);
		return result;
	}
	else
	{
		//Send the buffer with encrypted data
		int result = Send((char*)buffer, size);
		return result;
	}
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

void Connection::run(void* arg)
{
	//in this thread we will listen to messages
	NetworkLabb4* app = (NetworkLabb4*)arg;
	app->con->socket = app->socket;
	app->con->showMessage("\nStarting connection thread");
	if (app->con->reqJoin(app->clientName) > 0)
	{
		app->con->whileChatting();
	}
	app->con->showMessage("\nClosing connection thread");
}
