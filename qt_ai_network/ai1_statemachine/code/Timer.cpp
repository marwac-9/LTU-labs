#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

Timer* Timer::Instance()
{
	static Timer instance;

	return &instance;
}


void Timer::tickHours()
{
	if (time.h >= 23)
	{
		time.h = 0;
		tickDays();
	}
	else
	{
		time.h++;
	}
}

void Timer::tickMinutes()
{
	if (time.m >= 59)
	{
		time.m = 0;
	}
	else
	{
		time.m++;
	}
	time.tm++;
}

void Timer::tick()
{

	if (time.m >= 59)
	{
		tickMinutes();
		tickHours();
	}
	else
	{
		tickMinutes();
	}
}

int Timer::getHours()
{
	return time.h;
}

int Timer::getMinutes()
{
	return time.m;
}

mTime Timer::getTime()
{
	return time;
}

void Timer::tickDays()
{
	time.d++;
}

int Timer::getTotalMinutes()
{
	return time.tm;
}

mTime Timer::convertTotalMinutesToTime(int totalMinutes)
{
	mTime timeFromMinutes;
	timeFromMinutes.m = totalMinutes % 60;
	timeFromMinutes.h = (totalMinutes / 60) % 24;
	timeFromMinutes.d = totalMinutes / 60 / 24;
	return timeFromMinutes;
}

int Timer::getDays()
{
	return time.d;
}

int Timer::convertTimeToTotalMinutes(mTime time)
{
	return time.d * 24 * 60 + time.h * 60 + time.m;
}
