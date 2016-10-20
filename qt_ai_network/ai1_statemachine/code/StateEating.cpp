#include "StateEating.h"
#include "StateWorking.h"
#include "StateMachine.h"
#include "StateIdle.h"
#include "Sim.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Timer.h"
#include "EntityManager.h"
#include <iostream>
#include "Storage.h"

StateEating::StateEating()
{
}

StateEating::~StateEating()
{
}

StateEating* StateEating::Instance()
{
	static StateEating instance;

	return &instance;
}

void StateEating::Enter(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Boy, ah sure is hungry!";
}

void StateEating::Execute(Sim* sim)
{
	if (sim->BuyAndEatABurger())
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "That's mighty fine burger maximus";
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "I can't afford a meal";
	}
	sim->GetFSM()->ChangeState(StateIdle::Instance());
}

void StateEating::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Done eating, feelin' good";
}

bool StateEating::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}