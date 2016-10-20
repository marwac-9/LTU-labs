#pragma once
#include "DebugDraw.h"
#include "Object.h"
#include "Node.h"
#include "GraphicsStorage.h"
#include "Material.h"
#include "Frustum.h"

using namespace mwm;

DebugDraw::DebugDraw()
{
}

DebugDraw::~DebugDraw()
{
}

DebugDraw* DebugDraw::Instance()
{
	static DebugDraw instance;

	return &instance;
}

void DebugDraw::LoadPrimitives()
{
	debugMat = new Material();
	debugMat->AssignTexture(GraphicsStorage::textures.at(0));

	Object* newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["tetra"]);
	debugShapes["tetra"] = newObject;

	newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["pyramid"]);
	debugShapes["pyramid"] = newObject;

	newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["cube"]);
	debugShapes["cube"] = newObject;

	newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["sphere"]);
	debugShapes["sphere"] = newObject;

	newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["icosphere"]);
	debugShapes["icosphere"] = newObject;

	newObject = new Object();
	newObject->AssignMaterial(debugMat);
	newObject->AssignMesh(GraphicsStorage::meshes["unitCube"]);
	debugShapes["unitCube"] = newObject;
}

Object* DebugDraw::AddObjectAtPos(const Vector3& pos, Mesh* mesh)
{
	Object* object = new Object();

	object->AssignMesh(mesh);
	object->AssignMaterial(debugMat);
	object->mat->SetColor(0, 0.8f, 0.8f);
	debugObjectsToDraw.push_back(object);
	return object;
}

void DebugDraw::Draw(const Matrix4& Projection,	const Matrix4& View)
{
	for (auto& obj : debugObjectsToDraw)
	{
		if (FrustumManager::Instance()->isBoundingSphereInView(obj->GetPosition(), obj->radius)) {
			obj->draw(Projection, View);
		}
	}
}

void DebugDraw::DrawShapeAtPos(const char* shapeName, const Vector3& pos)
{
	Object* shape = DebugDraw::Instance()->debugShapes[shapeName];
	shape->SetPosition(pos);
	shape->SetScale(0.5f, 0.5f, 0.5f);
	shape->node.UpdateNodeMatrix(Matrix4::identityMatrix());
	shape->draw(*Projection, *View);
}

void DebugDraw::DrawLine(const Vector3& normal, const Vector3& position, float width)
{
	Matrix4 model = Matrix4::translate(position);
	Vector3 axis = Vector3(0.f, 0.f, 1.f).crossProd(normal);
	float tetha = acos(normal.z);
	if (axis.squareMag() < 0.0001f)
	{
		axis = Vector3(1.f, 0.f, 0.f);
	}
	if (tetha != 0)
	{
		float  deg = (tetha * 180) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	
	line.Draw(model, *View, *Projection, width);
}


void DebugDraw::DrawNormal(const Vector3& normal, const Vector3& position, float width /*= 4.f*/)
{
	Matrix4 model = Matrix4::translate(position);
	Vector3 axis = Vector3(0.f, 0.f, 1.f).crossProd(normal);
	float tetha = acos(normal.z);
	if (axis.squareMag() < 0.0001f)
	{
		axis = Vector3(1.f, 0.f, 0.f);
	}
	if (tetha != 0)
	{
		float  deg = (tetha * 180) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}

	line.Draw(model, *View, *Projection, width);
	point.Draw(model, *View, *Projection);
}

void DebugDraw::DrawPlane(const Vector3& normal, const Vector3& position, const Vector3& halfExtent)
{
	
	Matrix4 model = Matrix4::translate(position);
	Vector3 axis = Vector3(0.f,0.f,1.f).crossProd(normal);
	float tetha = acos(normal.z);
	if (axis.squareMag() < 0.0001f)
	{
		axis = Vector3(1.f, 0.f, 0.f);
	}
	if (tetha != 0)
	{
		float  deg = (tetha * 180) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	model = Matrix4::scale(halfExtent) * model;
	plane.Draw(model, *View, *Projection);
}


void DebugDraw::DrawPlaneN(const Vector3& normal, const Vector3& position, const Vector3& halfExtent /*= Vector3(1, 1, 1)*/)
{
	Matrix4 model = Matrix4::translate(position);
	Vector3 axis = Vector3(0.f, 0.f, 1.f).crossProd(normal);
	float tetha = acos(normal.z);
	if (axis.squareMag() < 0.0001f)
	{
		axis = Vector3(1.f, 0.f, 0.f);
	}
	if (tetha != 0)
	{
		float  deg = (tetha * 180) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	model = Matrix4::scale(halfExtent) * model;
	plane.Draw(model, *View, *Projection);
	line.Draw(model, *View, *Projection);
	point.Draw(model, *View, *Projection);
}


void DebugDraw::DrawPoint(const Vector3& position, float size)
{
	point.Draw(Matrix4::translate(position), *View, *Projection, size);
}

void DebugDraw::DrawCrossHair(int windowWidth, int windowHeight, const Vector3& color)
{
	float scale = 20;
	float offset = scale / 2.f;
	float x = windowWidth / 2.f;
	float y = windowHeight / 2.f;
	Matrix4 model1 = Matrix4::translate(x, y + offset, 0);
	Matrix4 model2 = Matrix4::translate(x - offset, y, 0);

	model1 = Matrix4::rotateAngle(Vector3(1.f, 0.f, 0.f), 90)*model1;
	model2 = Matrix4::rotateAngle(Vector3(0.f, 1.f, 0.f), 90)*model2;
	Matrix4 scaleM = Matrix4::scale(scale, scale, scale);
	model1 = scaleM * model1;
	model2 = scaleM * model2;
	Matrix4 view = Matrix4::identityMatrix();
	Matrix4 proj = Matrix4::orthographicTopToBottom(-1, 2000, 0, windowWidth, windowHeight, 0);
	line.mat->color = color;
	line.Draw(model1, view, proj, 2.f);
	line.Draw(model2, view, proj, 2.f);
}
