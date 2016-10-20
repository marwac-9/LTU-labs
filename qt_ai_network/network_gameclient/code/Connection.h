#include <string>
#include <QString>
class ClientSocket;
class NetworkLabb4;
struct MsgHead;
class Connection
{
public:
	Connection(void* arg);
	~Connection();
	static void run(void* arg);
	void whileChatting();
	void executeMessage(unsigned char* message, int size);
	void showMessage(std::string message);
	ClientSocket* socket;
	NetworkLabb4* app;
	int Send(char* message, int size);
	int Recv(char* message, int size);
	int reqJoin(QString name);
	void reqLeave();
	void reqMove(float posX, float posY, float dirX, float dirY);
	int cryptAndSend(unsigned char* buffer, int size);
	MsgHead* processMessage(unsigned char* message, int size, int index = 0);
private:
	void rc4(unsigned char* key, unsigned char* input, unsigned char* output, int inputSize, int inputIndex);
	void ksa(unsigned char* state, unsigned char* key);
	unsigned char* prng(unsigned char* state, int inputSize);
	void swapArrayElements(unsigned char* marray, int i, int j);
	void crypt(unsigned char* input, unsigned char* output, int inputSize, int inputIndex);
};

