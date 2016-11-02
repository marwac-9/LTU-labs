#pragma once
#include "DebugDraw.h"
#include "Object.h"
#include "Node.h"
#include "GraphicsStorage.h"
#include "Material.h"
#include "Mesh.h"
#include "ShaderManager.h"
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
		float  deg = (tetha * 180.f) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	
	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	line.Draw(model, *View, *Projection, wireframeShader, width);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
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
		float  deg = (tetha * 180.f) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	line.Draw(model, *View, *Projection, wireframeShader, width);
	point.Draw(model, *View, *Projection, wireframeShader);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
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
		float  deg = (tetha * 180.f) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	model = Matrix4::scale(halfExtent) * model;

	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	plane.Draw(model, *View, *Projection, wireframeShader);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
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
		float  deg = (tetha * 180.f) / 3.14159f;
		model = Matrix4::rotateAngle(axis, deg)*model;
	}
	model = Matrix4::scale(halfExtent) * model;

	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	plane.Draw(model, *View, *Projection, wireframeShader);
	line.Draw(model, *View, *Projection, wireframeShader);
	point.Draw(model, *View, *Projection, wireframeShader);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
}


void DebugDraw::DrawPoint(const Vector3& position, float size)
{
	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	point.Draw(Matrix4::translate(position), *View, *Projection, wireframeShader, size);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
}

void DebugDraw::DrawCrossHair(int windowWidth, int windowHeight, const Vector3& color)
{
	float scale = 20.f;
	float offset = scale / 2.f;
	float x = windowWidth / 2.f;
	float y = windowHeight / 2.f;
	Matrix4 model1 = Matrix4::translate(x, y + offset, 0.f);
	Matrix4 model2 = Matrix4::translate(x - offset, y, 0.f);

	model1 = Matrix4::rotateAngle(Vector3(1.f, 0.f, 0.f), 90.f)*model1;
	model2 = Matrix4::rotateAngle(Vector3(0.f, 1.f, 0.f), 90.f)*model2;
	Matrix4 scaleM = Matrix4::scale(scale, scale, scale);
	model1 = scaleM * model1;
	model2 = scaleM * model2;
	Matrix4 view = Matrix4::identityMatrix();
	Matrix4 proj = Matrix4::orthographicTopToBottom(-1.f, 2000.f, 0.f, (float)windowWidth, (float)windowHeight, 0.f);
	line.mat->color = color;

	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	ShaderManager::Instance()->SetCurrentShader(wireframeShader);
	line.Draw(model1, view, proj, wireframeShader, 2.f);
	line.Draw(model2, view, proj, wireframeShader, 2.f);
	ShaderManager::Instance()->SetCurrentShader(prevShader);
}

void DebugDraw::DrawQuad()
{
	//bind vao before drawing
	glBindVertexArray(plane.mesh->vaoHandle);
	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, plane.mesh->indicesSize, GL_UNSIGNED_SHORT, (void*)0); // mode, count, type, element array buffer offset
}