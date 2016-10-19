#pragma once
#include "mTime.h"
#include <vector>
class Sim;
class Location;
class Meeting 
{
public:
	Meeting(Sim* organizer, bool isPublic, int d, int h, int m, int duration, Location* loc);
	Location* GetLocation();
	mTime GetTime();
	void AddParticipant(Sim* sim);
	int GetParticipantsCount();
	int GetDuration();
	int GetStartTime();
	int CountOfPresentParticipants();
	void ReportPresence(Sim* sim);
	bool IsEveryonePresent();
	bool AreMoreThanOneParticipants();
	void LeaveMeeting(Sim* sim);
	bool IsMeetingStarted();
	void StartSocializingTime();
	bool HasEveryoneLeft();
	void CancelMeeting();
	bool IsTimeBeforeMeeting();
	bool IsPublic();
	std::string IsPublicToStr();
	bool IsPastSocializingTime();
	bool IsPastMeetingTime();
	void UnsubscribeMeeting(Sim* sim);
private:
	int FindPresentParticipantIndex(Sim* sim);
	int FindParticipantIndex(Sim* sim);
	void SetSocializingStart(int start);
	Sim* organizer;
	mTime time;
	int isPublic;
	Location* location;
	std::vector<Sim*> participants;
	int duration;
	int socializingStart = -1;
	std::vector<Sim*> presentParticipants;
};

