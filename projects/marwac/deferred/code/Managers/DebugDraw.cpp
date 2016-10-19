#pragma once
#include "DebugDraw.h"
#include "Object.h"
#include "Node.h"
#include "GraphicsStorage.h"
#include "Material.h"
#include "Frustum.h"
#include "FBOManager.h"
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
	line.Draw(model1, view, proj, 2.f);
	line.Draw(model2, view, proj, 2.f);
}

void DebugDraw::DrawShadowMap(int width, int height)
{
	//Quad render

	ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["depthPanel"]);
	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	float fHeight = (float)height;
	float fWidth = (float)width;
	int y = (int)(fHeight - fHeight*0.20f);
	int glWidth = (int)(fWidth *0.15f);
	int glHeight = (int)(fHeight*0.20f);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, y, glWidth, glHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, y, glWidth, glHeight);


	//GLuint ShadowMapHandle = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "shadowMapSampler");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapHandle);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	//glUniform1i(ShadowMapHandle, 0);

	DrawQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, width, height);
}

void DebugDraw::DrawGeometryMaps(int width, int height)
{
	ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["depthPanel"]);
	//GLuint ShadowMapHandle = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "shadowMapSampler");
	//glUniform1i(ShadowMapHandle, 0);
	float fHeight = (float)height;
	float fWidth = (float)width;
	int y = (int)(fHeight*0.20f);
	int glWidth = (int)(fWidth *0.15f);
	int glHeight = (int)(fHeight*0.20f);

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, y, glWidth, glHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, y, glWidth, glHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->positionBufferHandle);
	DrawQuad();

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, glWidth, glHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, glWidth, glHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->diffuseBufferHandle);
	DrawQuad();

	glEnable(GL_SCISSOR_TEST);
	glScissor(glWidth, 0, glWidth, glHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(glWidth, 0, glWidth, glHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->normalBufferHandle);
	DrawQuad();
	/*
	glEnable(GL_SCISSOR_TEST);
	glScissor(width *0.15, height*0.20, width *0.15, height*0.20);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(width *0.15, height*0.20, width *0.15, height*0.20);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->texcoordBufferHandle);
	DrawQuad();
	*/
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, width, height);
}

void DebugDraw::DrawQuad()
{
	//bind vao before drawing
	glBindVertexArray(plane.mesh->vaoHandle);
	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, plane.mesh->indicesSize, GL_UNSIGNED_SHORT, (void*)0); // mode, count, type, element array buffer offset
}