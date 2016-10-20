#include "BaseGameEntity.h"
#include <assert.h>   

int BaseGameEntity::nextId = 0;

BaseGameEntity::BaseGameEntity(int id)
{
	SetId(id);
}

BaseGameEntity::~BaseGameEntity()
{
}

void BaseGameEntity::Update()
{
}

int BaseGameEntity::GetId() const
{
	return this->id;
}

void BaseGameEntity::SetId(int val)
{
	//make sure the val is equal to or greater than the next available ID
	assert((val >= this->nextId) && "<BaseGameEntity::SetID>: invalid ID");

	this->id = val;

	this->nextId = id + 1;
}




