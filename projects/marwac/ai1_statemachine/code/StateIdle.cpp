#include "StateWalking.h"
#include "StateWorking.h"
#include "StateSleeping.h"
#include "StateShopping.h"
#include "StateIdle.h"
#include "StateDrinking.h"
#include "StateEating.h"
#include "Sim.h"
#include "EntityManager.h"
#include <iostream>
#include "StateMachine.h"
#include "Storage.h"
#include "Meeting.h"
StateIdle::StateIdle()
{
}

StateIdle::~StateIdle()
{
}

StateIdle* StateIdle::Instance()
{
	static StateIdle instance;

	return &instance;
}

void StateIdle::Enter(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "entering idle";
}


void StateIdle::Execute(Sim* sim)
{
	//we do all checks and change states if needed
	if (sim->Fatigued())
	{
		if (sim->HasAnyMeetingsPlanned())
		{
			Meeting* simMeeting = sim->GetMeeting();
			if (simMeeting->GetParticipantsCount() < 3)
			{
				simMeeting->CancelMeeting();
			}
			else
			{
				simMeeting->UnsubscribeMeeting(sim);
				sim->RemoveMeeting(simMeeting);
			}
		}
		sim->GetFSM()->ChangeState(StateSleeping::Instance());
	}
	//since this is in idle state then if sim is doing something else he will go to work after he finished previous task
	//if this is in global state sim will switch immediately to working
	else if (sim->IsTimeToWork()) //this function checks hour so a sim can be late for one hour top then he misses his job day
	{
		sim->GetFSM()->ChangeState(StateWorking::Instance());
	}
	else if (sim->IsInAreaOf(sim->GetJobLocation()) && sim->IsNowWorkingHours())
	{
		sim->GetFSM()->ChangeState(StateWorking::Instance());
	}
	else if (sim->Wealthy())
	{
		sim->ChangeDestination(Storage::Instance()->GetRandomShopLocation());
		sim->GetFSM()->ChangeState(StateShopping::Instance());
	}
	else
	{
		if (!sim->IsInAreaOf(sim->GetHomeLocation()))
		{
			sim->ChangeDestination(sim->GetHomeLocation());
			sim->GetFSM()->ChangeState(StateWalking::Instance());
		}
		// if none of the conditions is true
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << " Idling";
	}
}


void StateIdle::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": "
		<< "stop idling";
}

bool StateIdle::OnMessage(Sim* sim, const Telegram& msg)
{
	//send msg to global message handler
	return false;
}