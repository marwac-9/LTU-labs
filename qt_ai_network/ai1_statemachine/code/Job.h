#pragma once
#include <string>
class Location;
class Job
{
public:
	//Job specifics
	Job(const char* name, Location* loc, int workingFrom, int workHours, int payment);
	Job(int payment);
	int GetPayment();
	int GetWorkingFrom();
	int GetWorkHours();
	Location* GetLocation();
	std::string GetName();
private:
	std::string name = "new job";
	int payment = 20;
	int workingFrom = 8;
	int workHours = 8;
	Location* loc;
};

