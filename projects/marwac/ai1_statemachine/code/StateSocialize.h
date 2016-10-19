#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateSocialize : public State < Sim >
{
public:

	static StateSocialize* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateSocialize();
	~StateSocialize();
	//copy
	StateSocialize(const StateSocialize&);
	//assign
	StateSocialize& operator=(const StateSocialize&);
};