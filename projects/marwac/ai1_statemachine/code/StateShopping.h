#pragma once 
#include "State.h"
class Sim;
struct Telegram;

class StateShopping : public State<Sim>
{
public:

	static StateShopping* Instance();

	virtual void Enter(Sim* sim);

	virtual void Execute(Sim* sim);

	virtual void Exit(Sim* sim);

	virtual bool OnMessage(Sim* sim, const Telegram& msg);

private:
	StateShopping();
	~StateShopping();
	//copy
	StateShopping(const StateShopping&);
	//assign
	StateShopping& operator=(const StateShopping&);
};