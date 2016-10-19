#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateDrinking : public State<Sim>
{
public:

	static StateDrinking* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateDrinking();
	~StateDrinking();
	//copy
	StateDrinking(const StateDrinking&);
	//assign
	StateDrinking& operator=(const StateDrinking&);
};