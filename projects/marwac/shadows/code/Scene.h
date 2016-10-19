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
    Object* SceneObject;
    Object* LastAddedObject;
    Object* build();
    void addRandomObject();
	Object* addObject(const char* name = "cube", const mwm::Vector3& pos = mwm::Vector3());
	Object* addPhysicObject(const char* name = "cube", const mwm::Vector3& pos = mwm::Vector3());
	Object* addRandomlyObject(const char* name);
    void addRandomObjects(int numberOfSpheres);

	void addRandomlyPhysicObjects(const char* name, int numberOfSpheres);

	void Clear();
private:
    Scene();
    ~Scene();
    //copy
    Scene(const Scene&);
    //assign
    Scene& operator=(const Scene&);
};