#include "StateDrinking.h"
#include "StateEating.h"
#include "StateIdle.h"
#include "StateMachine.h"
#include "Sim.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Timer.h"
#include "EntityManager.h"
#include <iostream>
#include "Storage.h"

StateDrinking::StateDrinking()
{
}

StateDrinking::~StateDrinking()
{
}

StateDrinking* StateDrinking::Instance()
{
	static StateDrinking instance;

	return &instance;
}

void StateDrinking::Enter(Sim* sim)
{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Boy, ah sure is thusty! Taking a drink";
}

void StateDrinking::Execute(Sim* sim)
{
	if (sim->BuyAndDrinkAWhiskey())
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "That's mighty fine sippin' liquer";
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "I can't afford a drink";
	}

	sim->GetFSM()->ChangeState(StateIdle::Instance());
}

void StateDrinking::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Done drinking, feelin' good";
}

bool StateDrinking::OnMessage(Sim* sim, const Telegram& msg)
{

	return false; //send message to global message handler
}