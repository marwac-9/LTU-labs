#pragma once
#include "State.h"
class Sim;
struct Telegram;

class StateVisitBathroom : public State<Sim>
{
public:

	static StateVisitBathroom* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateVisitBathroom();
	~StateVisitBathroom();
	//copy
	StateVisitBathroom(const StateVisitBathroom&);
	//assign
	StateVisitBathroom& operator=(const StateVisitBathroom&);

};