#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateWalking : public State < Sim >
{
public:

	static StateWalking* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateWalking();
	~StateWalking();
	//copy
	StateWalking(const StateWalking&);
	//assign
	StateWalking& operator=(const StateWalking&);
};