#include "Protocol.h"
struct PlayerInfo
{
	int id;
	char name[MAXNAMELEN];
	ObjectDesc desc;
	ObjectForm form;
	Coordinate pos;
};