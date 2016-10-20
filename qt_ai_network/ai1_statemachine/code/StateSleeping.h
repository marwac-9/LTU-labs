#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateSleeping : public State<Sim>
{
public:
	
	static StateSleeping* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateSleeping();
	~StateSleeping();
	//copy
	StateSleeping(const StateSleeping&);
	//assign
	StateSleeping& operator=(const StateSleeping&);
};

