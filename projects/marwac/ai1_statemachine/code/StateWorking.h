#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateWorking : public State<Sim>
{
public:

	static StateWorking* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateWorking();
	~StateWorking();
	//copy
	StateWorking(const StateWorking&);
	//assign
	StateWorking& operator=(const StateWorking&);
};

