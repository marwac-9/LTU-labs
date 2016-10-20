#pragma once 
#include "mTime.h"

class Timer
{
public:
	void tick();
	int getDays();
	int getHours();
	int getMinutes();
	int getTotalMinutes();
	mTime convertTotalMinutesToTime(int totalMinutes);
	int convertTimeToTotalMinutes(mTime time);
	mTime getTime();
	static Timer* Instance();
private:
	Timer();
	~Timer();
	mTime time;
	void tickDays();
	void tickHours();
	void tickMinutes();
	
};


