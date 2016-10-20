#include <string>
#include "Protocol.h"
using namespace std;
class ServerInfo;
class Server;
struct PlayerInfo;


class Connection
{
public:
	Connection(void* info);
	~Connection();
	ServerInfo* info;
	PlayerInfo* player;
	Server* app;

	void showMessage(string message);

	void whileChatting();
	bool executeMessage(unsigned char* message, int size);
	

	int Send(char* message, int size);
	int Send(int connection, char* message, int size);

	int cryptAndSend(int connection, unsigned char* buffer, int size);
	void cryptAndSendToAll(unsigned char* buffer, int size);

	void Recv(char* message, int size);
	int recJoin();

	void sendNewPlayerToAll(PlayerInfo* player);
	void sendNewPlayerPosToAll(PlayerInfo* player);
	void sendPlayerLeaveToAll(int id);
	void sendAllConnectedPlayersAndCurrentPosToPlayer();

	bool isPlayerAbleToMove(PlayerInfo* player, NewPlayerPositionMsg* newPlayerPosMsg);
	bool isPlayerNotMovingTooFast(PlayerInfo* player, NewPlayerPositionMsg* newPlayerPosMsg);
	bool isPlayerInWorldBorders(NewPlayerPositionMsg* newPlayerPosMsg);
	bool isPlayerNotInsideAnotherPlayer(NewPlayerPositionMsg* newPlayerPosMsg);

	PlayerInfo* addPlayer(JoinMsg* joinMsg);
	void removePlayer(int id);

	PlayerInfo* getPlayer(int id);
	int getPlayerIndex(int id);
	Coordinate generatePlayerRandomPos();
private:
	void rc4(unsigned char* key, unsigned char* input, unsigned char* output, int inputSize, int inputIndex);
	void ksa(unsigned char* state, unsigned char* key);
	unsigned char* prng(unsigned char* state, int inputSize);
	void swapArrayElements(unsigned char* marray, int i, int j);
	void crypt(unsigned char* input, unsigned char* output, int inputSize, int inputIndex);
};

