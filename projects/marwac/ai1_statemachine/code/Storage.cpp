#include "Storage.h"
#include "Job.h"
#include "Shop.h"
#include "Item.h"
#include "Sim.h"
#include "EntityManager.h"
#include "dirent.h"
#include <iostream>
Storage::Storage()
{
}

Storage::~Storage()
{
}

Storage* Storage::Instance()
{
	static Storage instance;

	return &instance;
}

bool Storage::LoadJobs(const char * path) {

	FILE * file;
	errno_t err = fopen_s(&file, path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	char line[128];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '/' || line[0] == ';' || line[0] == '#') continue; /* ignore comment line */
		int workingFrom, workHours, payment;
		char jobName[128];
		char locationName[128];
		int matches = sscanf_s(line, "%s %s %d %d %d", jobName, _countof(jobName), locationName, _countof(locationName), &workingFrom, &workHours, &payment);
		if (matches != 5)
		{
			printf("Wrong job information!\n");
		}
		else {
			Location* jobLocation = GetLocation(locationName);
			Job* job = new Job(jobName, jobLocation, workingFrom, workHours, payment);
			this->jobsList.push_back(job);
		}

	}
	fclose(file);
	return true;
}

bool Storage::LoadShops(const char * path) {

	std::vector<std::string>* files = LoadDirectory(path);
	if (files != NULL)
	{
		for (size_t i = 0; i < files->size(); i++)
		{
			std::string slash = "/";
			std::string fileWithPath = path + slash + files->at(i);
			LoadShop(fileWithPath.c_str());
		}
		delete files;
		return true;
	}
	return false;
}

bool Storage::LoadShop(const char * path) {

	FILE * file;
	errno_t err = fopen_s(&file, path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	Shop* shop;
	char line[128];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '/' || line[0] == ';' || line[0] == '#') continue; /* ignore comment line */
		
		char header[128];
		if (line[0] == 'n')
		{
			shop = new Shop();
			char shopName[128];
			int matches = sscanf_s(line, "%s %s", header, _countof(header), shopName, _countof(shopName));
			shop->name = shopName;
			this->shopsList.push_back(shop);
		}

		if (line[0] == 'i')
		{
			char itemName[128];
			int itemValue = 0;
			int matches = sscanf_s(line, "%s %s %d", header, _countof(header), itemName, _countof(itemName), &itemValue);
			if (matches != 3)
			{
				printf("Wrong item information!\n");
			}
			else {
				Item* newItem = new Item();
				newItem->name = itemName;
				newItem->value = itemValue;
				shop->itemsList.push_back(newItem);
			}
		}
	}
	fclose(file);
	return true;
}

bool Storage::LoadLocations(const char * path) {

	FILE * file;
	errno_t err = fopen_s(&file, path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	char line[128];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '/' || line[0] == ';' || line[0] == '#') continue; /* ignore comment line */
		int posX, posY, red, green, blue, width, height;
		char locationName[128];
		int matches = sscanf_s(line, "%s %d %d %d %d %d %d %d", locationName, _countof(locationName), &posX, &posY, &red, &green, &blue, &width, &height);
		if (matches != 8)
		{
			printf("Wrong location information!\n");
		}
		else {
			Location* newLocation = new Location();
			newLocation->name = locationName;
			newLocation->SetPosition(posX, posY);
			newLocation->SetColor(red, green, blue);
			newLocation->SetSize(width, height);
			this->locationsList.push_back(newLocation);
		}

	}
	fclose(file);
	return true;
}

std::vector<std::string>* Storage::LoadDirectory(const char * path) {

	std::vector<std::string> * files = new std::vector<std::string>();

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
			{
				files->push_back(ent->d_name);
			}
			//printf("%s\n", ent->d_name);
		}
		closedir(dir);
		return files;
	}
	else {
		/* could not open directory */
		perror("could not open directory");
		return NULL;
	}
}

bool Storage::LoadSims(const char * path) {

	FILE * file;
	errno_t err = fopen_s(&file, path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	char line[128];
	int id = 0;
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '/' || line[0] == ';' || line[0] == '#') continue; /* ignore comment line */
		int money, loneliness, hunger, thirst, fatigue, hatred, red, green, blue, width, height;
		char simName[128];
		char startLocation[128];
		char job[128];
		char homeLocation[128];
		int matches = sscanf_s(line, "%s %s %s %s %d %d %d %d %d %d %d %d %d %d %d", 
			simName, _countof(simName), 
			startLocation, _countof(startLocation), 
			homeLocation, _countof(homeLocation),
			job, _countof(job), 
			&money, &loneliness, &hunger, &thirst, &fatigue, &hatred, &red, &green, &blue, &width, &height);
		if (matches != 15)
		{
			printf("Wrong sim information! %s\n", simName);
		}
		else {
			Sim* sim = new Sim(id, simName, money, loneliness, hunger, thirst, fatigue, hatred);
			sim->SetColor(red, green, blue);
			Job* simJob = GetJob(job);
			sim->AssignJob(simJob);
			sim->AssignHomeLocation(GetLocation(homeLocation));
			sim->ChangeLocation(*GetLocation(startLocation));
			sim->SetSize(width, height); // should be read from file
			simsList.push_back(sim);
			id++;
		}

	}
	fclose(file);
	return true;
}

Job* Storage::GetJob(std::string jobName)
{
	for (size_t i = 0; i < jobsList.size(); i++)
	{
		if (jobsList.at(i)->GetName().compare(jobName) == 0)
		{
			return jobsList.at(i);
		}
		
	}
	return false;
}

Sim* Storage::GetSim(std::string simName)
{
	for (size_t i = 0; i < simsList.size(); i++)
	{
		if (simsList.at(i)->name.compare(simName) == 0)
		{
			return simsList.at(i);
		}

	}
	return false;
}

Shop* Storage::GetShop(std::string shopName)
{
	for (size_t i = 0; i < shopsList.size(); i++)
	{
		if (shopsList.at(i)->name.compare(shopName) == 0)
		{
			return shopsList.at(i);
		}

	}
	return false;
}

Location* Storage::GetLocation(std::string locationName)
{
	for (size_t i = 0; i < locationsList.size(); i++)
	{
		if (locationsList.at(i)->name.compare(locationName) == 0)
		{
			return locationsList.at(i);
		}

	}
	return false;
}

Shop* Storage::GetShopAt(int index)
{
	return shopsList.at(index);
}

Shop* Storage::GetRandomShop()
{
	int index = rand() % shopsList.size();
	return shopsList.at(index);
}

int Storage::ShopCount()
{
	return shopsList.size();
}

Location* Storage::GetRandomShopLocation()
{
	Shop* randShop = GetRandomShop();
	return GetLocation(randShop->name);
}

Sim* Storage::GetRandomSim()
{
	int index = rand() % simsList.size();
	return simsList.at(index);
}

Location* Storage::GeRandomLocation()
{
	int index = rand() % locationsList.size();
	return locationsList.at(index);
}

