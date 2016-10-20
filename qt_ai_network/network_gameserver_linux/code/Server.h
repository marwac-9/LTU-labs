#include <pthread.h>
#include <vector>
#include "GameInfo.h"

using namespace std;
struct PlayerInfo;
class Server
{
public:
	Server();
	~Server();
	vector<int> threads;
	vector<int> connections;
	vector<pthread_t> threadHandles;
	vector<PlayerInfo*> players;
	GameInfo game;

	void run();
	bool crypt;
private:

};

