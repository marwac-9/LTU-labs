#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateDead : public State < Sim >
{
public:

	static StateDead* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateDead();
	~StateDead();
	//copy
	StateDead(const StateDead&);
	//assign
	StateDead& operator=(const StateDead&);
};