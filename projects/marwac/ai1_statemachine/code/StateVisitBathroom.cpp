#include "StateVisitBathroom.h"
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

StateVisitBathroom::StateVisitBathroom()
{
}

StateVisitBathroom::~StateVisitBathroom()
{
}

StateVisitBathroom* StateVisitBathroom::Instance()
{
	static StateVisitBathroom instance;

	return &instance;
}

void StateVisitBathroom::Enter(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " 
		<< "gonna take a pee";
}

void StateVisitBathroom::Execute(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
		<< "Im in da bathroom";
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " 
		<< "done";
	sim->GetFSM()->RevertToPreviousState();
}

void StateVisitBathroom::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Leaving the bathroom";
}

bool StateVisitBathroom::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}