//
// Created by marwac-9 on 9/16/15.
//

#include "config.h"
#include "SubdivApp.h"
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "GraphicsManager.h"
#include "GraphicsStorage.h"
#include "Node.h"
#include "Material.h"
#include "Mesh.h"
#include "OBJ.h"
#include "HalfEdgeMesh.h"
#include <fstream>
#include "Scene.h"
#include "ShaderManager.h"
#include <string>
#include "DebugDraw.h"
#include "PhysicsManager.h"

using namespace mwm;
using namespace Display;

namespace Subdivision
{

	//------------------------------------------------------------------------------
	/**
	*/
	SubdivisionApp::SubdivisionApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	SubdivisionApp::~SubdivisionApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	bool
		SubdivisionApp::Open()
	{
		App::Open();
		this->window = new Display::Window;
		glfwSwapInterval(0);
		window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mode)
		{
			KeyCallback(key, scancode, action, mode);
		});

		// window resize callback
		this->window->SetWindowSizeFunction([this](int width, int height)
		{
			this->windowWidth = width;
			this->windowHeight = height;
			this->window->SetSize(this->windowWidth, this->windowHeight);
			float aspect = (float)this->windowWidth / (float)this->windowHeight;
			this->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, 0.1f, 100.f);
		});

		if (this->window->Open())
		{
			running = true;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void
		SubdivisionApp::Run()
	{

		InitGL();
		GraphicsManager::LoadAllAssets();

		Object* Scene = Scene::Instance()->build();
		LoadScene1();
		Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());

		// For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowWidth / 2.f, windowHeight / 2.f + 100);
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		cam.SetInitPos(Vector3(0.f, 1.f, 6.f));
		cam.computeViewFromInput(this->window, 1, 0.016f);

		double fps_timer = 0;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		while (running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window->Update();

			// Measure FPS
			double currentTime = glfwGetTime();
			double deltaTime = currentTime - lastTime;
			//is cursor window locked
			if (altButtonToggle)
			{
				// Compute the view matrix from keyboard and mouse input
				cam.computeViewFromInput(this->window, 1, deltaTime);
			}
			ViewMatrix = cam.getViewMatrix();
			FrustumManager::Instance()->ExtractPlanes(ProjectionMatrix, ViewMatrix);
			Monitor(this->window, ViewMatrix);

			Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());

			Draw(ProjectionMatrix, ViewMatrix);

			if (currentTime - fps_timer >= 0.2f){
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " FPS: " + std::to_string(1.f / deltaTime));
				fps_timer = currentTime;
			}

			this->window->SwapBuffers();

			lastTime = currentTime;
		}
		this->ClearBuffers();
		GraphicsStorage::ClearMaterials();
		this->window->Close();
	}


	void
		SubdivisionApp::ClearBuffers()
	{
		//clean up buffers
		for (auto& mesh : GraphicsStorage::meshes)
		{
			glDeleteBuffers(1, &mesh.second->vertexbuffer);
			glDeleteBuffers(1, &mesh.second->uvbuffer);
			glDeleteBuffers(1, &mesh.second->normalbuffer);
			glDeleteBuffers(1, &mesh.second->elementbuffer);
			glDeleteBuffers(1, &mesh.second->vaoHandle);
		}

		//clean up textures
		for (auto& texture : GraphicsStorage::textures)
		{
			glDeleteBuffers(1, &texture->TextureID);
		}

		ShaderManager::Instance()->DeleteShaders();
	}

	void
		SubdivisionApp::KeyCallback(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			running = false;
		}
		//this is just used to show hide cursor, mouse steal on/off
		else if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
			if (altButtonToggle) {
				altButtonToggle = false;
				window->SetCursorMode(GLFW_CURSOR_NORMAL);
			}
			else {
				altButtonToggle = true;
				window->SetCursorPos(windowWidth / 2.f, windowHeight / 2.f);
				window->SetCursorMode(GLFW_CURSOR_DISABLED);
			}
		}
		else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
			LoadScene1();
		}
		else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
			LoadScene2();
		}
		else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
			LoadScene3();
		}
		else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
			printf("\nWIREFRAME MODE\n");
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
			printf("\nSHADED MODE\n");
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (key == GLFW_KEY_S && window->GetKey(GLFW_KEY_LEFT_CONTROL) && action == GLFW_PRESS) {
			GraphicsManager::SaveToOBJ(GraphicsStorage::objects.back());
			std::cout << "Last Mesh Saved" << std::endl;
		}
		else if (key == GLFW_KEY_F5 && action == GLFW_PRESS)
		{
			if (DebugDraw::Instance()->debug) DebugDraw::Instance()->debug = false;
			else DebugDraw::Instance()->debug = true;
		}

		else if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			Object* cube = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, 8.f, 0.f));
			cube->SetPosition(Vector3(0.f, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
		}
	}

	void
		SubdivisionApp::Monitor(Display::Window* window, Matrix4& ViewMatrix)
	{

	}

	void
		SubdivisionApp::InitGL()
	{

		// grey background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Cull triangles which normal is not towards the camera
		glEnable(GL_CULL_FACE);

		ShaderManager::Instance()->LoadShaders();
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
		LightID = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightPosition_worldspace");
		GLuint LightDir = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightInvDirection_worldspace");
		glUniform3f(LightID, 0.f, 0.f, 0.f);

		this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, 0.1f, 200.0f);
	}

	void
		SubdivisionApp::Draw(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetPosition(), obj.second->radius)) {
				obj.second->draw(ProjectionMatrix, ViewMatrix);
				objectsRendered++;
			}
		}
	}

	void
		SubdivisionApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		ClearSubdivisionData();
	}


	void SubdivisionApp::ClearSubdivisionData()
	{
		for (auto& obj : dynamicOBJs)
		{
			delete obj;
		}
		dynamicOBJs.clear();
		for (auto& obj : dynamicMeshes)
		{
			delete obj;
		}
		dynamicMeshes.clear();
		for (auto& obj : dynamicHEMeshes)
		{
			delete obj;
		}
		dynamicHEMeshes.clear();
	}

	void
		SubdivisionApp::LoadScene1()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[5]);
	}

	void SubdivisionApp::LoadScene2()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[1]);
	}

	void SubdivisionApp::LoadScene3()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[2]);
	}

	void SubdivisionApp::Subdivide(OBJ* objToSubdivide)
	{
		Object* HalfMesh = Scene::Instance()->addChild(Scene::Instance()->SceneObject); //Object added to scene for rendering
		
		printf("\nCreating half edge mesh\n");
		HalfEdgeMesh* newHMesh = new HalfEdgeMesh();
		OBJ* newOBJ = new OBJ();
		dynamicOBJs.push_back(newOBJ);
		dynamicHEMeshes.push_back(newHMesh);

		newHMesh->Construct(*objToSubdivide);
		HalfEdgeMesh::ExportMeshToOBJ(newHMesh, newOBJ);  // HE_Mesh -> OBJ
		GraphicsManager::LoadOBJToVBO(newOBJ, newHMesh);  // OBJ -> Mesh
		newOBJ->CalculateDimensions();

		HalfMesh->AssignMesh(newHMesh);
		HalfMesh->AssignMaterial(GraphicsStorage::materials[0]);
		HalfMesh->SetScale(4.0f, 4.0f, 4.0f);
		HalfMesh->mat->SetAmbientIntensity(0.5f);
		HalfMesh->setRadius(4);

		Object* HalfMeshProxy = Scene::Instance()->addChild(HalfMesh);

		Mesh* proxyMesh = new Mesh();
		GraphicsManager::LoadOBJToVBO(newOBJ, proxyMesh); //proxy will use same OBJ
		dynamicMeshes.push_back(proxyMesh);

		HalfMeshProxy->AssignMesh(proxyMesh);
		HalfMeshProxy->AssignMaterial(GraphicsStorage::materials[0]);
		HalfMeshProxy->setRadius(4);
		printf("\nDONE\n");

		for (int i = 0; i < 5; i++)
		{
			printf("\nLET'S SUBDIVIDE STAGE%d\n", i+1);
			newHMesh->Subdivide();
			printf("\nDONE\n");
		}

		OBJ* subdividedOBJ = new OBJ(); //we need new renderable OBJ for subdivided half-edge mesh while proxy is still using the first generated OBJ
		dynamicOBJs.push_back(subdividedOBJ);
		HalfEdgeMesh::ExportMeshToOBJ(newHMesh, subdividedOBJ);
		GraphicsManager::LoadOBJToVBO(subdividedOBJ, newHMesh);
		
		//must create containers for dynamic meshes and OBJ and delete them when clearing
	}

} // namespace Example