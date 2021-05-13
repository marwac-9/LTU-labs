//
// Created by marwac-9 on 9/16/15.
//

#include "PickingApp.h"
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "GraphicsManager.h"
#include "GraphicsStorage.h"
#include "Node.h"
#include "Material.h"
#include "Texture.h"
#include "OBJ.h"
#include <fstream>
#include "SceneGraph.h"
#include "ShaderManager.h"
#include <string>
#include "DebugDraw.h"
#include "PhysicsManager.h"
#include "FBOManager.h"
#include "Camera.h"
#include "Frustum.h"
#include "Render.h"
#include "RigidBody.h"
#include "CameraManager.h"
#include "FrameBuffer.h"
#include "Times.h"
#include "BoundingBox.h"
#include "ImGuiWrapper.h"
#include <imgui.h>
#include "DirectionalLight.h"

using namespace Display;
namespace Picking
{

//------------------------------------------------------------------------------
/**
*/
    PickingApp::PickingApp()
    {
        // empty
    }

//------------------------------------------------------------------------------
/**
*/
    PickingApp::~PickingApp()
    {
        // empty
    }

//------------------------------------------------------------------------------
/**
*/
    bool
    PickingApp::Open()
    {
        App::Open();
        this->window = new Display::Window;

        window->SetKeyPressFunction([this](int32 key , int32 scancode, int32 action, int32 mode)
        {
            KeyCallback(key,scancode,action,mode);
        });

		window->SetMouseMoveFunction([this](double mouseX, double mouseY)
		{
			MouseCallback(mouseX, mouseY);
		});

        // window resize callback
        this->window->SetWindowSizeFunction([this](int width, int height)
        {
			if (!minimized)
			{
				this->windowWidth = width;
				this->windowHeight = height;
				this->windowMidX = windowWidth / 2.0f;
				this->windowMidY = windowHeight / 2.0f;
				this->window->SetSize(this->windowWidth, this->windowHeight);
				FBOManager::Instance()->UpdateTextureBuffers(this->windowWidth, this->windowHeight);
				currentCamera->UpdateSize(width, height);
			}
        });

        this->window->SetMousePressFunction([this](int button, int action, int mods)
        {
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                isLeftMouseButtonPressed = true;
            }
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            {
                isLeftMouseButtonPressed = false;
            }
			else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			{

			}
			else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			{
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
			{
				SceneGraph::Instance()->InitializeSceneTree();
				GraphicsManager::ReloadShaders();
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
			{
			}

        });
		window->SetWindowIconifyFunction([this](int iconified) {
			if (iconified) minimized = true;
			else minimized = false;
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
    PickingApp::Run()
    {
        InitGL();

		SetUpBuffers(this->windowWidth, this->windowHeight);

		GraphicsManager::LoadAllAssets();

		Render::Instance()->InitializeShderBlockDatas();

		ImGuiWrapper ImGuiWrapper(window);

        // For speed computation (FPS)
		Times::Instance()->currentTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		LoadScene2();
		currentScene = scene2Loaded;

		double customIntervalTime = 0.0;
		SceneGraph::Instance()->Update();

		glfwSwapInterval(0); //unlock fps

        while (running)
        {
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glDisable(GL_BLEND);
            this->window->Update();
			if (minimized) continue;
			ImGuiWrapper.NewFrame();

			Times::Instance()->Update(glfwGetTime());

			Monitor(this->window);

			CameraManager::Instance()->Update(Times::Instance()->deltaTime);
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);

			switch (currentScene)
			{
				case scene1Loaded:
					if (customIntervalTime >= 0.2){
						SpawnSomeLights(); //have to draw on plane
						customIntervalTime = 0.0;
					}
					break;
				case scene2Loaded:
					MovePlaneUpNDown();
					break;				
			}

			SceneGraph::Instance()->Update();
			
			PhysicsManager::Instance()->Update(Times::Instance()->dtInv);
			
			if (lightsPhysics) 
			{
				if (currentScene == scene2Loaded) Vortex();
			}

			SceneGraph::Instance()->FrustumCulling();

			Render::Instance()->UpdateShaderBlockDatas();

			GenerateGUI();

			DrawPicking();

			DrawGeometryPass();

			if (altButtonToggle) PickingTest();
			
			DrawLightPass();

			BlitToScreenPass();

			if (debug) DrawDebug();

			DebugDraw::Instance()->DrawCrossHair();
			
			DrawGeometryMaps(windowWidth, windowHeight);

			ImGuiWrapper.Render();

			customIntervalTime += Times::Instance()->deltaTime;
            
            this->window->SwapBuffers();
        }
		GraphicsStorage::Clear();
        this->window->Close();
    }

    void
    PickingApp::KeyCallback(int key, int scancode, int action, int mods)
    {
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_ESCAPE) {
				running = false;
			}
			else if (key == GLFW_KEY_R) {
				//SceneGraph::Instance()->addRandomObject();
			}
			//this is just used to show hide cursor, mouse steal on/off
			else if (key == GLFW_KEY_LEFT_ALT) {
				if (altButtonToggle) {
					altButtonToggle = false;
					window->SetCursorMode(GLFW_CURSOR_NORMAL);
					currentCamera->holdingForward = false;
					currentCamera->holdingBackward = false;
					currentCamera->holdingRight = false;
					currentCamera->holdingLeft = false;
					currentCamera->holdingUp = false;
					currentCamera->holdingDown = false;
				}
				else {
					altButtonToggle = true;
					window->SetCursorPos(windowWidth / 2.f, windowHeight / 2.f);
					window->SetCursorMode(GLFW_CURSOR_DISABLED);
				}
			}
			else if (key == GLFW_KEY_1) {
				currentScene = scene1Loaded;
				LoadScene1();
			}
			else if (key == GLFW_KEY_2) {
				currentScene = scene2Loaded;
				LoadScene2();
			}
			else if (key == GLFW_KEY_3) {
				currentScene = scene3Loaded;
				LoadScene3();
			}
			else if (key == GLFW_KEY_BACKSPACE)
			{
				if (lightsPhysics) lightsPhysics = false;
				else lightsPhysics = true;
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
				auto it = --GraphicsStorage::objs.end();
				GraphicsManager::SaveToOBJ(it->second);
				std::cout << "Last Mesh Saved" << std::endl;
			}
			else if (key == GLFW_KEY_O) {
				if (debug)
				{
					debug = false;			
				}
				else
				{
					debug = true;
				}
			}
			else if (key == GLFW_KEY_P)
			{
				if (Times::Instance()->paused) Times::Instance()->paused = false;
				else Times::Instance()->paused = true;
			}
			else if (key == GLFW_KEY_T)
			{
				Times::Instance()->timeModifier = 0.0;
			}
			else if (key == GLFW_KEY_F5)
			{
				if (DebugDraw::Instance()->debug) DebugDraw::Instance()->debug = false;
				else DebugDraw::Instance()->debug = true;
			}

			else if (key == GLFW_KEY_E)
			{
				Object* cube = SceneGraph::Instance()->addPhysicObject("cube", Vector3(0.0, 8.0, 0.0));
				cube->node->SetPosition(Vector3(0.0, (double)SceneGraph::Instance()->pickingList.size() * 2.0 - 10.0 + 0.001, 0.0));
				SceneGraph::Instance()->SwitchObjectMovableMode(cube, true);
			}
		}
    }

    void
    PickingApp::Monitor(Display::Window* windowIn)
    {
		if (windowIn->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) Times::Instance()->timeModifier += 0.0005;
		if (windowIn->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) Times::Instance()->timeModifier -= 0.0005;
		if (windowIn->GetKey(GLFW_KEY_UP) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->node->Translate(Vector3(0.f, 0.05f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_DOWN) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->node->Translate(Vector3(0.f, -0.05f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_LEFT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->node->Translate(Vector3(0.05f, 0.f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->node->Translate(Vector3(-0.05f, 0.f, 0.f));

		if (altButtonToggle)
		{
			currentCamera->holdingForward = (window->GetKey(GLFW_KEY_W) == GLFW_PRESS);
			currentCamera->holdingBackward = (window->GetKey(GLFW_KEY_S) == GLFW_PRESS);
			currentCamera->holdingRight = (window->GetKey(GLFW_KEY_D) == GLFW_PRESS);
			currentCamera->holdingLeft = (window->GetKey(GLFW_KEY_A) == GLFW_PRESS);
			currentCamera->holdingUp = (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS);
			currentCamera->holdingDown = (window->GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
		}
		currentCamera->fov = fov;
		currentCamera->near = near;
		currentCamera->far = far;
    }

	void
	PickingApp::GenerateGUI()
	{
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::NewLine();
		if (dirLightComp != nullptr)
		{
			ImGui::SliderFloat("DirDif", &dirLightComp->properties.diffuse, 0.f, 100.f);
			ImGui::SliderFloat("DirSpec", &dirLightComp->properties.specular, 0.f, 100.f);
		}
		if (planeObject != nullptr)
		{
			ImGui::SliderFloat("PlaneShi", &planeObject->materials[0]->shininess, 0.f, 100.f);
		}

		ImGui::Text("Objects rendered %d", objectsRendered);
		ImGui::Text("Lights rendered %d", lightsRendered);
		ImGui::Text("FPS %.3f", 1.0 / Times::Instance()->deltaTime);
		ImGui::Text("TimeStep %.3f", 1.0 / Times::Instance()->timeStep);
		ImGui::Text("PickedID %d", pickedID);
		ImGui::Text("PRUNE %.8f", PhysicsManager::Instance()->pruneAndSweepTime);
		ImGui::Text("SAT %.8f", PhysicsManager::Instance()->satTime);
		ImGui::Text("Intersection Test %.8f", PhysicsManager::Instance()->intersectionTestTime);
		ImGui::Text("Generate Contacts %.8f", PhysicsManager::Instance()->generateContactsTime);
		ImGui::Text("Process Contacts %.8f", PhysicsManager::Instance()->processContactTime);
		ImGui::Text("Positional Correction %.8f", PhysicsManager::Instance()->positionalCorrectionTime);
		ImGui::Text("Iterations Count %d", PhysicsManager::Instance()->iterCount);

		ImGui::End();
	}

    void
    PickingApp::PickingTest()
    {   
        if(isLeftMouseButtonPressed)
        {
			window->GetCursorPos(&leftMouseX,&leftMouseY);

			//read pixel from picking texture
			unsigned int Pixel;
			//inverted y coordinate because glfw 0,0 starts at topleft while opengl texture 0,0 starts at bottomleft
			pickingBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, 1, 1, GL_UNSIGNED_INT, &Pixel, pickingTexture);
			pickedID = Pixel;
			//std::cout << pickedID << std::endl;
			if(lastPickedObject != nullptr) //reset previously picked object color
			{
				lastPickedObject->materials[0]->color = Vector3F(0.f, 0.f, 0.f);

			}
			if(SceneGraph::Instance()->pickingList.find(pickedID) != SceneGraph::Instance()->pickingList.end())
			{
				lastPickedObject = SceneGraph::Instance()->pickingList[pickedID];
				lastPickedObject->materials[0]->color = Vector3F(0.5f, 0.25f, 0.f);
				Vector3F world_position;
				geometryBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, 1, 1, GL_FLOAT, world_position.vect, worldPosTexture);
				Vector3 dWorldPos = Vector3(world_position.x, world_position.y, world_position.z);
				Vector3 impulse = (dWorldPos - currentCamera->GetPosition2()).vectNormalize();

				if (RigidBody* body = this->lastPickedObject->GetComponent<RigidBody>()) body->ApplyImpulse(impulse, 1.f, dWorldPos);
			}
        }
    }

    void
    PickingApp::InitGL()
    {

        // grey background
        glClearColor(0.f, 0.f, 0.f, 1.0f);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
    }

	void
	PickingApp::DrawPicking()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		Render::Instance()->drawPicking(GraphicsStorage::shaderIDs["picking"], SceneGraph::Instance()->pickingList, pickingBuffer, DrawBuffers, 1);
	}

	void
	PickingApp::DrawDebug()
	{
		DebugDraw::Instance()->DrawBoundingBoxes();
	}

	void 
	PickingApp::LoadScene1()
	{
		//A plank suspended on a static box.	
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 20.f, 60.f));
		DebugDraw::Instance()->Init(SceneGraph::Instance()->addChild());

		Object* sphere = SceneGraph::Instance()->addPhysicObject("sphere", Vector3(0.f, 3.f, 0.f));//automatically registered for collision detection and response
		RigidBody* body = sphere->GetComponent<RigidBody>();
		body->SetIsKinematic(true);
		sphere->materials[0]->shininess = 10.f;

		Object* directionalLight = SceneGraph::Instance()->addDirectionalLight();
		//directionalLight->mat->SetDiffuseIntensity(0.1f);
		directionalLightObject = directionalLight;

		Object* plane = SceneGraph::Instance()->addObject("cube", Vector3(0.f, -2.5f, 0.f));
		planeObject = plane;
		plane->materials[0]->SetShininess(12.f);
		//plane->mat->tileX = 2;
		//plane->mat->tileY = 2;
		body = new RigidBody();
		plane->AddComponent(body);
		plane->node->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetIsKinematic(true);
		PhysicsManager::Instance()->RegisterRigidBody(body); //manually registered after manually creating rigid body component and assembling the object
		SceneGraph::Instance()->SceneObject->node->SetMovable(true);
		SceneGraph::Instance()->InitializeSceneTree();
		
	}

	void PickingApp::LoadScene2()
	{
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 10.f, 60.f));
		DebugDraw::Instance()->Init(SceneGraph::Instance()->addChild());

		Object* directionalLight = SceneGraph::Instance()->addDirectionalLight();

		for (int i = 0; i < 300; i++)
		{
			Object* pointLight = SceneGraph::Instance()->addPointLight(false, SceneGraph::Instance()->generateRandomIntervallVectorCubic(-20, 20));
			RigidBody* body = new RigidBody();
			pointLight->AddComponent(body, true);
		}

		for (int i = 0; i < 300; i++)
		{
			SceneGraph::Instance()->addObject("sphere", SceneGraph::Instance()->generateRandomIntervallVectorCubic(-20, 20));
		}

		Object* plane = SceneGraph::Instance()->addObject("cube");
		plane->node->SetScale(Vector3(25.f, 0.2f, 25.f));
		this->plane = plane;
		PhysicsManager::Instance()->gravity = Vector3();
		SceneGraph::Instance()->SceneObject->node->SetMovable(true);
		SceneGraph::Instance()->InitializeSceneTree();
	}

	void PickingApp::LoadScene3()
	{
		Clear();
		currentCamera->SetPosition(Vector3());
		DebugDraw::Instance()->Init(SceneGraph::Instance()->addChild());

		Object* directionalLight = SceneGraph::Instance()->addDirectionalLight();

		float rS = 1.f;
		for (int i = 0; i < 500; i++)
		{
			Object* object = SceneGraph::Instance()->addPhysicObject("icosphere", SceneGraph::Instance()->generateRandomIntervallVectorCubic(-80, 80));
			rS = (float)(rand() % 5) + 1.f;
			object->node->SetScale(Vector3(rS, rS, rS));
			RigidBody* body = object->GetComponent<RigidBody>();
			body->SetCanSleep(false);
		}
		
		PhysicsManager::Instance()->gravity = Vector3();
		SceneGraph::Instance()->SceneObject->node->SetMovable(true);
		SceneGraph::Instance()->InitializeSceneTree();
	}

	void 
	PickingApp::Clear()
	{
		SceneGraph::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		DebugDraw::Instance()->Clear();
		lastPickedObject = nullptr;
		directionalLightObject = nullptr;
		dirLightComp = nullptr;
		planeObject = nullptr;
	}

	void PickingApp::Vortex()
	{
		/*
		for (auto& obj : SceneGraph::Instance()->pickingList)
		{
			Vector3 dir = obj.second->GetPosition() - Vector3(0,-10,0);
			obj.second->ApplyImpulse(dir*-20.f, obj.second->GetPosition());
		}
		*/

		for (auto& obj : SceneGraph::Instance()->pointLights)
		{
			if (RigidBody* body = obj->GetComponent<RigidBody>())
			{
				Vector3 dir = obj->node->GetWorldPosition() - Vector3(0.f, -10.f, 0.f);
				body->ApplyImpulse(dir.vectNormalize()*-200.f, obj->node->GetWorldPosition());
			}
		}
	}

	void PickingApp::DrawGeometryPass()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		objectsRendered = Render::Instance()->drawGeometry(GraphicsStorage::shaderIDs["geometry"], SceneGraph::Instance()->renderList, geometryBuffer, DrawBuffers, 4);
	}

