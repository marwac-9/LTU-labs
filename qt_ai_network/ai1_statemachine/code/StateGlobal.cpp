#include "StateGlobal.h"
#include "StateEating.h"
#include "StateSocialize.h"
#include "StateDrinking.h"
#include "StateWalking.h"
#include "StateDead.h"
#include "StateIdle.h"
#include "StateVisitBathroom.h"
#include "StateSleeping.h"
#include "StateWorking.h"
#include "StateMachine.h"
#include "Sim.h"
#include "Telegram.h"
#include "Meeting.h"
#include "MessageDispatcher.h"
#include "MessageTypes.h"
#include "Timer.h"
#include "EntityManager.h"
#include <iostream>
#include <time.h> 
#include "Storage.h"

StateGlobal::StateGlobal()
{
}

StateGlobal::~StateGlobal()
{
}

StateGlobal* StateGlobal::Instance()
{
	static StateGlobal instance;

	return &instance;
}

void StateGlobal::Enter(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "entering global state";
}

void StateGlobal::Execute(Sim* sim)
{
	if (sim->Dead())
	{ 
		sim->GetFSM()->SetGlobalState(nullptr);
		sim->GetFSM()->ChangeState(StateDead::Instance());
	}
	else
	{
		if (sim->GetUpdateCounter() >= 30)
		{
			sim->IncreaseHunger();
			sim->IncreaseThirst();
			sim->IncreaseFatigue();
			sim->IncreaseLoneliness();

			sim->ResetUpdateCounter();
		}
		//if he gets thirsty, drink
		if (sim->Thirsty() && !sim->GetFSM()->isInState(*StateWalking::Instance()) && !sim->GetFSM()->isInState(*StateSleeping::Instance()))
		{
			sim->GetFSM()->ChangeState(StateDrinking::Instance());
		}
		//if he gets hungry eat
		if (sim->Hungry() && !sim->GetFSM()->isInState(*StateWalking::Instance()) && !sim->GetFSM()->isInState(*StateSleeping::Instance()))
		{
			sim->GetFSM()->ChangeState(StateEating::Instance());
		}
		if (sim->Lonely() && !sim->HasAnyMeetingsPlanned() && !sim->GetFSM()->isInState(*StateSleeping::Instance()))
		{
			//check when he asked last time if it was yesterday then he has a new chance to ask for meeting
			if (sim->LastTimeTurnedDown() < Timer::Instance()->getDays())
			{
				sim->ResetTurnedDown();
			}
			if (!sim->TurnedDownTooManyTimes())
			{
				//let's make a meeting
				int randomSim = Storage::Instance()->GetRandomSim()->GetId();
				while (true)
				{
					if (sim->GetId() != randomSim)
					{
						std::cout << "tried sim: " << randomSim;
						break;
					}
					randomSim = Storage::Instance()->GetRandomSim()->GetId();
				}
				std::cout << "\npicked sim: " << EntityManager::Instance()->GetEntityName(randomSim);
				sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": CanWeMeet to " + EntityManager::Instance()->GetEntityName(randomSim));
				MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
					sim->GetId(),        //ID of sender
					randomSim,        //ID of receiver
					CanWeMeet,   //the message
					NO_ADDITIONAL_INFO);
			}
		}
		if (sim->IsTimeForMeeting() && !sim->GetFSM()->isInState(*StateWorking::Instance()) && !sim->GetFSM()->isInState(*StateSleeping::Instance()))
		{
			//if sim destination is not the meeting place
			if (sim->GetDestination() != sim->GetMeeting()->GetLocation())
			{
				sim->ChangeDestination(sim->GetMeeting()->GetLocation());
				sim->GetFSM()->ChangeState(StateSocialize::Instance());
			}
			//if sim is already at the place but not socializing
			if (sim->IsInAreaOf(sim->GetMeeting()->GetLocation()) && !sim->GetFSM()->isInState(*StateSocialize::Instance()))
			{
				sim->GetFSM()->ChangeState(StateSocialize::Instance());
			}
		}
		//this is only to check if we are late
		if (sim->HasAnyMeetingsPlanned())
		{
			std::cout << "\n\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << " I HAVE MEETING: " << sim->HasAnyMeetingsPlanned() << " at: " << sim->GetMeeting()->GetLocation()->name << " time: " << sim->GetMeeting()->GetTime().h << " is public: " << sim->GetMeeting()->IsPublic() << "\n\n";
			//performing check in steps to decrease check amount by avoiding unnecessary checks that exclude the following checks
			Meeting* meeting = sim->GetMeeting();
			//if meeting time passed it's almost s
			if (meeting->IsPastMeetingTime())
			{
				if (!meeting->IsMeetingStarted())
				{
					meeting->CancelMeeting();
				}
			}
		}
		else
		{
			std::cout << "\n\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << " I HAVE MEETING: " << sim->HasAnyMeetingsPlanned() << "\n\n";
		}
	}
}

