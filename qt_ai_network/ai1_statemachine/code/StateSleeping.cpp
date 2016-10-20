#include "StateWalking.h"
#include "StateSleeping.h"
#include "StateEating.h"
#include "StateIdle.h"
#include "Sim.h"
#include "EntityManager.h"
#include <iostream>
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "StateMachine.h"
#include "Timer.h"
#include "Storage.h"
StateSleeping::StateSleeping()
{
}

StateSleeping::~StateSleeping()
{
}

StateSleeping* StateSleeping::Instance()
{
	static StateSleeping instance;

	return &instance;
}

void StateSleeping::Enter(Sim* sim)
{
	if (!sim->IsInAreaOf(sim->GetHomeLocation()))
	{
		sim->ChangeDestination(sim->GetHomeLocation());
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "going home to bed";
		sim->GetFSM()->ChangeState(StateWalking::Instance());
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "going to bed";
	}
}

void StateSleeping::Execute(Sim* sim)
{
	
	if (sim->CanWakeUp())
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
			<< "What a God darn fantastic sleep! I'm exploding with energy";

		sim->GetFSM()->ChangeState(StateIdle::Instance());
	}
	else
	{
		//sleep
		sim->DecreaseFatigue();
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "ZZZZ... ";
	}
	
}

void StateSleeping::Exit(Sim* sim)
{
	if (sim->IsInAreaOf(sim->GetHomeLocation())) 
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "waking up";
	}
}

bool StateSleeping::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}