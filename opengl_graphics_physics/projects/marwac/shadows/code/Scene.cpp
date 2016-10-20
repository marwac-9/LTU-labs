#include "Scene.h"
#include "Node.h"
#include "Object.h"
#include "Material.h"
#include "GraphicsStorage.h"
#include "PhysicsManager.h"
#include "Mesh.h"
#include "OBJ.h"
using namespace mwm;
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

void Scene::addRandomObject()
{
	Object* newChild = Scene::addChild(SceneObject);

	int index = rand() % (3);
	float rX = (float)(rand() % 20 - 10);
	float rY = (float)(rand() % 20 - 10);
	float rZ = (float)(rand() % 20 - 10);

	float rS = (float)(rand() % 5);

	newChild->node.TransformationMatrix = Matrix4::translate(rX, rY, rZ)*Matrix4::scale(rS, rS, rS);
	newChild->radius = rS;	

	Material* newMaterial = new Material();

	auto it = GraphicsStorage::meshes.begin();
	std::advance(it, index); //rand() % GraphicsStorage::meshes.size()

	newChild->AssignMesh(it->second);
	newMaterial->AssignTexture(GraphicsStorage::textures.at(0));
	GraphicsStorage::materials.push_back(newMaterial);
	newChild->AssignMaterial(newMaterial);

}

Object* Scene::addObject(const char* name, const Vector3& pos)
{
	Object* newChild = Scene::addChild(SceneObject);
	newChild->SetPosition(pos);

	newChild->AssignMesh(GraphicsStorage::meshes[name]);
	Material* newMaterial = new Material();
	newMaterial->AssignTexture(GraphicsStorage::textures.at(0));
	GraphicsStorage::materials.push_back(newMaterial);
	newChild->AssignMaterial(newMaterial);

	return newChild;
}

Object* Scene::addPhysicObject(const char* name, const Vector3& pos)
{
	Object* newObj = addObject(name, pos);
	PhysicsManager::Instance()->AddObject(newObj);
	return newObj;
}

Object* Scene::addRandomlyObject(const char* name)
{
	Object* newChild = addChild(SceneObject);

	int rX = rand() % 40 - 20;
	int rY = rand() % 40 + 2;
	int rZ = rand() % 40 - 20;
	/*
	float rX = (float)(rand() % 40 - 20);
	float rY = (float)(rand() % 40 - 20);
	float rZ = (float)(rand() % 40 - 20);
	*/

	while (!((rY > 8 || rY < -8) || (rZ > 8 || rZ < -8) || (rX > 8 || rX < -8)))
	{
		rX = rand() % 40 - 20;
		rY = rand() % 40 + 2;
		rZ = rand() % 40 - 20;
		/*
		rX = (float)(rand() % 40 - 20);
		rY = (float)(rand() % 40 - 20);
		rZ = (float)(rand() % 40 - 20);
		*/
	}
	newChild->SetPosition(Vector3((float)rX, (float)rY, (float)rZ));

	//newChild->SetPosition(Vector3(0, idCounter * 2 - 10+0.001f, 0));
	
	/*
	else if (idCounter < 5)
	{
		newChild->SetPosition(Vector3(2, idCounter * 2 - 9.9-4, 0));
	}
	else if (idCounter < 7)
	{
		newChild->SetPosition(Vector3(0, idCounter * 2 - 9.9 - 4 *2, 2));
	}
	else if (idCounter < 9)
	{
		newChild->SetPosition(Vector3(-2, idCounter * 2 - 9.9 - 4 *3, 0));
	}
	else if (idCounter < 11)
	{
		newChild->SetPosition(Vector3(0, idCounter * 2 - 9.9 - 4 *4, -2));
	}
	else if (idCounter < 13)
	{
		newChild->SetPosition(Vector3(-2, idCounter * 2 - 9.9 - 4 *5, -2));
	}
	else if (idCounter < 15)
	{
		newChild->SetPosition(Vector3(2, idCounter * 2 - 9.9 - 4 * 6, 2));
	}
	else if (idCounter < 17)
	{
		newChild->SetPosition(Vector3(-2, idCounter * 2 - 9.9 - 4 * 7, 2));
	}
	else if (idCounter < 19)
	{
		newChild->SetPosition(Vector3(2, idCounter * 2 - 9.9 - 4 * 8, -2));
	}
	*/
	/*
	if (idCounter == 1)
	{
		newChild->SetPosition(Vector3(idCounter * 7, 2.2, 0));
		newChild->SetOrientation(Quaternion(0.78,Vector3(1,1,0)));
		//newChild->SetPosition(newChild->GetPosition()+Vector3(0, -1.4, 0));
		//newChild->SetRotation(Matrix4::rotateAngle(Vector3(1, 0, 0), 45));
	}
	else
	{
		newChild->SetPosition(Vector3(idCounter * 3, 0, 0));
	}
	*/
	Material* newMaterial = new Material();

	newChild->AssignMesh(GraphicsStorage::meshes[name]);
	newMaterial->AssignTexture(GraphicsStorage::textures.at(0));
	GraphicsStorage::materials.push_back(newMaterial);
	newChild->AssignMaterial(newMaterial);
	return newChild;
}

void Scene::addRandomObjects(int num)
{
	for(int i = 0; i < num; i++)
    {
	    addRandomObject();
    }
}

void Scene::addRandomlyPhysicObjects(const char* name, int num)
{
	for (int i = 0; i < num; i++)
	{
		PhysicsManager::Instance()->AddObject(addRandomlyObject(name));
	}
}

void Scene::Clear()
{
	for (auto& obj : Scene::Instance()->objectsToRender)
	{
		delete obj.second;
	}
	Scene::Instance()->objectsToRender.clear();
	SceneObject->node.children.clear();
	idCounter = 0;
}
