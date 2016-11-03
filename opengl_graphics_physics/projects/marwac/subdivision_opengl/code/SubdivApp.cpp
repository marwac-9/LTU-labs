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
#include "Camera.h"
#include "Frustum.h"

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

		window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mode)
		{
			KeyCallback(key, scancode, action, mode);
		});

		window->SetMouseMoveFunction([this](double mouseX, double mouseY)
		{
			MouseCallback(mouseX, mouseY);
		});

		// window resize callback
		this->window->SetWindowSizeFunction([this](int width, int height)
		{
			this->windowWidth = width;
			this->windowHeight = height;
			this->windowMidX = windowWidth / 2.0f;
			this->windowMidY = windowHeight / 2.0f;
			this->window->SetSize(this->windowWidth, this->windowHeight);
			float aspect = (float)this->windowWidth / (float)this->windowHeight;
			this->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, 0.1f, 100.f);

			currentCamera->UpdateSize(width, height);
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
		window->SetCursorPos(windowMidX, windowMidY + 100);
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera(0.016f);

		double fps_timer = 0;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframe = true;
		//glfwSwapInterval(0); //unlock fps

		while (running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window->Update();

			// Measure FPS
			double currentTime = glfwGetTime();
			double deltaTime = currentTime - lastTime;

			Monitor(this->window);

			//is cursor window locked
			if (altButtonToggle)
			{
				// Compute the view matrix from keyboard and mouse input
				currentCamera->Update((float)deltaTime);
			}
			ViewMatrix = currentCamera->getViewMatrix();
			FrustumManager::Instance()->ExtractPlanes(ProjectionMatrix, ViewMatrix);
			

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
		GraphicsStorage::ClearMeshes();
		GraphicsStorage::ClearTextures();
		ShaderManager::Instance()->DeleteShaders();
	}

	void
	SubdivisionApp::KeyCallback(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_ESCAPE) {
				running = false;
			}
			//this is just used to show hide cursor, mouse steal on/off
			else if (key == GLFW_KEY_LEFT_ALT) {
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
			else if (key == GLFW_KEY_1) {
				LoadScene1();
			}
			else if (key == GLFW_KEY_2) {
				LoadScene2();
			}
			else if (key == GLFW_KEY_3) {
				LoadScene3();
			}
			else if (key == GLFW_KEY_TAB) {
				if (wireframe)
				{
					wireframe = false;
					printf("\nSHADED MODE\n");
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				else
				{
					wireframe = true;
					printf("\nWIREFRAME MODE\n");
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
			}
			else if (key == GLFW_KEY_S && window->GetKey(GLFW_KEY_LEFT_CONTROL)) {
				GraphicsManager::SaveToOBJ(GraphicsStorage::objects.back());
				std::cout << "Last Mesh Saved" << std::endl;
			}
			else if (key == GLFW_KEY_F5)
			{
				if (DebugDraw::Instance()->debug) DebugDraw::Instance()->debug = false;
				else DebugDraw::Instance()->debug = true;
			}

			else if (key == GLFW_KEY_E)
			{
				Object* cube = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, 8.f, 0.f));
				cube->SetPosition(Vector3(0.f, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
			}
		}
	}

	void
	SubdivisionApp::Monitor(Display::Window* window)
	{
		currentCamera->holdingForward = (window->GetKey(GLFW_KEY_W) == GLFW_PRESS);
		currentCamera->holdingBackward = (window->GetKey(GLFW_KEY_S) == GLFW_PRESS);
		currentCamera->holdingRight = (window->GetKey(GLFW_KEY_D) == GLFW_PRESS);
		currentCamera->holdingLeft = (window->GetKey(GLFW_KEY_A) == GLFW_PRESS);
		currentCamera->holdingUp = (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS);
		currentCamera->holdingDown = (window->GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
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

		LoadShaders();
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
		LightID = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightPosition_worldspace");
		GLuint LightDir = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightInvDirection_worldspace");
		glUniform3f(LightID, 0.f, 0.f, 0.f);

		this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
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

	void
	SubdivisionApp::ClearSubdivisionData()
	{
		for (auto& obj : dynamicOBJs)
		{
			delete obj;
		}
		dynamicOBJs.clear();
		for (auto& mesh : dynamicMeshes)
		{
			delete mesh;
		}
		dynamicMeshes.clear();
		for (auto& mesh : dynamicHEMeshes)
		{
			delete mesh;
		}
		dynamicHEMeshes.clear();
	}

	void
	SubdivisionApp::LoadScene1()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[5]);
	}

	void
	SubdivisionApp::LoadScene2()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[1]);
	}

	void
	SubdivisionApp::LoadScene3()
	{
		Clear();
		Subdivide(GraphicsStorage::objects[2]);
	}

	void
	SubdivisionApp::Subdivide(OBJ* objToSubdivide)
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

	void
	SubdivisionApp::MouseCallback(double mouseX, double mouseY)
	{
		if (altButtonToggle)
		{
			currentCamera->UpdateOrientation(mouseX, mouseY);
			window->SetCursorPos(windowMidX, windowMidY);
		}
	}

	void
	SubdivisionApp::SetUpCamera(float timeStep)
	{
		currentCamera = new Camera(Vector3(0.f, 3.f, 16.f), windowWidth, windowHeight);
		currentCamera->Update(timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
	}

	void
	SubdivisionApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
	}

} // namespace Example