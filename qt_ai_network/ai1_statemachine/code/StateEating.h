#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateEating : public State<Sim>
{
public:

	static StateEating* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateEating();
	~StateEating();
	//copy
	StateEating(const StateEating&);
	//assign
	StateEating& operator=(const StateEating&);
};