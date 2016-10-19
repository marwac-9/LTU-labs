#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateIdle : public State < Sim >
{
public:

	static StateIdle* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateIdle();
	~StateIdle();
	//copy
	StateIdle(const StateIdle&);
	//assign
	StateIdle& operator=(const StateIdle&);
};