	void PickingApp::DrawLightPass()
	{
		GLuint pointLightShader = GraphicsStorage::shaderIDs["pointLight"];
		GLuint pointLightShadowShader = GraphicsStorage::shaderIDs["pointLightShadow"];
		GLuint spotLightShader = GraphicsStorage::shaderIDs["spotLight"];
		GLuint spotLightShadowShader = GraphicsStorage::shaderIDs["spotLightShadow"];
		GLuint directionalLightShader = GraphicsStorage::shaderIDs["directionalLight"];
		GLuint directionalLightShadowShader = GraphicsStorage::shaderIDs["directionalLightShadow"];

		GLenum drawLightAttachments[] = { GL_COLOR_ATTACHMENT4 };
		lightsRendered = Render::Instance()->drawLight(pointLightShader, pointLightShadowShader, spotLightShader, spotLightShadowShader, directionalLightShader, directionalLightShadowShader, geometryBuffer, geometryBuffer, drawLightAttachments, 1);
	}

	void PickingApp::BlitToScreenPass()
	{
		//bind geometry buffer for final pass
		FBOManager::Instance()->BindFrameBuffer(GL_DRAW_FRAMEBUFFER, 0); //for drawing we are unbinding to the screen buffer
		FBOManager::Instance()->BindFrameBuffer(GL_READ_FRAMEBUFFER, geometryBuffer->handle); //and we read from the geometry buffer
		glReadBuffer(GL_COLOR_ATTACHMENT4); //enable the final color texture buffer for reading, we tell that we want only to read the attachment 4
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR); 
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void PickingApp::MovePlaneUpNDown()
	{
		if (plane != nullptr)
		{
			if (plane->node->GetWorldPosition().y < -17) planeDir = 1;
			else if (plane->node->GetWorldPosition().y > 10) planeDir = -1;

			plane->node->Translate(Vector3(0.f, 0.1f*planeDir, 0.f));
		}
	}

