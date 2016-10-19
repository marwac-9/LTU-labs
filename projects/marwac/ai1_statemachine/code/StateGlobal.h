#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateGlobal : public State<Sim>
{
public:

	static StateGlobal* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateGlobal();
	~StateGlobal();
	//copy
	StateGlobal(const StateGlobal&);
	//assign
	StateGlobal& operator=(const StateGlobal&);
};