#pragma once
#include <map>
#include <vector>
#include "Vector3.h"
class Object;
class Scene
{
public:
	static Scene* Instance();
	int idCounter;
	Object* addChild(Object* parentNodeGR);
	std::map<int, Object*> objectsToRender;
	std::vector<Object*> pointLights;
	std::vector<Object*> directionalLights;
	Object* SceneObject;
	Object* MainPointLight;
	Object* MainDirectionalLight;
	Object* LastAddedObject;
	Object* build();
	void addRandomObject();
	Object* addObject(const char* name = "cube", const mwm::Vector3& pos = mwm::Vector3());
	Object* addPhysicObject(const char* name = "cube", const mwm::Vector3& pos = mwm::Vector3());
	Object* addRandomlyObject(const char* name);
	void addRandomObjects(int num);
	void addRandomlyObjects(const char* name, int num);
	Object* addPointLight(const mwm::Vector3& position, const mwm::Vector3& color = mwm::Vector3(1.f, 1.f, 1.f));
	void addRandomPointLight();
	Object* addDirectionalLight(const mwm::Vector3& direction, const mwm::Vector3& color = mwm::Vector3(1.f, 1.f, 1.f));
	void addRandomlyPointLights(int num);
	void addRandomlyPhysicObjects(const char* name, int numberOfSpheres);
	mwm::Vector3 generateRandomIntervallVector(int min, int max);

	void Clear();
private:
	Scene();
	~Scene();
	//copy
	Scene(const Scene&);
	//assign
	Scene& operator=(const Scene&);
};