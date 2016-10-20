#pragma once 
#include <vector>
#include <string>
struct Shop;
class Job;
class Location;
class Sim;


class Storage
{

private:
	//typedef std::vector<int> EntityMap;
	//to facilitate quick lookup the entities are stored in a std::map, in which
	//pointers to entities are cross referenced by their identifying number
	
	
	
	Storage();
	~Storage();

	//copy ctor and assignment should be private
	Storage(const Storage&);
	Storage& operator=(const Storage&);
	
public:
	std::vector<Sim*> simsList;
	std::vector<Location*> locationsList;
	std::vector<Shop*> shopsList;
	std::vector<Job*> jobsList;

	static Storage* Instance();
	std::vector<std::string>* LoadDirectory(const char * path);
	bool LoadJobs(const char* path);
	bool LoadShops(const char* path);
	bool LoadShop(const char * path);
	bool LoadLocations(const char * path);
	bool LoadSims(const char* path);
	Job* GetJob(std::string jobName);
	Location* GetLocation(std::string locationName);
	Location* GeRandomLocation();
	Sim* GetSim(std::string simName);
	Shop* GetShop(std::string shopName);
	Shop* GetRandomShop();
	Sim* GetRandomSim();
	Shop* GetShopAt(int index);
	Location* GetRandomShopLocation();
	int ShopCount();
	

};