	void PickingApp::DrawGeometryMaps(int width, int height)
	{
		GLuint depthPanelShader = GraphicsStorage::shaderIDs["depthPanel"];

		float fHeight = (float)height;
		float fWidth = (float)width;
		int y = (int)(fHeight*0.1f);
		int glWidth = (int)(fWidth *0.1f);
		int glHeight = (int)(fHeight*0.1f);

		Render::Instance()->drawRegion(depthPanelShader, 0, y, glWidth, glHeight, worldPosTexture);
		Render::Instance()->drawRegion(depthPanelShader, 0, 0, glWidth, glHeight, diffuseTexture);
		Render::Instance()->drawRegion(depthPanelShader, glWidth, 0, glWidth, glHeight, normalTexture);
		Render::Instance()->drawRegion(depthPanelShader, glWidth, y, glWidth, glHeight, finalColorTexture);
	}

	void PickingApp::SpawnSomeLights()
	{
		if (SceneGraph::Instance()->pointLights.size() < 500)
		{
			Object* pointLight = SceneGraph::Instance()->addPointLight(false, SceneGraph::Instance()->generateRandomIntervallVectorFlat(-20, 20, SceneGraph::y), SceneGraph::Instance()->generateRandomIntervallVectorCubic(0, 6000).toFloat() / 6000.f);
			Object* sphere = SceneGraph::Instance()->addObject("sphere", pointLight->node->GetLocalPosition());
			sphere->node->SetScale(Vector3(0.1f, 0.1f, 0.1f));
			sphere->materials[0]->shininess = 10.f;
		}
	}

