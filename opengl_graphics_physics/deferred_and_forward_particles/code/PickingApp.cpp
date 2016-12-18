//
// Created by marwac-9 on 9/16/15.
//

#include "PickingApp.h"
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
#include "ParticleSystem.h"
#include "RigidBody.h"
#include "CameraManager.h"

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
            float aspect = (float)this->windowWidth / (float)this->windowHeight;
            currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, 0.1f, 100.f);

			FBOManager::Instance()->UpdateTextureBuffers(this->windowWidth, this->windowHeight);
			currentCamera->UpdateSize(width, height);
        });

        this->window->SetMousePressFunction([this](int button, int action, int mods)
        {
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                isLeftMouseButtonPressed = true;
				if(currentScene == scene3Loaded) FireLightProjectile();
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

        FBOManager::Instance()->SetUpFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpDeferredFrameBuffer(this->windowWidth, this->windowHeight);

		GraphicsManager::LoadAllAssets();

		DebugDraw::Instance()->LoadPrimitives();

		boundingBox = &DebugDraw::Instance()->boundingBox;

        // For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		LoadScene2();
		currentScene = scene2Loaded;

		Matrix4 identityM = Matrix4::identityMatrix();
		double fps_timer = 0;
		Scene::Instance()->SceneObject->node.UpdateNodeMatrix(identityM);
		Scene::Instance()->MainPointLight->node.UpdateNodeMatrix(identityM);
		Scene::Instance()->MainDirectionalLight->node.UpdateNodeMatrix(identityM);

        while (running)
        {
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glDisable(GL_BLEND);
            this->window->Update();

            // Measure FPS
			Time::currentTime = glfwGetTime();
			Time::deltaTime = Time::currentTime - lastTime;

			Monitor(this->window);

			//is cursor window locked
			if (altButtonToggle) CameraManager::Instance()->Update();
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);

			
			Time::timeStep = 0.016 + Time::timeModifier;
			Time::dtInv = 1.0 / Time::timeStep;
			if (paused) Time::timeStep = 0.0, Time::dtInv = 0;

			switch (currentScene)
			{
				case scene1Loaded:
					if (Time::currentTime - fps_timer >= 0.2){
						SpawnSomeLights(); //have to draw on plane
					}
					break;
				case scene2Loaded:
					MovePlaneUpNDown();
					break;				
			}
			
			PhysicsManager::Instance()->SortAndSweep();
			PhysicsManager::Instance()->NarrowTestSAT((float)Time::dtInv);
			
			IntegrateAndUpdateBoxes();
			if (lightsPhysics) 
			{
				if (Time::currentTime - fps_timer >= 0.2 && currentScene == scene2Loaded) Vortex();
				IntegrateLights();
			}
			
			Scene::Instance()->SceneObject->node.UpdateNodeMatrix(identityM);
			Scene::Instance()->MainPointLight->node.UpdateNodeMatrix(identityM);
			Scene::Instance()->MainDirectionalLight->node.UpdateNodeMatrix(identityM);
			PassPickingTexture(); //picking

			DrawGeometryPass();

			PickingTest();
			
			DrawLightPass();

			BlitToScreenPass();
			
			GLuint particleShader = ShaderManager::Instance()->shaderIDs["particle"];
			ShaderManager::Instance()->SetCurrentShader(particleShader);
			for (auto& pSystem : particleSystems)
			{
				pSystem->Draw(CameraManager::Instance()->ViewProjection, particleShader, CameraManager::Instance()->up, CameraManager::Instance()->right);
			}

			if (debug) DrawDebug();

			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			
			FBOManager::Instance()->DrawGeometryMaps(windowWidth, windowHeight);

			if (Time::currentTime - fps_timer >= 0.2){
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " Lights rendered: " + std::to_string(lightsRendered) + " FPS: " + std::to_string(1.0 / Time::deltaTime) + " TimeStep: " + std::to_string(Time::timeStep) + " PickedID: " + std::to_string(pickedID) + (paused ? " PAUSED" : ""));
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
    PickingApp::ClearBuffers()
    {
		GraphicsStorage::ClearMeshes();
		GraphicsStorage::ClearTextures();
		ShaderManager::Instance()->DeleteShaders();
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
				if (paused)
				{
					paused = false;
				}
				else
				{
					paused = true;
				}
			}
			else if (key == GLFW_KEY_T)
			{
				Time::timeModifier = 0.0;
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
    PickingApp::Monitor(Display::Window* window)
    {
		if (window->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) Time::timeModifier += 0.0005;
		if (window->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) Time::timeModifier -= 0.0005;
		if (window->GetKey(GLFW_KEY_UP) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, 0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_DOWN) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, -0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_LEFT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.05f, 0.f, 0.f));
		if (window->GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(-0.05f, 0.f, 0.f));

		currentCamera->holdingForward = (window->GetKey(GLFW_KEY_W) == GLFW_PRESS);
		currentCamera->holdingBackward = (window->GetKey(GLFW_KEY_S) == GLFW_PRESS);
		currentCamera->holdingRight = (window->GetKey(GLFW_KEY_D) == GLFW_PRESS);
		currentCamera->holdingLeft = (window->GetKey(GLFW_KEY_A) == GLFW_PRESS);
		currentCamera->holdingUp = (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS);
		currentCamera->holdingDown = (window->GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    }

    void
    PickingApp::PassPickingTexture()
    {
		
		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		Draw();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		FBOManager::Instance()->UnbindFrameBuffer(draw);
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
			FBOManager::Instance()->ReadPixelID((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, &Pixel);
			pickedID = Pixel;

			//std::cout << pickedID << std::endl;
			if(lastPickedObject != nullptr) //reset previously picked object color
			{
				if (std::find(Scene::Instance()->pointLights.begin(), Scene::Instance()->pointLights.end(), lastPickedObject) == Scene::Instance()->pointLights.end()) //if it's not light
				{
					lastPickedObject->mat->color = Vector3(0.f, 0.f, 0.f);
					lastPickedObject->mat->SetDiffuseIntensity(1.f);
				}
			}  
			if(Scene::Instance()->objectsToRender.find(pickedID) != Scene::Instance()->objectsToRender.end())
			{
				lastPickedObject = Scene::Instance()->objectsToRender[pickedID];
				lastPickedObject->mat->color = Vector3(0.5f, 0.25f, 0.f);
				lastPickedObject->mat->SetDiffuseIntensity(3.f);
				Vector3 world_position;
				FBOManager::Instance()->ReadWorldPos((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, world_position.vect);
				//Vector3 mouseInWorld = ConvertMousePosToWorld();
			
				Vector3 impulse = (world_position - currentCamera->GetPosition2()).vectNormalize();
				if (RigidBody* body = this->lastPickedObject->GetComponent<RigidBody>()) body->ApplyImpulse(impulse, 20.f, world_position);
			}
        }
    }

	Vector3
	PickingApp::ConvertMousePosToWorld()
	{
		double x, y;
		window->GetCursorPos(&x, &y);
		Vector4 mouse_p0s(x,y,0.0,0.0);
		mouse_p0s[0] = ((float)x / (float)windowWidth)*2.f - 1.f;
		mouse_p0s[1] = (((float)windowHeight - (float)y) / windowHeight)*2.f - 1.f;
		mouse_p0s[2] = -1.f;
		mouse_p0s[3] = 1.f;

		Vector4 my_mouse_in_world_space = currentCamera->ProjectionMatrix.inverse() * mouse_p0s;
		my_mouse_in_world_space = currentCamera->getViewMatrix().inverse() * my_mouse_in_world_space;
		my_mouse_in_world_space = my_mouse_in_world_space / my_mouse_in_world_space[3];

		Vector3 my_mouse_in_world_space_vec3(my_mouse_in_world_space[0],my_mouse_in_world_space[1], my_mouse_in_world_space[2]);
		return my_mouse_in_world_space_vec3;
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

        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);

		LoadShaders();
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
		LightID = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightPosition_worldspace");
		GLuint LightDir = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["color"], "LightInvDirection_worldspace");
		glUniform3f(LightID, 0.f, 0.f, 0.f);
		glUniform3f(LightDir, lightInvDir.x, lightInvDir.y, lightInvDir.z);

        this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
    }

    void
	PickingApp::Draw()
    {
		Matrix4F View = currentCamera->getViewMatrix().toFloat();
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View[0][0]);
		
        objectsRendered = 0;
        for(auto& obj : Scene::Instance()->objectsToRender)
        {
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, CameraManager::Instance()->ViewProjection, currentShaderID);
                objectsRendered++;
            }
        }
    }

	void PickingApp::DrawGeometry()
	{
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();

		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::drawGeometry(obj.second, CameraManager::Instance()->ViewProjection, currentShaderID);
				objectsRendered++;
			}
		}
	}

	void
	PickingApp::DrawDebug()
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		
		GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
		GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
		ShaderManager::Instance()->SetCurrentShader(wireframeShader);
		for (auto& obj : PhysicsManager::Instance()->satOverlaps)
		{
			obj.rbody1->aabb.color = Vector3(1.f, 0.f, 0.f);
			obj.rbody2->aabb.color = Vector3(1.f, 0.f, 0.f);
		}

		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius))
			{
				if (RigidBody* body = obj.second->GetComponent<RigidBody>())
				{
					boundingBox->mat->SetColor(body->obb.color);
					boundingBox->Draw(Matrix4::scale(obj.second->GetMeshDimensions())*obj.second->node.TopDownTransform, currentCamera->getViewMatrix(), currentCamera->ProjectionMatrix, wireframeShader);
					boundingBox->mat->SetColor(body->aabb.color);
					boundingBox->Draw(body->aabb.model, currentCamera->getViewMatrix(), currentCamera->ProjectionMatrix, wireframeShader);
				}
			}
		}
		glDepthMask(GL_FALSE);
	}

    void 
	PickingApp::IntegrateAndUpdateBoxes()
    {
		for(auto& obj : Scene::Instance()->objectsToRender)
		{
			obj.second->Update();
			/*
			if (RigidBody* body = obj.second->GetComponent<RigidBody>())
			{
				body->IntegrateRunge(timestep, PhysicsManager::Instance()->gravity);
				body->UpdateBoundingBoxes(DebugDraw::Instance()->boundingBox);
				body->UpdateInertiaTensor();
			}
			*/
		}
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

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);		

		Object* plane = Scene::Instance()->addObject("cube", Vector3(0.f, -2.5f, 0.f));
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

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);

		for (int i = 0; i < 300; i++)
		{
			Object* pointLight = Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVectorCubic(-20, 20));
			RigidBody* body = new RigidBody(pointLight);
			pointLight->AddComponent(body);
		}

		for (int i = 0; i < 300; i++)
		{
			Object* sphere = Scene::Instance()->addObject("sphere", Scene::Instance()->generateRandomIntervallVectorCubic(-20, 20));
		}

		Object* plane = Scene::Instance()->addObject("cube");
		plane->SetScale(Vector3(25.f, 0.2f, 25.f));
		this->plane = plane;

		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.f, 10.f, 0.f));
		pointLight->SetScale(Vector3(20.f, 20.f, 20.f));
		pointLight->mat->SetColor(Vector3(1.f, 0.f, 0.f));
		pointLight->mat->SetDiffuseIntensity(10.f);

		ParticleSystem* pSystem = new ParticleSystem(100000, 1000);
		pSystem->SetTexture(GraphicsStorage::textures[10]->TextureID);
		pSystem->SetLifeTime(5.0f);
		pSystem->SetColor(Vector4(1.f, 0.f, 0.f, 0.2f));
		pointLight->AddComponent(pSystem);
		particleSystems.push_back(pSystem);
		lightsPhysics = true;
	}

	void PickingApp::LoadScene3()
	{
		Clear();
		currentCamera->SetPosition(Vector3());

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);

		float rS = 1.f;
		for (int i = 0; i < 500; i++)
		{
			Object* object = Scene::Instance()->addObject("icosphere", Scene::Instance()->generateRandomIntervallVectorCubic(-80, 80));
			RigidBody* body = new RigidBody(object);
			object->AddComponent(body);
			PhysicsManager::Instance()->RegisterRigidBody(body);
			rS = (float)(rand() % 5) + 1.f;
			object->SetScale(Vector3(rS, rS, rS));
			body->SetCanSleep(false);
		}
		
		lightsPhysics = true;
		PhysicsManager::Instance()->gravity = Vector3();
	}

	void PickingApp::FireLightProjectile()
	{
		Object* pointLight = Scene::Instance()->addPointLight(currentCamera->GetPosition2()+currentCamera->getDirection()*3.f, Vector3(1.f, 1.f, 0.f));
		pointLight->SetScale(Vector3(10.f, 10.f, 10.f));

		RigidBody* body = new RigidBody(pointLight);
		pointLight->AddComponent(body);
		body->SetCanSleep(false);
		body->ApplyImpulse(currentCamera->getDirection()*4000.f, pointLight->GetLocalPosition());

		ParticleSystem* pSystem = new ParticleSystem(3000, 80);
		pointLight->AddComponent(pSystem);
		pSystem->SetTexture(GraphicsStorage::textures[10]->TextureID);
		pSystem->SetDirection(Vector3(0.f, 0.f, 0.f));
		
		particleSystems.push_back(pSystem);
	}

	void 
	PickingApp::Clear()
	{
		particleSystems.clear();
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
	}

	void PickingApp::Vortex()
	{
		/*
		for (auto& obj : Scene::Instance()->objectsToRender)
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
				body->ApplyImpulse(dir.vectNormalize()*-2000.f, obj->GetWorldPosition());
			}			
		}
	}

	void PickingApp::DrawGeometryPass()
	{
		//bind for geometry pass
		FBOManager::Instance()->BindGeometryBuffer(draw);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, DrawBuffers);

		//glViewport(0, 0, 2048, 2048);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//glDisable(GL_BLEND);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["geometry"]);
		DrawGeometry();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDepthMask(GL_FALSE);

		//glDisable(GL_DEPTH_TEST);
		FBOManager::Instance()->UnbindFrameBuffer(draw); //we don't have to unbind we work all the way with the buffer but i prefer to do it anyway and enable when needed
	}

	void PickingApp::DrawLightPass()
	{
		//commented out stuff that is now set up in different place
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE, GL_ONE);

		FBOManager::Instance()->BindGeometryBuffer(draw); // we draw now to the geometry buffer since it has the final color where the light is composited
		//glClear(GL_COLOR_BUFFER_BIT);
		GLuint pointLightS = ShaderManager::Instance()->shaderIDs["pointLight"];
		ShaderManager::Instance()->SetCurrentShader(pointLightS);
		GLuint screenSize = glGetUniformLocation(pointLightS, "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);
		GLuint CameraPos = glGetUniformLocation(pointLightS, "CameraPos");
		Vector3 camPos = currentCamera->GetPosition2();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint directionalLightS = ShaderManager::Instance()->shaderIDs["directionalLight"];
		ShaderManager::Instance()->SetCurrentShader(directionalLightS);
		GLuint screenSizeD = glGetUniformLocation(directionalLightS, "screenSize");
		glUniform2f(screenSizeD, (float)windowWidth, (float)windowHeight);
		GLuint CameraPosD = glGetUniformLocation(directionalLightS, "CameraPos");
		glUniform3fv(CameraPosD, 1, &camPos.x);

		DrawPointLights();
		DrawDirectionalLights();
		DisableTextures();
		FBOManager::Instance()->UnbindFrameBuffer(draw);
	}

	void PickingApp::DrawPointLights()
	{
		glEnable(GL_STENCIL_TEST);
		int pointLightsNum = Scene::Instance()->pointLights.size();
		lightsRendered = 0;
		for (int i = 0; i < pointLightsNum; i++)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(Scene::Instance()->pointLights[i]->GetWorldPosition(), Scene::Instance()->pointLights[i]->radius)) {
				StencilPass(Scene::Instance()->pointLights[i]); //sets up stencil pass
				PointLightPass(Scene::Instance()->pointLights[i]); //sets up point light pass	
				lightsRendered++;
			}
		}
		glDisable(GL_STENCIL_TEST);
	}

	void PickingApp::StencilPass(Object* pointLight)
	{
		//enable stencil shader 
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["stencil"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		glDrawBuffer(GL_NONE); //don't write to geometry buffers

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_CULL_FACE);

		glClear(GL_STENCIL_BUFFER_BIT);

		// We need the stencil test to be enabled but we want it
		// to succeed always. Only the depth test matters.
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		Render::drawLight(pointLight, CameraManager::Instance()->ViewProjection, currentShaderID);
	}

	void PickingApp::PointLightPass(Object* pointLight)
	{
		//enable drawing of final color in geometry buffer 
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["pointLight"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);
		ActivateTextures();

		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		Render::drawLight(pointLight, CameraManager::Instance()->ViewProjection, currentShaderID);
		
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);
	}

	void PickingApp::DrawDirectionalLights()
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		//set 
		//directional shader
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["directionalLight"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);
		ActivateTextures();

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);
		
		for (auto& directionalLight : Scene::Instance()->directionalLights)
		{
			Render::drawLight(directionalLight, Matrix4::identityMatrix(), currentShaderID);
			lightsRendered++;
		}
		glDisable(GL_BLEND);
	}

	void PickingApp::ActivateTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->positionBufferHandle);
		GLuint positionSampler = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "positionSampler");
		glUniform1i(positionSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->diffuseBufferHandle);
		GLuint diffuseSampler = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "diffuseSampler");
		glUniform1i(diffuseSampler, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->normalBufferHandle);
		GLuint normalsSampler = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "normalsSampler");
		glUniform1i(normalsSampler, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->diffIntAmbIntShinSpecIntBufferHandle);
		GLuint diffIntAmbIntShinSampler = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "diffIntAmbIntShinSpecIntSampler");
		glUniform1i(diffIntAmbIntShinSampler, 3);
	}

	void PickingApp::BlitToScreenPass()
	{
		//bind geometry buffer for final pass
		FBOManager::Instance()->UnbindFrameBuffer(draw); //for drawing we are unbinding to the screen buffer
		FBOManager::Instance()->BindGeometryBuffer(read); //and we read from the geometry buffer
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

	void PickingApp::DisableTextures()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void PickingApp::SpawnSomeLights()
	{
		if (Scene::Instance()->pointLights.size() < 500)
		{
			Object* pointLight = Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::y), Scene::Instance()->generateRandomIntervallVectorCubic(0, 6000) / 6000.f);
			Object* sphere = Scene::Instance()->addObject("sphere", pointLight->GetLocalPosition());
			sphere->SetScale(Vector3(0.1f, 0.1f, 0.1f));
			sphere->mat->diffuseIntensity = 2.f;
			sphere->mat->ambientIntensity = 1.5f;
			sphere->mat->specularIntensity = 4.f;
			sphere->mat->shininess = 10.f;
		}
	}

	void PickingApp::IntegrateLights()
	{
		for (auto& obj : Scene::Instance()->pointLights)
		{
			obj->Update();
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
		currentCamera->Update((float)Time::timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, 0.1f, 200.0f);
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->getViewMatrix();
	}

	void PickingApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
		ShaderManager::Instance()->AddShader("picking", GraphicsManager::LoadShaders("Resources/Shaders/VSPicking.glsl", "Resources/Shaders/FSPicking.glsl"));
		ShaderManager::Instance()->AddShader("wireframe", GraphicsManager::LoadShaders("Resources/Shaders/VSBB.glsl", "Resources/Shaders/FSBB.glsl"));
		ShaderManager::Instance()->AddShader("dftext", GraphicsManager::LoadShaders("Resources/Shaders/VSDFText.glsl", "Resources/Shaders/FSDFText.glsl"));
		ShaderManager::Instance()->AddShader("depth", GraphicsManager::LoadShaders("Resources/Shaders/VSDepth.glsl", "Resources/Shaders/FSDepth.glsl"));
		ShaderManager::Instance()->AddShader("depthPanel", GraphicsManager::LoadShaders("Resources/Shaders/VSShadowMapPlane.glsl", "Resources/Shaders/FSShadowMapPlane.glsl"));
		ShaderManager::Instance()->AddShader("blur", GraphicsManager::LoadShaders("Resources/Shaders/VSBlur.glsl", "Resources/Shaders/FSBlur.glsl"));
		ShaderManager::Instance()->AddShader("geometry", GraphicsManager::LoadShaders("Resources/Shaders/VSGeometry.glsl", "Resources/Shaders/FSGeometry.glsl"));
		ShaderManager::Instance()->AddShader("pointLight", GraphicsManager::LoadShaders("Resources/Shaders/VSPointLight.glsl", "Resources/Shaders/FSPointLight.glsl"));
		ShaderManager::Instance()->AddShader("directionalLight", GraphicsManager::LoadShaders("Resources/Shaders/VSDirectionalLight.glsl", "Resources/Shaders/FSDirectionalLight.glsl"));
		ShaderManager::Instance()->AddShader("stencil", GraphicsManager::LoadShaders("Resources/Shaders/VSStencil.glsl", "Resources/Shaders/FSStencil.glsl"));
		ShaderManager::Instance()->AddShader("particle", GraphicsManager::LoadShaders("Resources/Shaders/VSParticle.glsl", "Resources/Shaders/FSParticle.glsl"));
	}

	



} // namespace Example