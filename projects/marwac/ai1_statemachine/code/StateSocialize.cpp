#include "StateWalking.h"
#include "StateSocialize.h"
#include "StateEating.h"
#include "StateIdle.h"
#include "Sim.h"
#include "Meeting.h"
#include "EntityManager.h"
#include <iostream>
#include "Telegram.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "StateMachine.h"
#include "Timer.h"
#include "Storage.h"
StateSocialize::StateSocialize()
{
}

StateSocialize::~StateSocialize()
{
}

StateSocialize* StateSocialize::Instance()
{
	static StateSocialize instance;

	return &instance;
}

void StateSocialize::Enter(Sim* sim)
{
	//all sims when arriving have to report their arrival
	if (!sim->IsInAreaOf(sim->GetDestination()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "going to meeting at: " << sim->GetMeeting()->GetLocation()->name;
		sim->GetFSM()->ChangeState(StateWalking::Instance());
	}
	else
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Meeting time, at: " << sim->GetMeeting()->GetLocation()->name;
		sim->ChangeLocationName("At meeting: " + sim->GetDestination()->name);
		sim->GetMeeting()->ReportPresence(sim);
	}
}

void StateSocialize::Execute(Sim* sim)
{
	Meeting* meeting = sim->GetMeeting();
	if (meeting->AreMoreThanOneParticipants())
	{
		if (!meeting->IsMeetingStarted())
		{
			meeting->StartSocializingTime();
		}
		sim->DecreaseLoneliness();
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": Socializing at: " << meeting->GetLocation()->name;
	}
	if (meeting->IsPastSocializingTime() && meeting->IsMeetingStarted())
	{
		meeting->LeaveMeeting(sim); // this could be moved to the exit
		meeting->CancelMeeting(); //cancel meeting could be called end meeting as well
	}
}

void StateSocialize::Exit(Sim* sim)
{
	if (sim->IsInAreaOf(sim->GetDestination()))
	{
		std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "leaving meeting";
	}
}

bool StateSocialize::OnMessage(Sim* sim, const Telegram& msg)
{
	return false; //send message to global message handler
}