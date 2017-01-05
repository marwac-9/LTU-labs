#pragma once
#include <map>
class Object;

class Scene
{
public:
	static Scene* Instance();
	int idCounter;
	Object* addChildTo(Object* parentNodeGR);
	std::map<int, Object*> objectsToRender;
	Object* SceneObject;
	Object* LastAddedObject;
	Object* build();
private:
	Scene();
	~Scene();
	//copy
	Scene(const Scene&);
	//assign
	Scene& operator=(const Scene&);
};