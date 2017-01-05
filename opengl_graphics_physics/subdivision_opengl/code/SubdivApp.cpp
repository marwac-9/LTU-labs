//
// Created by marwac-9 on 9/16/15.
//

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
#include "Render.h"
#include "CameraManager.h"
#include <chrono>

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
			currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, 0.1f, 100.f);
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

		LoadScene1();
		
		// For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY + 100);
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		double fps_timer = 0;
		Node initNode = Node();
		Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframe = true;
		//glfwSwapInterval(0); //unlock fps

		while (running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window->Update();

			// Measure FPS
			Time::currentTime = glfwGetTime();
			Time::deltaTime = Time::currentTime - lastTime;

			Monitor(this->window);

			//is cursor window locked
			if (altButtonToggle) CameraManager::Instance()->Update();
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);
			

			Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);

			Draw();

			if (Time::currentTime - fps_timer >= 0.2){
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " FPS: " + std::to_string(1.0 / Time::deltaTime));
				fps_timer = Time::currentTime;
			}

			this->window->SwapBuffers();

			lastTime = Time::currentTime;
		}
		this->ClearBuffers();
		GraphicsStorage::ClearMaterials();
		GraphicsStorage::ClearOBJs();
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
		glClearColor(0.f, 0.f, 0.f, 1.0f);

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
	}

	void
	SubdivisionApp::Draw()
	{
		Matrix4F View = currentCamera->getViewMatrix().toFloat();
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View[0][0]);

		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, CameraManager::Instance()->ViewProjection, currentShaderID);
				objectsRendered++;
			}
		}
	}

	void
	SubdivisionApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
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
		Object* ObjectHalfMesh = Scene::Instance()->addChildTo(Scene::Instance()->SceneObject); //Object added to scene for rendering
		
		printf("\nConstructing half edge mesh\n");
		HalfEdgeMesh* newHMesh = new HalfEdgeMesh();
		OBJ* constructedOBJ = new OBJ();
		dynamicOBJs.push_back(constructedOBJ);

		newHMesh->Construct(*objToSubdivide);
		HalfEdgeMesh::ExportMeshToOBJ(newHMesh, constructedOBJ);  // HE_Mesh -> OBJ
		constructedOBJ->CalculateDimensions();

		printf("\nDONE\n");

		std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
		start = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds; 
		for (int i = 0; i < 5; i++)
		{
			printf("\nLET'S SUBDIVIDE STAGE%d\n", i+1);
			newHMesh->Subdivide();

			end = std::chrono::high_resolution_clock::now();
			elapsed_seconds = end - start;

			std::cout << "\nPass " << i + 1 << " subdivided in: " << elapsed_seconds.count() << "s\n";
			
		}
		printf("\nDONE\n");

		printf("\nExporting subdivided mesh\n");
		OBJ* subdividedOBJ = new OBJ(); //we need new renderable OBJ for subdivided half-edge mesh while proxy will still use the first generated OBJ
		dynamicOBJs.push_back(subdividedOBJ);
		HalfEdgeMesh::ExportMeshToOBJ(newHMesh, subdividedOBJ);
		GraphicsManager::LoadOBJToVBO(subdividedOBJ, newHMesh);
		dynamicHEMeshes.push_back(newHMesh);
		
		ObjectHalfMesh->AssignMesh(newHMesh); //we assign the subdivided and exported mesh
		Material* newMaterial = new Material();
		newMaterial->AssignTexture(GraphicsStorage::textures.at(0));
		GraphicsStorage::materials.push_back(newMaterial);
		ObjectHalfMesh->AssignMaterial(newMaterial);
		ObjectHalfMesh->SetScale(Vector3(4.0f, 4.0f, 4.0f));
		ObjectHalfMesh->SetPosition(Vector3(0.f, 0.f, -10.f));
		ObjectHalfMesh->mat->SetAmbientIntensity(0.5f);
		printf("\nDONE\n");

		printf("\nCreating proxy mesh\n");
		Object* ObjectHalfMeshProxy = Scene::Instance()->addChildTo(ObjectHalfMesh); //we create the object for proxy

		Mesh* proxyMesh = new Mesh(); //and a new mesh for proxy
		GraphicsManager::LoadOBJToVBO(constructedOBJ, proxyMesh); //proxy will use the originally generated OBJ from constructed half edge mesh
		dynamicMeshes.push_back(proxyMesh);

		ObjectHalfMeshProxy->AssignMesh(proxyMesh); //and we assign now generated proxy mesh from constructedOBJ to the proxy Object
		Material* newMaterialProxy = new Material();
		newMaterialProxy->SetColor(Vector3(1.f, 0.f, 0.f));
		newMaterialProxy->AssignTexture(GraphicsStorage::textures.at(0));
		GraphicsStorage::materials.push_back(newMaterialProxy);
		ObjectHalfMeshProxy->AssignMaterial(newMaterialProxy);
		printf("\nDONE\n");
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
	SubdivisionApp::SetUpCamera()
	{
		currentCamera = new Camera(Vector3(0.f, 3.f, 16.f), windowWidth, windowHeight);
		currentCamera->Update((float)Time::timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, 0.1f, 200.0f);
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->getViewMatrix();
	}

	void
	SubdivisionApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
	}

} // namespace Example