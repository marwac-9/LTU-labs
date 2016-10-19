#pragma once 
#include <string>
#include <vector>
#include "Location.h"
struct Item;

struct Shop : public Location
{
public:
	Item* GetItem(int index);
	Item* GetRandomItem();
	std::vector<Item*> itemsList;
private:

};

