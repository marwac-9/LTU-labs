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
#include "Mesh.h"
#include "OBJ.h"
#include <fstream>
#include "Scene.h"
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

using namespace mwm;
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
			this->windowWidth = width;
			this->windowHeight = height;
			this->windowMidX = windowWidth / 2.0f;
			this->windowMidY = windowHeight / 2.0f;
            this->window->SetSize(this->windowWidth, this->windowHeight);

			FBOManager::Instance()->UpdateTextureBuffers(this->windowWidth, this->windowHeight);

			currentCamera->UpdateSize(width, height);
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

		DebugDraw::Instance()->LoadPrimitives();

        // For speed computation (FPS)
		Times::Instance()->currentTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		LoadScene2();
		currentScene = scene2Loaded;

		double customIntervalTime = 0.0;
		Scene::Instance()->Update();

		ImGui_ImplGlfwGL3_Init(this->window->GetGLFWWindow(), false);

        while (running)
        {
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glDisable(GL_BLEND);
            this->window->Update();
			ImGui_ImplGlfwGL3_NewFrame();

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

			Scene::Instance()->Update();
			
			PhysicsManager::Instance()->Update(Times::Instance()->dtInv);
			
			if (lightsPhysics) 
			{
				if (currentScene == scene2Loaded) Vortex();
			}

			Render::Instance()->UpdateEBOs();

			GenerateGUI();

			DrawPicking();

			DrawGeometryPass();

			if (altButtonToggle) PickingTest();
			
			DrawLightPass();

			BlitToScreenPass();

			if (debug) DrawDebug();

			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			
			DrawGeometryMaps(windowWidth, windowHeight);

			ImGui::Render();

			customIntervalTime += Times::Instance()->deltaTime;
            
            this->window->SwapBuffers();
        }
        this->ClearBuffers();
		GraphicsStorage::ClearMaterials();
		GraphicsStorage::ClearOBJs();
        this->window->Close();
    }

    void
    PickingApp::ClearBuffers()
    {
		GraphicsStorage::ClearMeshes();
		GraphicsStorage::ClearTextures();
		GraphicsStorage::ClearShaders();
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
				//Scene::Instance()->addRandomObject();
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
				GraphicsManager::SaveToOBJ(GraphicsStorage::objects.back());
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
				Object* cube = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, 8.f, 0.f));
				cube->SetPosition(Vector3(0.f, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
			}
		}
    }

    void
    PickingApp::Monitor(Display::Window* windowIn)
    {
		if (windowIn->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) Times::Instance()->timeModifier += 0.0005;
		if (windowIn->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) Times::Instance()->timeModifier -= 0.0005;
		if (windowIn->GetKey(GLFW_KEY_UP) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, 0.05f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_DOWN) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, -0.05f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_LEFT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.05f, 0.f, 0.f));
		if (windowIn->GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(-0.05f, 0.f, 0.f));

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
		if (directionalLightObject != nullptr)
		{
			ImGui::SliderFloat("DirDif", &directionalLightObject->mat->diffuseIntensity, 0.f, 100.f);
			ImGui::SliderFloat("DirSpec", &directionalLightObject->mat->specularIntensity, 0.f, 100.f);
			ImGui::SliderFloat("DirShi", &directionalLightObject->mat->shininess, 0.f, 100.f);
		}
		if (planeObject != nullptr)
		{
			ImGui::SliderFloat("PlaneDif", &planeObject->mat->diffuseIntensity, 0.f, 100.f);
			ImGui::SliderFloat("PlaneSpec", &planeObject->mat->specularIntensity, 0.f, 100.f);
			ImGui::SliderFloat("PlaneShi", &planeObject->mat->shininess, 0.f, 100.f);
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
			pickingBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RED_INTEGER, GL_UNSIGNED_INT, &Pixel, pickingTexture->attachment);
			pickedID = Pixel;
			//std::cout << pickedID << std::endl;
			if(lastPickedObject != nullptr) //reset previously picked object color
			{
				lastPickedObject->mat->color = Vector3F(0.f, 0.f, 0.f);
				lastPickedObject->mat->SetDiffuseIntensity(1.f);
			}
			if(Scene::Instance()->pickingList.find(pickedID) != Scene::Instance()->pickingList.end())
			{
				lastPickedObject = Scene::Instance()->pickingList[pickedID];
				lastPickedObject->mat->color = Vector3F(0.5f, 0.25f, 0.f);
				lastPickedObject->mat->SetDiffuseIntensity(3.f);
				Vector3F world_position;
				geometryBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RGB, GL_FLOAT, world_position.vect, worldPosTexture->attachment);
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
		Render::Instance()->drawPicking(Scene::Instance()->pickingList, pickingBuffer, DrawBuffers, 1);
	}

	void
	PickingApp::DrawDebug()
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		
		GLuint wireframeShader = GraphicsStorage::shaderIDs["wireframe"];
		ShaderManager::Instance()->SetCurrentShader(wireframeShader);

		for (auto& obj : Scene::Instance()->renderList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj->node.centeredPosition, obj->radius))
			{
				if (RigidBody* body = obj->GetComponent<RigidBody>())
				{
					Render::Instance()->boundingBox.mat->SetColor(body->obb.color);
					Render::Instance()->boundingBox.Draw(Matrix4::scale(obj->GetMeshDimensions())*obj->node.TopDownTransform, CameraManager::Instance()->ViewProjection, wireframeShader);
					Render::Instance()->boundingBox.mat->SetColor(body->aabb.color);
					Render::Instance()->boundingBox.Draw(body->aabb.model, CameraManager::Instance()->ViewProjection, wireframeShader);
				}
			}
		}
		glDepthMask(GL_FALSE);
	}

	void 
	PickingApp::LoadScene1()
	{
		//A plank suspended on a static box.	
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 20.f, 60.f));

		Object* sphere = Scene::Instance()->addPhysicObject("sphere", Vector3(0.f, 3.f, 0.f));//automatically registered for collision detection and response
		RigidBody* body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);
		sphere->mat->specularIntensity = 4.f;
		sphere->mat->shininess = 10.f;

		Object* directionalLight = Scene::Instance()->addDirectionalLight();
		//directionalLight->mat->SetDiffuseIntensity(0.1f);
		directionalLight->mat->SetSpecularIntensity(0.1f);
		directionalLight->mat->SetShininess(1.0f);
		directionalLightObject = directionalLight;

		Object* plane = Scene::Instance()->addObject("cube", Vector3(0.f, -2.5f, 0.f));
		planeObject = plane;
		plane->mat->SetShininess(12.f);
		plane->mat->SetSpecularIntensity(3.f);
		//plane->mat->tileX = 2;
		//plane->mat->tileY = 2;
		body = new RigidBody(plane);
		plane->AddComponent(body);
		plane->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetMass(FLT_MAX); 
		body->isKinematic = true;
		PhysicsManager::Instance()->RegisterRigidBody(body); //manually registered after manually creating rigid body component and assembling the object
		
	}

	void PickingApp::LoadScene2()
	{
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 10.f, 60.f));

		Object* directionalLight = Scene::Instance()->addDirectionalLight();
		directionalLight->mat->SetDiffuseIntensity(0.5f);

		for (int i = 0; i < 300; i++)
		{
			Object* pointLight = Scene::Instance()->addPointLight(false, Scene::Instance()->generateRandomIntervallVectorCubic(-20, 20));
			RigidBody* body = new RigidBody(pointLight);
			pointLight->AddComponent(body);
		}

		for (int i = 0; i < 300; i++)
		{
			Scene::Instance()->addObject("sphere", Scene::Instance()->generateRandomIntervallVectorCubic(-20, 20));
		}

		Object* plane = Scene::Instance()->addObject("cube");
		plane->SetScale(Vector3(25.f, 0.2f, 25.f));
		this->plane = plane;
		PhysicsManager::Instance()->gravity = Vector3();
	}

	void PickingApp::LoadScene3()
	{
		Clear();
		currentCamera->SetPosition(Vector3());

		Object* directionalLight = Scene::Instance()->addDirectionalLight();
		directionalLight->mat->SetDiffuseIntensity(0.5f);

		float rS = 1.f;
		for (int i = 0; i < 500; i++)
		{
			Object* object = Scene::Instance()->addPhysicObject("icosphere", Scene::Instance()->generateRandomIntervallVectorCubic(-80, 80));
			rS = (float)(rand() % 5) + 1.f;
			object->SetScale(Vector3(rS, rS, rS));
			RigidBody* body = object->GetComponent<RigidBody>();
			body->SetCanSleep(false);
		}
		
		PhysicsManager::Instance()->gravity = Vector3();
	}

	void 
	PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
		directionalLightObject = nullptr;
		planeObject = nullptr;
	}

	void PickingApp::Vortex()
	{
		/*
		for (auto& obj : Scene::Instance()->pickingList)
		{
			Vector3 dir = obj.second->GetPosition() - Vector3(0,-10,0);
			obj.second->ApplyImpulse(dir*-20.f, obj.second->GetPosition());
		}
		*/

		for (auto& obj : Scene::Instance()->pointLights)
		{
			if (RigidBody* body = obj->GetComponent<RigidBody>())
			{
				Vector3 dir = obj->GetWorldPosition() - Vector3(0.f, -10.f, 0.f);
				body->ApplyImpulse(dir.vectNormalize()*-200.f, obj->GetWorldPosition());
			}
		}
	}

	void PickingApp::DrawGeometryPass()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		objectsRendered = Render::Instance()->drawGeometry(Scene::Instance()->renderList, geometryBuffer, DrawBuffers, 4);
	}

	void PickingApp::DrawLightPass()
	{
		GLenum drawLightAttachments[] = { GL_COLOR_ATTACHMENT4 };
		lightsRendered = Render::Instance()->drawLight(geometryBuffer, geometryBuffer, drawLightAttachments, 1);
	}

	void PickingApp::BlitToScreenPass()
	{
		//bind geometry buffer for final pass
		FBOManager::Instance()->UnbindFrameBuffer(draw); //for drawing we are unbinding to the screen buffer
		FBOManager::Instance()->BindFrameBuffer(read, geometryBuffer->handle); //and we read from the geometry buffer
		glReadBuffer(GL_COLOR_ATTACHMENT4); //enable the final color texture buffer for reading
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR); 
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		FBOManager::Instance()->UnbindFrameBuffer(readDraw);
	}

	void PickingApp::MovePlaneUpNDown()
	{
		if (plane != nullptr)
		{
			if (plane->GetWorldPosition().y < -17) planeDir = 1;
			else if (plane->GetWorldPosition().y > 10) planeDir = -1;

			plane->Translate(Vector3(0.f, 0.1f*planeDir, 0.f));
		}
		
	}

	void PickingApp::DrawGeometryMaps(int width, int height)
	{
		ShaderManager::Instance()->SetCurrentShader(GraphicsStorage::shaderIDs["depthPanel"]);

		float fHeight = (float)height;
		float fWidth = (float)width;
		int y = (int)(fHeight*0.1f);
		int glWidth = (int)(fWidth *0.1f);
		int glHeight = (int)(fHeight*0.1f);

		DebugDraw::Instance()->DrawMap(0, y, glWidth, glHeight, worldPosTexture->handle, width, height);
		DebugDraw::Instance()->DrawMap(0, 0, glWidth, glHeight, diffuseTextureHandle, width, height);
		DebugDraw::Instance()->DrawMap(glWidth, 0, glWidth, glHeight, normalTextureHandle, width, height);
	}

	void PickingApp::SpawnSomeLights()
	{
		if (Scene::Instance()->pointLights.size() < 500)
		{
			Object* pointLight = Scene::Instance()->addPointLight(false, Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::y), Scene::Instance()->generateRandomIntervallVectorCubic(0, 6000).toFloat() / 6000.f);
			Object* sphere = Scene::Instance()->addObject("sphere", pointLight->GetLocalPosition());
			sphere->SetScale(Vector3(0.1f, 0.1f, 0.1f));
			sphere->mat->diffuseIntensity = 2.f;
			sphere->mat->shininess = 10.f;
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
		worldPosTexture = geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT0)); //position
		Texture* diffuseTexture = geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1)); //diffuse
		Texture* normalTexture = geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT2)); //normal
		Texture* materialPropertiesTexture = geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidthIn, windowHeightIn, GL_RGBA, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT3)); //metDiffIntShinSpecInt
		geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidthIn, windowHeightIn, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT4)); //final color
		Texture* depthTexture = geometryBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidthIn, windowHeightIn, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_STENCIL_ATTACHMENT)); //depth
		geometryBuffer->AddDefaultTextureParameters();
		geometryBuffer->GenerateAndAddTextures();
		geometryBuffer->CheckAndCleanup();

		diffuseTextureHandle = diffuseTexture->handle;
		normalTextureHandle = normalTexture->handle;
		metDiffIntShinSpecIntTextureHandle = materialPropertiesTexture->handle;
		
		pickingBuffer = FBOManager::Instance()->GenerateFBO();
		pickingTexture = pickingBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_R32UI, windowWidthIn, windowHeightIn, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL, GL_COLOR_ATTACHMENT0)); //picking
		pickingBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidthIn, windowHeightIn, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT)); //depth
		pickingBuffer->AddDefaultTextureParameters();
		pickingBuffer->GenerateAndAddTextures();
		pickingBuffer->CheckAndCleanup();

		Render::Instance()->GenerateEBOs();
	}



} // namespace Example