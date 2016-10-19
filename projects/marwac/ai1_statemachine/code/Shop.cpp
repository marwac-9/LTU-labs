#include "Shop.h"

Item* Shop::GetRandomItem()
{
	int index = rand() % itemsList.size();
	return GetItem(index);
}

Item* Shop::GetItem(int index)
{
	return itemsList.at(index);
}