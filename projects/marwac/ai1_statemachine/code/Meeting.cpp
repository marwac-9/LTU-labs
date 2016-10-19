#include "Meeting.h"
#include "Timer.h"
#include "Sim.h"
#include "MessageTypes.h"
#include "MessageDispatcher.h"
#include "EntityManager.h"
Meeting::Meeting(Sim* orgnzr, bool publc, int d, int h, int m, int dur, Location* loc)
{
	time.d = d;
	time.h = h;
	time.m = m;
	location = loc;
	duration = dur;
	organizer = orgnzr;
	isPublic = publc;
	time.tm = Timer::Instance()->convertTimeToTotalMinutes(time);
}

mTime Meeting::GetTime()
{
	return time;
}

Location* Meeting::GetLocation()
{
	return location;
}

void Meeting::AddParticipant(Sim* sim)
{
	participants.push_back(sim);
}

int Meeting::GetParticipantsCount()
{
	return participants.size();
}

int Meeting::GetDuration()
{
	return duration;
}

void Meeting::SetSocializingStart(int start)
{
	socializingStart = start;
}

int Meeting::CountOfPresentParticipants()
{
	return presentParticipants.size();
}

void Meeting::ReportPresence(Sim* sim)
{
	presentParticipants.push_back(sim);
}

bool Meeting::IsEveryonePresent()
{
	return participants.size() == presentParticipants.size();
}

bool Meeting::IsPastSocializingTime()
{
	return Timer::Instance()->getTotalMinutes() > socializingStart + duration;
}

bool Meeting::AreMoreThanOneParticipants()
{
	return presentParticipants.size() > 1;
}

void Meeting::LeaveMeeting(Sim* sim)
{
	int index = FindPresentParticipantIndex(sim);
	if (index != -1)
	{
		presentParticipants.erase(presentParticipants.begin() + index);
	}
	
}

int Meeting::FindPresentParticipantIndex(Sim* sim)
{
	for (size_t i = 0; i < presentParticipants.size(); i++)
	{
		if (presentParticipants.at(i) == sim)
		{
			return i;
		}
	}
	return -1;
}

bool Meeting::IsMeetingStarted()
{
	return socializingStart > -1;
}

void Meeting::StartSocializingTime()
{
	SetSocializingStart(Timer::Instance()->getTotalMinutes());
}

int Meeting::GetStartTime()
{
	return socializingStart;
}

bool Meeting::HasEveryoneLeft()
{
	return presentParticipants.size() < 1;
}

void Meeting::CancelMeeting()
{
	for (size_t i = 0; i < participants.size(); i++)
	{
		organizer->SendMessageToGUI(EntityManager::Instance()->GetEntityName(organizer->GetId()) + ": MeetingCanceled to " + EntityManager::Instance()->GetEntityName(participants.at(i)->GetId()));
		MessageDispatcher::Instance()->DispatchAMessage(SEND_MSG_IMMEDIATELY, //time delay
			organizer->GetId(),        //ID of sender
			participants.at(i)->GetId(),        //ID of receiver
			MeetingCanceled,   //the message
			this);
	}
	delete this;
}

bool Meeting::IsTimeBeforeMeeting()
{
	return Timer::Instance()->getTotalMinutes() < time.tm;
}

bool Meeting::IsPublic()
{
	return isPublic;
}

bool Meeting::IsPastMeetingTime()
{
	return Timer::Instance()->getTotalMinutes() > time.tm + duration;
}

void Meeting::UnsubscribeMeeting(Sim* sim)
{
	int index = FindParticipantIndex(sim);
	if (index != -1)
	{
		participants.erase(participants.begin() + index);
	}
}

int Meeting::FindParticipantIndex(Sim* sim)
{
	for (size_t i = 0; i < participants.size(); i++)
	{
		if (participants.at(i) == sim)
		{
			return i;
		}
	}
	return -1;
}

std::string Meeting::IsPublicToStr()
{
	if (isPublic)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}