	void PickingApp::MouseCallback(double mouseX, double mouseY)
	{
		if (altButtonToggle)
		{
			currentCamera->UpdateOrientation(mouseX, mouseY);
			window->SetCursorPos(windowMidX, windowMidY);
		}
	}

	void PickingApp::SetUpCamera()
	{
		currentCamera = new Camera(Vector3(0.f, 10.f, 60.f), windowWidth, windowHeight);
		currentCamera->Update(Times::Instance()->timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void PickingApp::SetUpBuffers(int windowWidthIn, int windowHeightIn)
	{
		geometryBuffer = FBOManager::Instance()->GenerateFBO();
		worldPosTexture = new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT0);
		worldPosTexture->GenerateBindSpecify();
		worldPosTexture->SetClampingToEdge();
		worldPosTexture->SetLinear();
		diffuseTexture = new Texture(GL_TEXTURE_2D, 0, GL_RGB, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1);
		diffuseTexture->GenerateBindSpecify();
		diffuseTexture->SetClampingToEdge();
		diffuseTexture->SetLinear();
		normalTexture = new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT2);
		normalTexture->GenerateBindSpecify();
		normalTexture->SetClampingToEdge();
		normalTexture->SetLinear();
		Texture* materialPropertiesTexture = new Texture(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidthIn, windowHeightIn, GL_RGBA, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT3);
		materialPropertiesTexture->GenerateBindSpecify();
		materialPropertiesTexture->SetClampingToEdge();
		materialPropertiesTexture->SetLinear();
		finalColorTexture = new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT4);
		finalColorTexture->GenerateBindSpecify();
		finalColorTexture->SetClampingToEdge();
		finalColorTexture->SetLinear();
		Texture* depthTexture = new Texture(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidthIn, windowHeightIn, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_STENCIL_ATTACHMENT);
		depthTexture->GenerateBindSpecify();
		depthTexture->SetClampingToEdge();
		depthTexture->SetLinear();

