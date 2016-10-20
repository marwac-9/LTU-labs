#include "Scene.h"
#include "Node.h"
#include "Object.h"

Scene::Scene()
{
	idCounter = 0;
}

Scene::~Scene()
{
}

Scene* Scene::Instance()
{
	static Scene instance;

	return &instance;
}

Object* Scene::build()
{
	SceneObject = new Object();
	return SceneObject;
}

Object* Scene::addChild(Object* parentObject)
{
	//bind
	Object* child = new Object();
	parentObject->node.addChild(&child->node);

	child->ID = idCounter;
	objectsToRender[idCounter] = child;
	idCounter++;
	LastAddedObject = child;
	return child;
}