#include "StateDead.h"
#include "StateWalking.h"
#include "StateIdle.h"
#include "StateWorking.h"
#include "StateMachine.h"
#include "StateEating.h"
#include "Sim.h"
#include "Item.h"
#include "Shop.h"
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Timer.h"
#include "EntityManager.h"
#include <iostream>
#include "Storage.h"
StateDead::StateDead()
{
}

StateDead::~StateDead()
{
}

StateDead* StateDead::Instance()
{
	static StateDead instance;

	return &instance;
}

void StateDead::Enter(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << ": is dead";
}

void StateDead::Execute(Sim* sim)
{
}

void StateDead::Exit(Sim* sim)
{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
			<< "have been resurrected";
}

bool StateDead::OnMessage(Sim* sim, const Telegram& msg)
{
	return false;
}