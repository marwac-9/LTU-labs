#include "EntityManager.h"
#include "BaseGameEntity.h"
#include <assert.h>


EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

EntityManager* EntityManager::Instance()
{
	static EntityManager instance;

	return &instance;
}


BaseGameEntity* EntityManager::GetEntityFromID(int id) const
{
	//find the entity
	EntityMap::const_iterator ent = entityMap.find(id);

	//assert that the entity is a member of the map
	assert((ent != entityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

	return ent->second;
}

BaseGameEntity* EntityManager::GetEntityFromName(std::string name) const
{
	EntityMap::const_iterator it = entityMap.begin();
	while (it != entityMap.end())
	{
		if (it->second->name == name)
			return it->second;
		++it;
	}
	//did not find the entity
	//moving iterator back so we can make assert, otherwise we get an error about impossible dereferencing because of last last i++ in while loop
	it--;
	assert(it->second->name == name  && "<EntityManager::GetEntityFromName>: invalid name");
	return NULL;
}

std::string EntityManager::GetEntityName(int id) const
{
	//find the entity
	EntityMap::const_iterator ent = entityMap.find(id);

	//assert that the entity is a member of the map
	assert((ent != entityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

	return ent->second->name;
}

void EntityManager::RemoveEntity(BaseGameEntity* entity)
{
	entityMap.erase(entityMap.find(entity->GetId()));
}

void EntityManager::RegisterEntity(BaseGameEntity* newEntity)
{
	entityMap.insert(std::make_pair(newEntity->GetId(), newEntity));
}

void EntityManager::Update(BaseGameEntity* entity)
{
	EntityMap::const_iterator ent = entityMap.find(entity->id);

	//assert that the entity is a member of the map
	assert((ent != entityMap.end()) && "<EntityManager::GetEntityFromID>: invalid ID");

	ent->second->Update();
}

void EntityManager::UpdateAll()
{
	std::map<int, BaseGameEntity*>::iterator it;
	for (it = entityMap.begin(); it != entityMap.end(); it++)
	{
		it->second->Update();
	}
}

int EntityManager::EntityCount()
{
	return entityMap.size();
}