		geometryBuffer->RegisterTexture(worldPosTexture);
		geometryBuffer->RegisterTexture(diffuseTexture);
		geometryBuffer->RegisterTexture(normalTexture);
		geometryBuffer->RegisterTexture(materialPropertiesTexture);
		geometryBuffer->RegisterTexture(finalColorTexture);
		geometryBuffer->RegisterTexture(depthTexture);
		geometryBuffer->SpecifyTextures();
		geometryBuffer->CheckAndCleanup();

		diffuseTexture = diffuseTexture;
		normalTexture = normalTexture;
		ORMSTexture = materialPropertiesTexture;
		
		pickingBuffer = FBOManager::Instance()->GenerateFBO();
		pickingTexture = new Texture(GL_TEXTURE_2D, 0, GL_R32UI, windowWidthIn, windowHeightIn, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL, GL_COLOR_ATTACHMENT0);
		pickingTexture->GenerateBindSpecify();
		pickingTexture->SetClampingToEdge();
		pickingTexture->SetLinear();
		Texture* pickingDepth = new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidthIn, windowHeightIn, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT);
		pickingDepth->GenerateBindSpecify();
		pickingDepth->SetClampingToEdge();
		pickingDepth->SetLinear();
		pickingBuffer->RegisterTexture(pickingTexture);
		pickingBuffer->RegisterTexture(pickingDepth);
		pickingBuffer->SpecifyTextures();
		pickingBuffer->CheckAndCleanup();
	}



} // namespace Example
