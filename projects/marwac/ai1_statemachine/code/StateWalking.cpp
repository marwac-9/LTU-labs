#include "StateWalking.h"
#include "StateEating.h"
#include "StateWorking.h"
#include "Sim.h"
#include "EntityManager.h"
#include <iostream>
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "StateMachine.h"
#include "Timer.h"
#include "Storage.h"
StateWalking::StateWalking()
{
}

StateWalking::~StateWalking()
{
}

StateWalking* StateWalking::Instance()
{
	static StateWalking instance;

	return &instance;
}

void StateWalking::Enter(Sim* sim)
{
	if (!sim->IsInAreaOf(sim->GetDestination()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "walking to: " << sim->GetDestination()->name; // << " at: " << sim->GetDestination()->pos.x << " " << sim->GetDestination()->pos.y;
		sim->ChangeLocationName("On the way to: " + sim->GetDestination()->name);
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "at the: " << sim->GetDestination()->name;
	}
}

void StateWalking::Execute(Sim* sim)
{
	Location* simDestination = sim->GetDestination();
	Location* simLocation = sim->GetLocation();
	//if (simLocation->compare(simDestination)) // for exact position
	if (sim->IsInAreaOf(simDestination))
	{
		sim->ChangeLocationName(simDestination->name);
		sim->GetFSM()->RevertToPreviousState();
	}
	else
	{
		sim->AdvanceToDestination();
	}
}

void StateWalking::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << " exiting walk state";
}

bool StateWalking::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}