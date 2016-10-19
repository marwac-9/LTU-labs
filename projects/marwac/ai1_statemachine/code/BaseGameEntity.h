#pragma once
#include <string>

struct Telegram;

class BaseGameEntity
{
public:
	BaseGameEntity();
	BaseGameEntity(int id);
	virtual ~BaseGameEntity();
	virtual void Update() = 0;
	virtual bool HandleMessage(const Telegram& msg) = 0;
	
	//every entity has a unique identifying number
	int id;
	//this is the next valid ID. Each time a BaseGameEntity is instantiated this value is updated
	static int nextId;

	std::string name;
	void SetId(int val);

	int GetId() const;

	

private:

};

