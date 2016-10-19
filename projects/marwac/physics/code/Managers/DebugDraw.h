#pragma once
#include "MyMathLib.h"
#include <vector>
#include <map>
#include <string>
#include "BoundingBox.h"
#include "Line.h"
#include "Plane.h"
#include "Point.h"

class Object;
class Mesh;
class Material;

class DebugDraw
{
public:
	static DebugDraw* Instance();
	Object* AddObjectAtPos(const mwm::Vector3& pos, Mesh* mesh);
	void DrawShapeAtPos(const char* shapeName, const mwm::Vector3& pos);
	void DrawLine(const mwm::Vector3& normal, const mwm::Vector3& position, float width = 4.f);
	void DrawPlane(const mwm::Vector3& normal, const mwm::Vector3& position, const mwm::Vector3& halfExtent = mwm::Vector3(1.f, 1.f, 1.f));
	void DrawPlaneN(const mwm::Vector3& normal, const mwm::Vector3& position, const mwm::Vector3& halfExtent = mwm::Vector3(1.f, 1.f, 1.f));
	void DrawPoint(const mwm::Vector3& position, float size = 10.f);
	void DrawNormal(const mwm::Vector3& normal, const mwm::Vector3& position, float width = 4.f);
	void DrawCrossHair(int windowWidth, int windowHeight, const mwm::Vector3& color = mwm::Vector3(1.f, 1.f, 0.f));
	std::vector<Object*> debugObjectsToDraw;
	Material* debugMat;
	
	mwm::Matrix4* Projection;
	mwm::Matrix4* View;
	void LoadPrimitives();
	BoundingBox boundingBox;
	Line line;
	Plane plane;
	Point point;
	bool debug = false;
private:
	void Draw(const mwm::Matrix4& Projection, const mwm::Matrix4& View);
	DebugDraw();
	~DebugDraw();
	//copy
	DebugDraw(const DebugDraw&);
	//assign
	DebugDraw& operator=(const DebugDraw&);
	
	std::map<std::string, Object*> debugShapes;
};

