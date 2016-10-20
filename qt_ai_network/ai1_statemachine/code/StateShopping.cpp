#include "StateShopping.h"
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
StateShopping::StateShopping()
{
}

StateShopping::~StateShopping()
{
}

StateShopping* StateShopping::Instance()
{
	static StateShopping instance;

	return &instance;
}

void StateShopping::Enter(Sim* sim)
{
	if (!sim->IsInAreaOf(sim->GetDestination()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": Shopping time, I must get to the: " << sim->GetDestination()->name;
		sim->GetFSM()->ChangeState(StateWalking::Instance());
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << ": Shopping :D";
	}
}

void StateShopping::Execute(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << "";
	//do the shopping here
	Shop* shop = Storage::Instance()->GetShop(sim->GetDestination()->name);
	Item* randItem = shop->GetRandomItem();
	if (sim->Buy(randItem))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << " bought item: " << randItem->name << " for " << randItem->value;
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": item was too expensive";
	}
	sim->GetFSM()->ChangeState(StateIdle::Instance());
}

void StateShopping::Exit(Sim* sim)
{
	if (sim->IsInAreaOf(sim->GetDestination()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
			<< "stop shopping";
	}
}

bool StateShopping::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}