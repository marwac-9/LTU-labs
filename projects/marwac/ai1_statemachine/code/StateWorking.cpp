#include "StateWalking.h"
#include "StateWorking.h"
#include "StateSleeping.h"
#include "StateIdle.h"
#include "StateDrinking.h"
#include "StateEating.h"
#include "Sim.h"
#include "EntityManager.h"
#include <iostream>
#include "StateMachine.h"
#include "Storage.h"
StateWorking::StateWorking()
{
}

StateWorking::~StateWorking()
{
}

StateWorking* StateWorking::Instance()
{
	static StateWorking instance;

	return &instance;
}

void StateWorking::Enter(Sim* sim)
{
	if (!sim->IsInAreaOf(sim->GetJobLocation()))
	{
		sim->ChangeDestination(sim->GetJobLocation());
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Walkin' to work";
		sim->GetFSM()->ChangeState(StateWalking::Instance());
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Getting back to work";
	}
}


void StateWorking::Execute(Sim* sim)
{	


	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Working my ass off!";
	if (sim->IsTimeToStopWork())
	{ 
		sim->GetPaid();
		sim->GetFSM()->ChangeState(StateIdle::Instance());
	}
}


void StateWorking::Exit(Sim* sim)
{
	if (sim->IsInAreaOf(sim->GetJobLocation()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
		<< "stop working";
	}
}

bool StateWorking::OnMessage(Sim* sim, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}