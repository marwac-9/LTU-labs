#include "Job.h"

int Job::GetPayment()
{
	return payment;
}

Job::Job(const char* name, Location* loc, int workingFrom, int workHours, int payment)
{
	this->name = name;
	this->loc = loc;
	this->workingFrom = workingFrom;
	this->workHours = workHours;
	this->payment = payment;
}

Job::Job(int payment)
{
	this->payment = payment;
}

int Job::GetWorkingFrom()
{
	return workingFrom;
}

int Job::GetWorkHours()
{
	return workHours;
}

std::string Job::GetName()
{
	return this->name;
}

Location* Job::GetLocation()
{
	return this->loc;
}