void StateGlobal::Exit(Sim* sim)
{
	std::cout << "\n" << EntityManager::Instance()->GetEntityName(sim->GetId()) << ": " << "Leaving the saloon, feelin' good";
}

bool StateGlobal::OnMessage(Sim* sim, const Telegram& msg)
{
	switch (msg.msg)
	{
		case CanWeMeet:
		{
			//ignore message sent to himself, this is in case we send CanWeMeet to all
			if (msg.sender != sim->GetId())
			{
				if (!sim->HasAnyMeetingsPlanned() && sim->Lonely())
				{
					sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": YesWhatTimeWeMeet to " + EntityManager::Instance()->GetEntityName(msg.sender));
					MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
						sim->GetId(),        //ID of sender
						msg.sender,        //ID of receiver
						YesWhatTimeWeMeet,   //the message
						NO_ADDITIONAL_INFO);
				}
				else if (!sim->Lonely())
				{
					sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": ImBusy to " + EntityManager::Instance()->GetEntityName(msg.sender));
					MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
						sim->GetId(),        //ID of sender
						msg.sender,        //ID of receiver
						ImBusy,   //the message
						NO_ADDITIONAL_INFO);
				}
				else if (sim->HasAnyMeetingsPlanned())
				{
					
					Meeting* meeting = sim->GetMeeting();
					//if sim has a meeting planned and it's before the meeting time and it's public
					if (meeting->IsTimeBeforeMeeting() && meeting->IsPublic())
					{
						sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": YouCanJoinUs to " + EntityManager::Instance()->GetEntityName(msg.sender));
						MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
							sim->GetId(),        //ID of sender
							msg.sender,        //ID of receiver
							YouCanJoinUs,   //the message
							meeting);
					}
					else
					{	//too late or private
						sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": ImBusy to " + EntityManager::Instance()->GetEntityName(msg.sender));
						MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
							sim->GetId(),        //ID of sender
							msg.sender,        //ID of receiver
							ImBusy,   //the message
							NO_ADDITIONAL_INFO);
					}
				}
				else
				{
					std::cout << "\nUNEXPECTED\n";
				}
			}
			return true;
		}
		case YesWhatTimeWeMeet:
		{
			
			sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": MeetingTime to " + EntityManager::Instance()->GetEntityName(msg.sender));
			std::cout << "\nMessage handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
				<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes();
			Job* simJob = sim->GetCurrentJob();
			int workHours = simJob->GetWorkHours();
			int workFrom = simJob->GetWorkingFrom();
			bool isPublic = rand() % 2 == 1; // it's true or false
			int sumOfHours = workFrom + workHours + 1;
			if (Timer::Instance()->getHours() > sumOfHours)
			{
				sumOfHours = Timer::Instance()->getHours(); // this solves so the sims won't try to request meeting for time that has already passed
			}
			int randomHour = sumOfHours + (rand() % (23 - sumOfHours + 1));
			int randomDay = Timer::Instance()->getDays() + (rand() % 3); // 0 days to 2 days away, now days hours and minutes are supported by meeting but it's pointless to set days as they are in need of meeting they don't want to wait for days
			int randomDuration = 30 + rand() % 120; //from 30 min to two hours, this is in case we want sims to stay at meeting for specific period of time, otherwise sims will stay until they are no longer lonely or everyone else leaves
			Location* meetingLocation = Storage::Instance()->GeRandomLocation();
			Meeting* newMeeting = new Meeting(sim, isPublic, Timer::Instance()->getDays(), randomHour, 0, randomDuration, meetingLocation);
			MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
				sim->GetId(),        //ID of sender
				msg.sender,        //ID of receiver
				MeetingTime,   //the message
				newMeeting);
			return true;
		}
		case MeetingTime:
		{
			Meeting* receivedMeeting = (Meeting*)msg.extraInfo;
			std::cout << "\nTime: " << receivedMeeting->GetTime().h << " Location: " << receivedMeeting->GetLocation()->name;
			Job* simJob = sim->GetCurrentJob();
			int workHours = simJob->GetWorkHours();
			int workFrom = simJob->GetWorkingFrom();
			int sumOfHours = workFrom + workHours;
			if (receivedMeeting->GetTime().h > sumOfHours)
			{
				sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": OkIWillComeAtThisTime to " + EntityManager::Instance()->GetEntityName(msg.sender));
				std::cout << "\nMessage handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
					<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes();
				//asked person accepts meeting 
				sim->AddMeeting(receivedMeeting);
				receivedMeeting->AddParticipant(sim);
				MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
					sim->GetId(),        //ID of sender
					msg.sender,        //ID of receiver
					OkIWillComeAtThisTime,   //the message
					receivedMeeting);
			}
			else
			{
				sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": ImBusy to " + EntityManager::Instance()->GetEntityName(msg.sender));
				MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
					sim->GetId(),        //ID of sender
					msg.sender,        //ID of receiver
					ImBusy,   //the message
					receivedMeeting);
			}
			return true;
			
		}
		case OkIWillComeAtThisTime:
		{
			Meeting* acceptedMeeting = (Meeting*)msg.extraInfo;
			sim->ResetTurnedDown();
			sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) 
									+ ": Creating a meeting at " + acceptedMeeting->GetLocation()->name
									+ " time: " + std::to_string(acceptedMeeting->GetTime().h)
									+ " is public: " + acceptedMeeting->IsPublicToStr());
			std::cout << "\nMessage handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
				<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes();
			//since proposed meeting is accepted it's safe to save it 
				
			sim->AddMeeting(acceptedMeeting);
			acceptedMeeting->AddParticipant(sim);
			return true;
		}

		case ImBusy:
		{
			//proposed meeting was rejected and has to be deleted
			sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": I was turned down by " + EntityManager::Instance()->GetEntityName(msg.sender));
			Meeting* receivedMeeting = (Meeting*)msg.extraInfo;
			delete receivedMeeting;
			sim->IncreaseTurnedDown();
			std::cout << "\nMessage ImBusy from " << EntityManager::Instance()->GetEntityName(msg.sender) << " handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
				<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes();
			return true;
		}
		
		case YouCanJoinUs:
		{
			Meeting* receivedMeeting = (Meeting*)msg.extraInfo;
			std::cout << "\nTime: " << receivedMeeting->GetTime().h << " Location: " << receivedMeeting->GetLocation()->name;
			Job* simJob = sim->GetCurrentJob();
			int workHours = simJob->GetWorkHours();
			int workFrom = simJob->GetWorkingFrom();
			int sumOfHours = workFrom + workHours;
			if (receivedMeeting->GetTime().h > sumOfHours)
			{
				std::cout << "\nMessage handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
					<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes();
				sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": OkIWillJoinYou to " + EntityManager::Instance()->GetEntityName(msg.sender));
				sim->AddMeeting(receivedMeeting);
				receivedMeeting->AddParticipant(sim);
				MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
					sim->GetId(),        //ID of sender
					msg.sender,        //ID of receiver
					OkIWillJoinYou,   //the message
					receivedMeeting);
			}
			else
			{
				sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": ImBusy to " + EntityManager::Instance()->GetEntityName(msg.sender));
				MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
					sim->GetId(),        //ID of sender
					msg.sender,        //ID of receiver
					ImBusy,   //the message
					NO_ADDITIONAL_INFO);
			}
			return true;
		}

		case OkIWillJoinYou:
		{
			Meeting* receivedMeeting = (Meeting*)msg.extraInfo;
			sim->ResetTurnedDown();

			sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) 
								+ ": will join meeting at: " 
								+ receivedMeeting->GetLocation()->name
								+ " time: " + std::to_string(receivedMeeting->GetTime().h)
								+ " is public: " + receivedMeeting->IsPublicToStr());
			std::cout << "\nMessage handled by " << EntityManager::Instance()->GetEntityName(sim->GetId())
				<< " at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes() << " will join meeting";
			return true;
		}

		case MeetingCanceled:
		{
			sim->SendMessageToGUI(EntityManager::Instance()->GetEntityName(sim->GetId()) + ": Removing meeting from my list");
			Meeting* receivedMeeting = (Meeting*)msg.extraInfo;
			if (sim->GetFSM()->isInState(*StateSocialize::Instance()))
			{
				sim->GetFSM()->ChangeState(StateIdle::Instance());
			}
			else if (sim->GetFSM()->isInState(*StateWalking::Instance()) && sim->GetDestination() == receivedMeeting->GetLocation())
			{
				sim->GetFSM()->ChangeState(StateIdle::Instance());
			}
			sim->RemoveMeeting(receivedMeeting);
			return true;
		}
		return true;

	}//end switch

	return false; //send message to global message handler
}