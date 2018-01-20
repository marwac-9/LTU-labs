//
// Created by marwac-9 on 9/16/15.
//

#include "PickingApp.h"
#include "GraphicsManager.h"
#include "GraphicsStorage.h"
#include "Node.h"
#include "Material.h"
#include "Mesh.h"
#include "OBJ.h"
#include "Scene.h"
#include "ShaderManager.h"
#include "DebugDraw.h"
#include "PhysicsManager.h"
#include "FBOManager.h"
#include "Camera.h"
#include "Frustum.h"
#include "Render.h"
#include "ParticleSystem.h"
#include "PointSystem.h"
#include "BoundingBoxSystem.h"
#include "LineSystem.h"
#include "RigidBody.h"
#include "CameraManager.h"
#include <string>
#include <algorithm>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>


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
			
			FBOManager::Instance()->UpdateTextureBuffers(this->windowWidth, this->windowHeight);
			currentCamera->UpdateSize(width, height);
		});

		this->window->SetMousePressFunction([this](int button, int action, int mods)
		{
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			{
				isLeftMouseButtonPressed = true;
				if (currentScene == scene3Loaded) FireLightProjectile();
			}
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
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

		//FBOManager::Instance()->SetUpPickingFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpDeferredFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpLightAndPostFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpBlurFrameBuffer(this->windowWidth, this->windowHeight);

		GraphicsManager::LoadAllAssets();
		DebugDraw::Instance()->LoadPrimitives();

		boundingBox = &DebugDraw::Instance()->boundingBox;

		//For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY + 100);
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		LoadScene2();
		currentScene = scene2Loaded;

		double fps_timer = 0;
		Scene::Instance()->Update();

		glfwSwapInterval(0); //unlock fps

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
			if (paused) Time::timeStep = 0.0, Time::dtInv = 0.0;

			if (currentScene == scene4Loaded) LoadScene4();
			if (Time::currentTime - fps_timer >= 0.5) {
				if (currentScene == scene2Loaded)
				{
					
					/*
					int pI = rand() % Scene::Instance()->pickingList.size();
					std::map<unsigned int, Object*>::iterator it = Scene::Instance()->pickingList.begin();
					std::advance(it, pI);

					Object* parent = it->second;
					Scene::Instance()->addObjectTo(parent, "icosphere", Scene::Instance()->generateRandomIntervallVectorCubic(-3, 3));
					*/
				}
			}

			PhysicsManager::Instance()->Update(Time::dtInv);
			Scene::Instance()->Update();
			
			//PassPickingTexture(); //picking

			DrawGeometryPass();

			PickingTest();

			DrawLightPass();
			
			//GenerateFastLines();

			if (drawLines) DrawFastLineSystems(); 
			DrawFastPointSystems();

			DrawParticles();

			BlurLight();

			DrawHDR(); //we draw color to screen here

			BlitToScreenPass(); //depth only

			if (debug) DrawDebugInstanced();
			//if (debug) DrawDebug();

			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);

			FBOManager::Instance()->DrawGeometryMaps(windowWidth, windowHeight);

			if (Time::currentTime - fps_timer >= 0.5) {
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " Lights rendered: " + std::to_string(lightsRendered) + " FPS: " + std::to_string(1.0 / Time::deltaTime) + " TimeStep: " + std::to_string(Time::timeStep) + " PickedID: " + std::to_string(pickedID) + (paused ? " PAUSED" : "") + " PRUNE: " + std::to_string(PhysicsManager::Instance()->pruneAndSweepTime) + " SAT: " + std::to_string(PhysicsManager::Instance()->satTime));
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
	PickingApp::Clear()
	{
		particleSystems.clear();
		lineSystems.clear();
		bbSystems.clear();
		pointSystems.clear();
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
	}

	void
	PickingApp::DrawParticles()
	{
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->lightAndPostFrameBufferHandle); //we bind the lightandposteffect buffer for drawing
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);

		GLuint particleShader = ShaderManager::Instance()->shaderIDs["particle"];
		ShaderManager::Instance()->SetCurrentShader(particleShader);
		Vector3F camUp = CameraManager::Instance()->up.toFloat();
		Vector3F camRight = CameraManager::Instance()->right.toFloat();
		for (auto& pSystem : particleSystems) //particles not affected by light, rendered in forward rendering
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(pSystem->object->GetWorldPosition(), 1.0)) {
				pSystem->Draw(CameraManager::Instance()->ViewProjection, particleShader, camUp, camRight);
			}
		}

		FBOManager::Instance()->UnbindFrameBuffer(draw);
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
			else if (key == GLFW_KEY_4) {
				currentScene = scene4Loaded;
				LoadScene4();
			}
			else if (key == GLFW_KEY_5) {
				currentScene = scene5Loaded;
				LoadScene5();
			}
			else if (key == GLFW_KEY_6) {
				currentScene = scene6Loaded;
				LoadScene6();
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
				if (debug) debug = false;
				else debug = true;

			}
			else if (key == GLFW_KEY_P)
			{
				if (paused) paused = false;
				else paused = true;
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
			else if (key == GLFW_KEY_L)
			{
				if (drawLines) drawLines = false;
				else drawLines = true;
			}
			else if (key == GLFW_KEY_KP_ADD) increment+=20;//Time::timeModifier += 0.0005;
			else if (key == GLFW_KEY_KP_SUBTRACT) increment-=20;//Time::timeModifier -= 0.0005;
		}
	}

	void
	PickingApp::Monitor(Display::Window* window)
	{
		//if (window->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) increment++;//Time::timeModifier += 0.0005;
		//if (window->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) increment--;//Time::timeModifier -= 0.0005;
		if (window->GetKey(GLFW_KEY_UP) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, 0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_DOWN) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, -0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_LEFT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.05f, 0.f, 0.f));
		if (window->GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(-0.05f, 0.f, 0.f));

		if (window->GetKey(GLFW_KEY_KP_9) == GLFW_PRESS) if (pointL) pointL->SetScale(pointL->getScale() + Vector3(0.5f, 0.5f, 0.5f));
		if (window->GetKey(GLFW_KEY_KP_6) == GLFW_PRESS) if (pointL) pointL->SetScale(pointL->getScale() - Vector3(0.5f, 0.5f, 0.5f));
		if (window->GetKey(GLFW_KEY_KP_8) == GLFW_PRESS) if (pointL) pointL->mat->SetDiffuseIntensity(pointL->mat->diffuseIntensity + 0.5f);
		if (window->GetKey(GLFW_KEY_KP_5) == GLFW_PRESS) if (pointL) pointL->mat->SetDiffuseIntensity(pointL->mat->diffuseIntensity - 0.5f);

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
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->pickingFrameBufferHandle);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		Draw();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		FBOManager::Instance()->UnbindFrameBuffer(draw);
	}

	void
	PickingApp::PickingTest()
	{
		if (isLeftMouseButtonPressed)
		{
			window->GetCursorPos(&leftMouseX, &leftMouseY);

			//read pixel from picking texture
			unsigned int Pixel;
			//inverted y coordinate because glfw 0,0 starts at topleft while opengl texture 0,0 starts at bottomleft
			FBOManager::Instance()->ReadPixelID((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, &Pixel);
			pickedID = Pixel;

			//std::cout << pickedID << std::endl;
			if (lastPickedObject != nullptr) //reset previously picked object color
			{
				if (std::find(Scene::Instance()->pointLights.begin(), Scene::Instance()->pointLights.end(), lastPickedObject) == Scene::Instance()->pointLights.end()) //if it's not light
				{
					lastPickedObject->mat->color = Vector3F(0.f, 0.f, 0.f);
					lastPickedObject->mat->SetDiffuseIntensity(1.f);
				}
			}
			if (Scene::Instance()->pickingList.find(pickedID) != Scene::Instance()->pickingList.end())
			{
				lastPickedObject = Scene::Instance()->pickingList[pickedID];
				lastPickedObject->mat->color = Vector3F(0.5f, 0.25f, 0.f);
				lastPickedObject->mat->SetDiffuseIntensity(8.f);
				Vector3F world_position;
				FBOManager::Instance()->ReadWorldPos((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, world_position.vect);
				//Vector3 mouseInWorld = ConvertMousePosToWorld();
				Vector3 dWorldPos = Vector3(world_position.x, world_position.y, world_position.z);
				Vector3 impulse = (dWorldPos - currentCamera->GetPosition2()).vectNormalize();
				if (RigidBody* body = this->lastPickedObject->GetComponent<RigidBody>()) body->ApplyImpulse(impulse, 20.0, dWorldPos);
			}
		}
	}

	Vector3
	PickingApp::ConvertMousePosToWorld()
	{
		double x, y;
		window->GetCursorPos(&x, &y);
		Vector4 mouse_p0s(x, y, 0.0, 0.0);
		mouse_p0s[0] = ((float)x / (float)windowWidth)*2.f - 1.f;
		mouse_p0s[1] = (((float)windowHeight - (float)y) / windowHeight)*2.f - 1.f;
		mouse_p0s[2] = -1.f;
		mouse_p0s[3] = 1.f;

		Vector4 my_mouse_in_world_space = currentCamera->ProjectionMatrix.inverse() * mouse_p0s;
		my_mouse_in_world_space = currentCamera->ViewMatrix.inverse() * my_mouse_in_world_space;
		my_mouse_in_world_space = my_mouse_in_world_space / my_mouse_in_world_space[3];

		Vector3 my_mouse_in_world_space_vec3(my_mouse_in_world_space[0], my_mouse_in_world_space[1], my_mouse_in_world_space[2]);
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
		Matrix4F View = currentCamera->ViewMatrix.toFloat();
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View[0][0]);

		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, CameraManager::Instance()->ViewProjection, currentShaderID);
				objectsRendered++;
			}
		}
	}

	void
	PickingApp::DrawGeometry()
	{
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();

		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->pickingList)
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
			obj.rbody1->aabb.color = Vector3F(1.f, 0.f, 0.f);
			obj.rbody2->aabb.color = Vector3F(1.f, 0.f, 0.f);
		}

		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius))
			{
				if (RigidBody* body = obj.second->GetComponent<RigidBody>())
				{
					boundingBox->mat->SetColor(body->obb.color);
					boundingBox->Draw(Matrix4::scale(obj.second->GetMeshDimensions())*obj.second->node.TopDownTransform, currentCamera->ViewMatrix, currentCamera->ProjectionMatrix, wireframeShader);
					boundingBox->mat->SetColor(body->aabb.color);
					boundingBox->Draw(body->aabb.model, currentCamera->ViewMatrix, currentCamera->ProjectionMatrix, wireframeShader);
				}
			}
		}
		glDepthMask(GL_FALSE);
	}

	void
	PickingApp::LoadScene1()
	{
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 20.f, 60.f));

		Object* sphere = Scene::Instance()->addPhysicObject("sphere", Vector3(0.f, 3.f, 0.f));//automatically registered for collision detection and response
		RigidBody* body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);
		sphere->mat->SetSpecularIntensity(4.f);
		sphere->mat->SetShininess(10.f);

		Object* tunnel = Scene::Instance()->addObjectToScene("tunnel", Vector3(0.f, 0.f, 25.f));
		Material* newMaterial = new Material();
		newMaterial->AssignTexture(GraphicsStorage::textures.at(9));
		GraphicsStorage::materials.push_back(newMaterial);
		tunnel->AssignMaterial(newMaterial);
		//tunnel->mat->SetSpecularIntensity(0.f);
		//tunnel->mat->SetShininess(10.f);

		body = new RigidBody(tunnel);
		tunnel->AddComponent(body);
		//tunnel->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetMass(FLT_MAX);
		body->isKinematic = true;
		PhysicsManager::Instance()->RegisterRigidBody(body);

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.01f);

		///when rendering lights only diffuse intensity and color is important as they are light power and light color
		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.f, 0.f, 50.f), Vector3F(1.0f, 1.0f, 1.0f));
		pointL = pointLight;
		pointLight->mat->SetDiffuseIntensity(100.f);
		pointLight->SetScale(Vector3(40.f, 40.f, 40.f));

		pointLight = Scene::Instance()->addPointLight(Vector3(-1.4f, -1.9f, 9.0f), Vector3F(0.1f, 0.0f, 0.0f));
		pointLight->SetScale(Vector3(10.f, 10.f, 10.f));
		//pointLight->mat->SetDiffuseIntensity(5.f);
		pointLight = Scene::Instance()->addPointLight(Vector3(0.0f, -1.8f, 4.0f), Vector3F(0.0f, 0.0f, 0.2f));
		pointLight = Scene::Instance()->addPointLight(Vector3(0.8f, -1.7f, 6.0f), Vector3F(0.0f, 0.1f, 0.0f));
		//pointLight->mat->SetDiffuseIntensity(1.f);

		//Object* plane = Scene::Instance()->addObject("cube", Vector3(0.f, -2.5f, 0.f));
		//body = new RigidBody(plane);
		//plane->AddComponent(body);
		//plane->SetScale(Vector3(25.f, 2.f, 25.f));
		//body->SetMass(FLT_MAX); 
		//body->isKinematic = true;
		//PhysicsManager::Instance()->RegisterRigidBody(body); //manually registered after manually creating rigid body component and assembling the object

	}

	void
		PickingApp::LoadScene6()
	{
		Clear();
		lightsPhysics = false;
		currentCamera->SetPosition(Vector3(0.f, 20.f, 60.f));

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.2f);

		Object* plane = Scene::Instance()->addPhysicObject("fatplane", Vector3(0.f, -10.f, 0.f));

		Material* newMaterial = new Material();
		newMaterial->AssignTexture(GraphicsStorage::textures.at(5));
		newMaterial->tileX = 50;
		newMaterial->tileY = 50;
		GraphicsStorage::materials.push_back(newMaterial);
		plane->AssignMaterial(newMaterial);
		plane->SetScale(Vector3(10.0,1.0,10.0));
		RigidBody* body = plane->GetComponent<RigidBody>();
		body->SetMass(FLT_MAX);
		body->isKinematic = true;

		for (size_t i = 0; i < 300; i++)
		{
			Object* cube = Scene::Instance()->addPhysicObject("cube", Scene::Instance()->generateRandomIntervallVectorFlat(-400, 400, Scene::axis::y, 10)/10.0);
		}

		lightsPhysics = false;

		bbSystems.push_back(new BoundingBoxSystem(6000));
	}

	void
	PickingApp::LoadScene2()
	{

		Vector3 playerPos = currentCamera->GetInitPos();
		Vector3 camPos = currentCamera->GetPosition2();
		//printf("initPos: %f %f %f\n", playerPos.x, playerPos.y, playerPos.z);
		//printf("camPos: %f %f %f\n", camPos.x, camPos.y, camPos.z);
		int gridSize = 10;
		int x = (int)playerPos.x / gridSize;
		int y = (int)playerPos.y / gridSize;
		int z = (int)playerPos.z / gridSize;
		
		//if (prevGridPos[0] == x && prevGridPos[1] == y && prevGridPos[2] == z) return;
		//printf("%d %d %d\n", x, y, z);
		prevGridPos[0] = x;
		prevGridPos[1] = y;
		prevGridPos[2] = z;

		Clear();
		//printf("%f", coord);
		int xy = ((x + y) * (x + y + 1)) / 2 + y; //unique values for a pair, do it twice to get the unique for three
		int xyz = ((xy + z) * (xy + z + 1)) / 2 + z; //unique values for a pair, do it twice to get the unique for three
		srand(xyz);
		lightsPhysics = false;
		//currentCamera->SetPosition(Vector3(0.f, 10.f, 60.f));

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);

		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.f, 0.f, 0.f));
		pointLight->SetScale(Vector3(20.f, 20.f, 20.f));
		pointLight->mat->SetColor(Vector3F(1.f, 0.f, 0.f));
		pointLight->mat->SetDiffuseIntensity(10.f);

		lineSystems.push_back(new LineSystem(3000));
		LineSystem* lSystem = lineSystems.front();

		bbSystems.push_back(new BoundingBoxSystem(6000));
		float rS = 1.f;
		

		for (int i = 0; i < 500; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment);
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment - 2)*100, (gridSize + increment)*100);
			//pos.x += x*gridSize;//playerPos;
			//pos.y += y*gridSize;//playerPos;
			//pos.z += z*gridSize;//playerPos;
			
			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("sphere", pos);
			sphere->mat->SetShininess(20.f);
			sphere->mat->SetSpecularIntensity(3.f);
			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 0.f, 1.f);

			//rS = ((rand() % 30) + 1.f) / 15.f;
			//sphere->SetScale(Vector3(rS, rS, rS));
			
			
			
		}
		
		for (int i = 0; i < 70; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment);
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment + 20)*100, (gridSize + increment + 22)*100);
			//pos.x += x*gridSize;//playerPos;
			//pos.y += y*gridSize;//playerPos;
			//pos.z += z*gridSize;//playerPos;

			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", pos);
			sphere->mat->SetShininess(20.f);
			sphere->mat->SetSpecularIntensity(3.f);
			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 3.f, 1.f);

			for (int j = 0; j < 3; j++)
			{
				Vector3 childPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-len, (int)len) / 4.f;
				double childLen = childPos.vectLengt();
				Object* child = Scene::Instance()->addObjectTo(sphere, "icosphere", childPos);
				child->mat->SetShininess(20.f);
				child->mat->SetSpecularIntensity(3.f);
				FastLine* line = lSystem->GetLine();

				line->AttachEndA(&sphere->node);
				line->AttachEndB(&child->node);

				line->colorA = Vector4F(1.f, 0.f, 0.f, 1.f);
				line->colorB = Vector4F(0.f, 1.f, 0.f, 1.f);

				//rS = ((rand() % 35) + 1.f) / 15.f;
				//child->SetScale(Vector3(rS, rS, rS));

				for (int k = 0; k < 5; k++)
				{
					Vector3 childOfChildPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-childLen, (int)childLen) / 2.f;
					Object* childOfChild = Scene::Instance()->addObjectTo(child, "sphere", childOfChildPos);
					childOfChild->mat->SetShininess(20.f);
					childOfChild->mat->SetSpecularIntensity(3.f);
					FastLine* line = lSystem->GetLine();

					line->AttachEndA(&child->node);
					line->AttachEndB(&childOfChild->node);

					line->colorA = Vector4F(6.f, 0.f, 0.f, 1.f);
					line->colorB = Vector4F(0.f, 0.f, 24.f, 1.f);

					//rS = ((rand() % 45) + 1.f) / 15.f;
					//childOfChild->SetScale(Vector3(rS, rS, rS));
				}

			}
		}
	
		Scene::Instance()->SceneObject->AddComponent(lSystem);
		Scene::Instance()->SceneObject->AddComponent(bbSystems.front());
	}

	void
	PickingApp::LoadScene4()
	{

		Vector3 playerPos = currentCamera->GetInitPos();
		int gridSize = 10;
		int x = (int)playerPos.x / gridSize;
		int y = (int)playerPos.y / gridSize;
		int z = (int)playerPos.z / gridSize;

		//if (prevGridPos[0] == x && prevGridPos[1] == y && prevGridPos[2] == z) return;
		//printf("%d %d %d\n", x, y, z);
		prevGridPos[0] = x;
		prevGridPos[1] = y;
		prevGridPos[2] = z;

		Clear();

		int xy = ((x + y) * (x + y + 1)) / 2 + y; //unique values for a pair, do it twice to get the unique for three
		int xyz = ((xy + z) * (xy + z + 1)) / 2 + z; //unique values for a pair, do it twice to get the unique for three		
		srand(xyz);
		lightsPhysics = false;

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.2f);

		lineSystems.push_back(new LineSystem(3000));
		LineSystem* lSystem = lineSystems.front();

		bbSystems.push_back(new BoundingBoxSystem(6000));
		float rS = 1.f;


		for (int i = 0; i < 700; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment); //cube
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment - 2) * 100, (gridSize + increment) * 100); //sphere
			//pos.x += x*gridSize;//add playerPos;
			//pos.y += y*gridSize;//add playerPos;
			//pos.z += z*gridSize;//add playerPos;

			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", pos);

			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 0.f, 1.f);

			//rS = ((rand() % 30) + 1.f) / 15.f;
			//sphere->SetScale(Vector3(rS, rS, rS));
			/*
			for (int j = 0; j < 3; j++)
			{
			Vector3 childPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-len, (int)len) / 4.f;
			float childLen = childPos.vectLengt();
			Object* child = Scene::Instance()->addObjectTo(sphere, "icosphere", childPos);

			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&sphere->node);
			line->AttachEndB(&child->node);

			line->colorA = Vector4(1.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4(0.f, 1.f, 0.f, 1.f);

			//rS = ((rand() % 35) + 1.f) / 15.f;
			//child->SetScale(Vector3(rS, rS, rS));

			for (int k = 0; k < 5; k++)
			{
			Vector3 childOfChildPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-childLen, (int)childLen) / 2.f;
			Object* childOfChild = Scene::Instance()->addObjectTo(child, "sphere", childOfChildPos);

			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&child->node);
			line->AttachEndB(&childOfChild->node);

			line->colorA = Vector4(6.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4(0.f, 0.f, 24.f, 1.f);

			//rS = ((rand() % 45) + 1.f) / 15.f;
			//childOfChild->SetScale(Vector3(rS, rS, rS));
			}

			}
			*/
		}

		for (int i = 0; i < 500; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment);
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment + 20) * 100, (gridSize + increment + 22) * 100);
			//pos.x += x*gridSize;//playerPos;
			//pos.y += y*gridSize;//playerPos;
			//pos.z += z*gridSize;//playerPos;

			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", pos);

			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 3.f, 1.f);
		}

		Scene::Instance()->SceneObject->AddComponent(lSystem);
		Scene::Instance()->SceneObject->AddComponent(bbSystems.front());
	}

	void
	PickingApp::LoadScene5()
	{

		Vector3 playerPos = currentCamera->GetInitPos();
		int gridSize = 10;
		int x = (int)playerPos.x / gridSize;
		int y = (int)playerPos.y / gridSize;
		int z = (int)playerPos.z / gridSize;

		//if (prevGridPos[0] == x && prevGridPos[1] == y && prevGridPos[2] == z) return;
		//printf("%d %d %d\n", x, y, z);
		prevGridPos[0] = x;
		prevGridPos[1] = y;
		prevGridPos[2] = z;

		Clear();
		//printf("%f", coord);
		int xy = ((x + y) * (x + y + 1)) / 2 + y; //unique values for a pair, do it twice to get the unique for three
		int xyz = ((xy + z) * (xy + z + 1)) / 2 + z; //unique values for a pair, do it twice to get the unique for three		
		srand(xyz);
		lightsPhysics = false;
		//currentCamera->SetPosition(Vector3(0.f, 10.f, 60.f));

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.2f);

		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.f, 0.f, 0.f));
		pointLight->SetScale(Vector3(20.f, 20.f, 20.f));
		pointLight->mat->SetColor(Vector3F(1.f, 0.f, 0.f));
		pointLight->mat->SetDiffuseIntensity(10.f);

		lineSystems.push_back(new LineSystem(3000));
		LineSystem* lSystem = lineSystems.front();

		bbSystems.push_back(new BoundingBoxSystem(6000));
		float rS = 1.f;


		for (int i = 0; i < 700; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment);
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment - 2) * 100, (gridSize + increment) * 100);
			//pos.x += x*gridSize;//playerPos;
			//pos.y += y*gridSize;//playerPos;
			//pos.z += z*gridSize;//playerPos;

			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", pos);

			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 0.f, 1.f);

			//rS = ((rand() % 30) + 1.f) / 15.f;
			//sphere->SetScale(Vector3(rS, rS, rS));
			/*
			for (int j = 0; j < 3; j++)
			{
			Vector3 childPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-len, (int)len) / 4.f;
			float childLen = childPos.vectLengt();
			Object* child = Scene::Instance()->addObjectTo(sphere, "icosphere", childPos);

			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&sphere->node);
			line->AttachEndB(&child->node);

			line->colorA = Vector4(1.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4(0.f, 1.f, 0.f, 1.f);

			//rS = ((rand() % 35) + 1.f) / 15.f;
			//child->SetScale(Vector3(rS, rS, rS));

			for (int k = 0; k < 5; k++)
			{
			Vector3 childOfChildPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-childLen, (int)childLen) / 2.f;
			Object* childOfChild = Scene::Instance()->addObjectTo(child, "sphere", childOfChildPos);

			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&child->node);
			line->AttachEndB(&childOfChild->node);

			line->colorA = Vector4(6.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4(0.f, 0.f, 24.f, 1.f);

			//rS = ((rand() % 45) + 1.f) / 15.f;
			//childOfChild->SetScale(Vector3(rS, rS, rS));
			}

			}
			*/
		}

		for (int i = 0; i < 500; i++)
		{
			//Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-gridSize + increment, gridSize + increment);
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical((gridSize + increment + 20) * 100, (gridSize + increment + 22) * 100);
			//pos.x += x*gridSize;//playerPos;
			//pos.y += y*gridSize;//playerPos;
			//pos.z += z*gridSize;//playerPos;

			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", pos);

			//it is better if we can attach the node to the object
			//to do that we simply set the pointer of the node of line to the object we want to follow
			//this will work for get lines but not for the generated ones,
			//generated ones set the position
			FastLine* line = lSystem->GetLine();

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&sphere->node);

			line->colorA = Vector4F(69.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 3.f, 1.f);
		}
		/*
		for (size_t i = 0; i < 500; i++)
		{
		Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical(17, 20);
		Scene::Instance()->addObjectToScene("cube", pos);
		}
		*/

		/*
		Object* plane = Scene::Instance()->addObjectToScene("sphere");
		plane->mat->SetSpecularIntensity(0.5f);
		plane->SetScale(Vector3(10.f, 0.5f, 10.f));
		//plane->mat->SetColor(Vector3(0.f,10.f,10.f));
		plane->mat->SetDiffuseIntensity(100.f);
		this->plane = plane;
		*/
		/*
		ParticleSystem* pSystem = new ParticleSystem(50000, 2000);
		pSystem->SetTexture(GraphicsStorage::textures[10]->TextureID);
		pSystem->SetLifeTime(5.0f);
		pSystem->SetColor(Vector4F(0.5f, 0.5f, 0.5f, 0.1f));
		pSystem->SetSize(1.0f);
		particleSystems.push_back(pSystem);

		Scene::Instance()->SceneObject->AddComponent(pSystem);
		*/
		Scene::Instance()->SceneObject->AddComponent(lSystem);
		Scene::Instance()->SceneObject->AddComponent(bbSystems.front());
	}

	void
	PickingApp::LoadScene3()
	{
		Clear();
		currentCamera->SetPosition(Vector3(0.0, 10.0, 60.0));

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.2f);

		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.0, 0.0, 0.0));
		pointLight->SetScale(Vector3(20.0, 20.0, 20.0));
		pointLight->mat->SetColor(Vector3F(1.f, 0.f, 0.f));
		pointLight->mat->SetDiffuseIntensity(10.f);

		BoundingBoxSystem* bbSystem = new BoundingBoxSystem(6000);
		LineSystem* lSystem = new LineSystem(4000);
		bbSystems.push_back(bbSystem);
		lineSystems.push_back(lSystem);
		PointSystem* poSys = new PointSystem(1000);
		pointSystems.push_back(poSys);

		float rS = 1.f;
		for (int i = 0; i < 1000; i++)
		{
			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorCubic(-80, 80);
			double len = pos.vectLengt();
			Object* object = Scene::Instance()->addObjectToScene("icosphere", pos);
			//object->mat->SetDiffuseIntensity(10.3f);
			RigidBody* body = new RigidBody(object);
			object->AddComponent(body);
			//PhysicsManager::Instance()->RegisterRigidBody(body);
			rS = (float)(rand() % 5) + 1.f;
			object->SetScale(Vector3(rS, rS, rS));
			body->SetCanSleep(false);

			FastLine* line = lSystem->GetLine();
			//Scene::Instance()->SceneObject->node.addChild(&line->nodeA);
			//object->node.addChild(&line->nodeB);

			line->AttachEndA(&Scene::Instance()->SceneObject->node);
			line->AttachEndB(&object->node);

			line->colorA = Vector4F(6.f, 0.f, 0.f, 1.f);
			line->colorB = Vector4F(3.f, 3.f, 0.f, 1.f);
		}
		
		lightsPhysics = true; //it has to update

		Scene::Instance()->SceneObject->AddComponent(bbSystem);
		Scene::Instance()->SceneObject->AddComponent(lSystem);
		PhysicsManager::Instance()->gravity = Vector3();
	}

	void
	PickingApp::FireLightProjectile()
	{
		Object* pointLight = Scene::Instance()->addPointLight(currentCamera->GetPosition2()+currentCamera->getDirection()*10.0, Vector3F(1.f, 1.f, 0.f));
		pointLight->mat->SetDiffuseIntensity(50.f);
		pointLight->mat->SetColor(Vector3F(1.f,0.f,0.f));
		pointLight->SetScale(Vector3(15.0, 15.0, 15.0));


		Object* icos = Scene::Instance()->addPhysicObject("icosphere", currentCamera->GetPosition2());
		Object* sphere = Scene::Instance()->addPhysicObject("sphere", currentCamera->GetPosition2() + currentCamera->getDirection()*10.0);

		RigidBody* body = new RigidBody(pointLight);
		pointLight->AddComponent(body);
		body->SetCanSleep(false);
		body->ApplyImpulse(currentCamera->getDirection()*3000.0, pointLight->GetLocalPosition());
		
		ParticleSystem* pSystem = new ParticleSystem(500, 170);
		pointLight->AddComponent(pSystem);
		pSystem->SetTexture(GraphicsStorage::textures[11]->TextureID);
		pSystem->SetDirection(Vector3F(0.0, 0.0, 0.0));
		pSystem->SetColor(Vector4F(50.f, 0.f, 0.f, 0.2f));
		pSystem->SetSize(0.2f);

		particleSystems.push_back(pSystem);

		PointSystem* pos = pointSystems[0];
		FastPoint* fpo = pos->GetPoint();
		fpo->color = Vector4F(0.f, 50.f, 50.f, 1.0f);
		fpo->node.position = currentCamera->GetPosition2() + currentCamera->getDirection()*16.0;

		FastLine* line = lineSystems[0]->GetLine();
		//Scene::Instance()->SceneObject->node.addChild(&line->nodeA);
		//object->node.addChild(&line->nodeB);

		//we can attach lines to another objects 
		line->AttachEndA(&icos->node);
		//we can set positions of lines
		line->SetPositionB(currentCamera->GetPosition2() + currentCamera->getDirection()*10.0);

		line->colorA = Vector4F(0.f, 5.f, 5.f, 1.f);
		line->colorB = Vector4F(0.f, 5.f, 5.f, 1.f);

		FastLine* line2 = lineSystems[0]->GetLine();
		//we can attach line to another line if line we are attaching to is using a node of an object
		//line2->AttachEndA(line->nodeA);
		line2->AttachEndA(&sphere->node);
		line2->SetPositionB(currentCamera->GetPosition2() + currentCamera->getDirection()*20.0);
		//we can apply some offsets to the attachments
		line2->SetPositionA(currentCamera->getDirection()*5.0);
		line2->colorA = Vector4F(5.f, 0.f, 0.f, 1.f);
		line2->colorB = Vector4F(5.f, 0.f, 0.f, 1.f);
	}

	void
	PickingApp::Vortex()
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
				body->ApplyImpulse(dir.vectNormalize()*-2000.f, obj->GetWorldPosition());
			}			
		}
	}

	void
	PickingApp::DrawGeometryPass()
	{
		//bind for geometry pass
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->geometryFrameBufferHandle);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, DrawBuffers);
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

	void
	PickingApp::DrawLightPass()
	{
		//commented out stuff that is now set up in different place
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE, GL_ONE);
		//start frame

		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->lightAndPostFrameBufferHandle); //we bind the lightandposteffect buffer for drawing
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);
		//glDrawBuffer(GL_COLOR_ATTACHMENT0); 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //added depth and stencil clear

		FBOManager::Instance()->BindFrameBuffer(read, FBOManager::Instance()->geometryFrameBufferHandle); //we enable geometry buffer for sampling the textures
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //we blit depth from geometry to light buffer

		//FBOManager::Instance()->BindGeometryBuffer(draw); // we draw now to the geometry buffer since it has the final color where the light is composited
		//glClear(GL_COLOR_BUFFER_BIT);
		GLuint pointLightS = ShaderManager::Instance()->shaderIDs["pointLight"];
		ShaderManager::Instance()->SetCurrentShader(pointLightS);
		GLuint screenSize = glGetUniformLocation(pointLightS, "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);
		GLuint CameraPos = glGetUniformLocation(pointLightS, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
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

	void
	PickingApp::DrawPointLights()
	{
		glEnable(GL_STENCIL_TEST);
		lightsRendered = 0;
		for (auto& pointLight : Scene::Instance()->pointLights)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(pointLight->GetWorldPosition(), pointLight->radius)) {
				StencilPass(pointLight); //sets up stencil pass
				PointLightPass(pointLight); //sets up point light pass	
				lightsRendered++;
			}
		}
		glDisable(GL_STENCIL_TEST);
	}

	void
	PickingApp::StencilPass(Object* pointLight)
	{
		//enable stencil shader 
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["stencil"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		glDrawBuffer(GL_NONE); //don't write to light buffers

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

	void
	PickingApp::PointLightPass(Object* pointLight)
	{
		//enable drawing of final color in light buffer 
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);
		//glDrawBuffer(GL_COLOR_ATTACHMENT4);
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

	void
	PickingApp::DrawDirectionalLights()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);
		//glDrawBuffer(GL_COLOR_ATTACHMENT4);
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

	void
	PickingApp::ActivateTextures()
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

	void
	PickingApp::BlitToScreenPass()
	{
		//FBOManager::Instance()->UnbindFrameBuffer(draw); //for drawing we are unbinding to the screen buffer
		FBOManager::Instance()->BindFrameBuffer(read, FBOManager::Instance()->lightAndPostFrameBufferHandle);  //and we read from the light buffer

		//glReadBuffer(GL_COLOR_ATTACHMENT4); //enable the final color texture buffer for reading
		//glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR); 
		glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST); //we need to blit depth only
		//FBOManager::Instance()->UnbindFrameBuffer(readDraw);
	}

	void
	PickingApp::MovePlaneUpNDown()
	{
		if (plane != nullptr)
		{
			if (plane->GetWorldPosition().y < -17) planeDir = 1;
			else if (plane->GetWorldPosition().y > 10) planeDir = -1;

			plane->Translate(Vector3(0.f, 0.1f*planeDir, 0.f));
		}
	}

	void
	PickingApp::DisableTextures()
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

	void
	PickingApp::SpawnSomeLights()
	{
		if (Scene::Instance()->pointLights.size() < 500)
		{
			Object* pointLight = Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::y), Scene::Instance()->generateRandomIntervallVectorCubic(0, 6000).toFloat() / 6000.f);
			Object* sphere = Scene::Instance()->addObjectToScene("sphere", pointLight->GetLocalPosition());
			sphere->SetScale(Vector3(0.1f, 0.1f, 0.1f));
			sphere->mat->diffuseIntensity = 2.f;
			sphere->mat->ambientIntensity = 1.5f;
			sphere->mat->specularIntensity = 4.f;
			sphere->mat->shininess = 10.f;
		}
	}

	void
	PickingApp::MouseCallback(double mouseX, double mouseY)
	{
		if (altButtonToggle)
		{
			currentCamera->UpdateOrientation(mouseX, mouseY);
			window->SetCursorPos(windowMidX, windowMidY);
		}
	}

	void
	PickingApp::SetUpCamera()
	{
		currentCamera = new Camera(Vector3(0.f, 10.f, 60.f), windowWidth, windowHeight);
		currentCamera->Update((float)Time::timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		currentCamera->SetFarPlane(20000.0);
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void
	PickingApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
		//ShaderManager::Instance()->AddShader("picking", GraphicsManager::LoadShaders("Resources/Shaders/VSPicking.glsl", "Resources/Shaders/FSPicking.glsl"));
		ShaderManager::Instance()->AddShader("wireframe", GraphicsManager::LoadShaders("Resources/Shaders/VSBB.glsl", "Resources/Shaders/FSBB.glsl"));
		ShaderManager::Instance()->AddShader("dftext", GraphicsManager::LoadShaders("Resources/Shaders/VSDFText.glsl", "Resources/Shaders/FSDFText.glsl"));
		ShaderManager::Instance()->AddShader("depth", GraphicsManager::LoadShaders("Resources/Shaders/VSDepth.glsl", "Resources/Shaders/FSDepth.glsl"));
		ShaderManager::Instance()->AddShader("depthPanel", GraphicsManager::LoadShaders("Resources/Shaders/VSShadowMapPlane.glsl", "Resources/Shaders/FSShadowMapPlane.glsl"));
		ShaderManager::Instance()->AddShader("blur", GraphicsManager::LoadShaders("Resources/Shaders/VSBlur.glsl", "Resources/Shaders/FSBlur.glsl"));
		//ShaderManager::Instance()->AddShader("blur2", GraphicsManager::LoadShaders("Resources/Shaders/VSBlur.glsl", "Resources/Shaders/FSBlur2.glsl"));
		ShaderManager::Instance()->AddShader("geometry", GraphicsManager::LoadShaders("Resources/Shaders/VSGeometry.glsl", "Resources/Shaders/FSGeometry.glsl"));
		ShaderManager::Instance()->AddShader("pointLight", GraphicsManager::LoadShaders("Resources/Shaders/VSPointLight.glsl", "Resources/Shaders/FSPointLight.glsl"));
		ShaderManager::Instance()->AddShader("directionalLight", GraphicsManager::LoadShaders("Resources/Shaders/VSDirectionalLight.glsl", "Resources/Shaders/FSDirectionalLight.glsl"));
		ShaderManager::Instance()->AddShader("stencil", GraphicsManager::LoadShaders("Resources/Shaders/VSStencil.glsl", "Resources/Shaders/FSStencil.glsl"));
		ShaderManager::Instance()->AddShader("particle", GraphicsManager::LoadShaders("Resources/Shaders/VSParticle.glsl", "Resources/Shaders/FSParticle.glsl"));
		ShaderManager::Instance()->AddShader("hdrBloom", GraphicsManager::LoadShaders("Resources/Shaders/VSHDRBloom.glsl", "Resources/Shaders/FSHDRBloom.glsl"));
		ShaderManager::Instance()->AddShader("fastLine", GraphicsManager::LoadShaders("Resources/Shaders/VSFastLine.glsl", "Resources/Shaders/FSFastLine.glsl"));
		ShaderManager::Instance()->AddShader("fastBB", GraphicsManager::LoadShaders("Resources/Shaders/VSFastBB.glsl", "Resources/Shaders/FSFastBB.glsl"));
	}

	void
	PickingApp::DrawHDR()
	{
		//we draw color to the screen
		GLuint hdrBloom = ShaderManager::Instance()->shaderIDs["hdrBloom"];
		ShaderManager::Instance()->SetCurrentShader(hdrBloom);

		GLuint hdrEnabled = glGetUniformLocation(hdrBloom, "hdr");
		GLuint exposure = glGetUniformLocation(hdrBloom, "exposure");
		GLuint gamma = glGetUniformLocation(hdrBloom, "gamma");
		GLuint bloomIntensity = glGetUniformLocation(hdrBloom, "bloomIntensity");
		
		glUniform1i(hdrEnabled, GL_TRUE);
		glUniform1f(exposure, 1.0f);
		glUniform1f(gamma, 1.2f);
		glUniform1f(bloomIntensity, 0.5f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->finalColorTextureHandle);
		GLuint hdrBuffer = glGetUniformLocation(hdrBloom, "hdrBuffer");
		glUniform1i(hdrBuffer, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle2[0]); //blurred bright light(bloom)
		GLuint bloomBuffer = glGetUniformLocation(hdrBloom, "bloomBuffer");
		glUniform1i(bloomBuffer, 1);

		DebugDraw::Instance()->DrawQuad();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void
	PickingApp::BlurLight()
	{
		//blit bright color texture to blur frame buffer
		//FBOManager::Instance()->BindFrameBuffer(read, FBOManager::Instance()->lightAndPostFrameBufferHandle);  //and we read from the light buffer
		//glReadBuffer(GL_COLOR_ATTACHMENT1); //enable the bright color texture buffer for reading
		//FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[0]); //for drawing we are binding to blurFrameBuffer 0
		//glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//FBOManager::Instance()->UnbindFrameBuffer(readDraw); //we unbind to screen but during blur we bind to two blur buffers

		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["blur"]);
		/*
		GLuint horizontal = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "horizontal");

		for (int i = 0; i < 10; i++){
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOManager::Instance()->blurFrameBufferHandle[1]);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(horizontal, GL_TRUE);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[0]);

			DebugDraw::Instance()->DrawQuad();


			FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[0]); //final blur result is stored in the blurFrameBufferHandle 0
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i(horizontal, GL_FALSE);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[1]);

			DebugDraw::Instance()->DrawQuad();
		}
		*/
		
		glViewport(0, 0, windowWidth / FBOManager::Instance()->scaleBlur, windowHeight / FBOManager::Instance()->scaleBlur);
		GLuint scaleUniform = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "scaleUniform");
		
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 1.0f / ((float)windowWidth), 0.0f); //horizontally

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->brightLightBufferHandle);

		DebugDraw::Instance()->DrawQuad();

		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[0]); //final blur result is stored in the blurFrameBufferHandle 0
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 0.0f, 1.0f / ((float)windowHeight)); //vertically

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[1]);

		DebugDraw::Instance()->DrawQuad();



		glViewport(0, 0, windowWidth / FBOManager::Instance()->scaleBlur2, windowHeight / FBOManager::Instance()->scaleBlur2);

		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle2[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 1.0f / ((float)windowWidth), 0.0f); //horizontally

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[0]);

		DebugDraw::Instance()->DrawQuad();

		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle2[0]); //final blur result is stored in the blurFrameBufferHandle2 0
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 0.0f, 1.0f / ((float)windowHeight)); //vertically

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle2[1]);

		DebugDraw::Instance()->DrawQuad();
		/*
		for (int i = 0; i < 10; i++){
			FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[1]);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 1.0f / (float)windowWidth, 0.0f); //horizontally

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[0]);

			DebugDraw::Instance()->DrawQuad();


			FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->blurFrameBufferHandle[0]); //final blur result is stored in the blurFrameBufferHandle 0
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 0.0f, 1.0f / (float)windowHeight); //vertically

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->blurBufferHandle[1]);

			DebugDraw::Instance()->DrawQuad();
		}
		*/
		FBOManager::Instance()->UnbindFrameBuffer(draw); //here we unbind to screen
		glViewport(0, 0, windowWidth, windowHeight);
	}

	void
	PickingApp::DrawFastLineSystems()
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->lightAndPostFrameBufferHandle); //we bind the lightandposteffect buffer for drawing
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);
		
		GLuint fastLineShader = ShaderManager::Instance()->shaderIDs["fastLine"];
		ShaderManager::Instance()->SetCurrentShader(fastLineShader);
		for (auto& lSystem : lineSystems) 
		{
			//lSystem->Update();
			lSystem->Draw(CameraManager::Instance()->ViewProjection, fastLineShader, 1.f);
		}

		FBOManager::Instance()->UnbindFrameBuffer(draw);
		glDepthMask(GL_FALSE);
	}

	void
	PickingApp::DrawFastPointSystems()
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->lightAndPostFrameBufferHandle); //we bind the lightandposteffect buffer for drawing
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);

		GLuint fastLineShader = ShaderManager::Instance()->shaderIDs["fastLine"];
		ShaderManager::Instance()->SetCurrentShader(fastLineShader);
		for (auto& poSystem : pointSystems)
		{
			//poSystem->Update();
			poSystem->Draw(CameraManager::Instance()->ViewProjection, fastLineShader, 5.f);
		}

		FBOManager::Instance()->UnbindFrameBuffer(draw);
		glDepthMask(GL_FALSE);
	}

	void
	PickingApp::GenerateFastLines()
	{
		if (lineSystems.empty()) return;
		for (auto& child : Scene::Instance()->SceneObject->node.children)
		{
			GenerateAndDrawFastLineChildren(&Scene::Instance()->SceneObject->node, child);
		}
	}

	void
	PickingApp::GenerateAndDrawFastLineChildren(Node* parent, Node* child)
	{
		FastLine* line = lineSystems.front()->GetLineOnce();
		line->AttachEndA(parent);
		line->AttachEndB(child);
		for (auto& childOfChild : child->children)
		{
			GenerateAndDrawFastLineChildren(child, childOfChild);
		}
	}

	void
	PickingApp::DrawDebugInstanced()
	{
		if (bbSystems.empty()) return;

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		//the outcommented code is for case we want to render to post effect buffer so the bbs will be affected by effects
		//FBOManager::Instance()->BindFrameBuffer(draw, FBOManager::Instance()->lightAndPostFrameBufferHandle); //we bind the lightandposteffect buffer for drawing
		//GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		//glDrawBuffers(2, DrawBuffers);

		GLuint fastBBShader = ShaderManager::Instance()->shaderIDs["fastBB"];
		ShaderManager::Instance()->SetCurrentShader(fastBBShader);

		for (auto& obj : PhysicsManager::Instance()->satOverlaps)
		{
			obj.rbody1->aabb.color = Vector3F(1.f, 0.f, 0.f);
			obj.rbody2->aabb.color = Vector3F(1.f, 0.f, 0.f);
		}

		BoundingBoxSystem* bbSystem = bbSystems.front();

		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius))
			{
				if (RigidBody* body = obj.second->GetComponent<RigidBody>())
				{
					FastBoundingBox* fastOBB = bbSystem->GetBoundingBoxOnce();
					fastOBB->color = Vector4F(body->obb.color, 1.f);
					fastOBB->node.TopDownTransform = Matrix4::scale(obj.second->GetMeshDimensions())*obj.second->node.TopDownTransform;

					FastBoundingBox* fastAABB = bbSystem->GetBoundingBoxOnce();
					fastAABB->color = Vector4F(body->aabb.color, 1.f);
					fastAABB->node.TopDownTransform = body->aabb.model;
				}
			}
		}

		bbSystem->Update();
		bbSystem->Draw(CameraManager::Instance()->ViewProjection, fastBBShader);

		//FBOManager::Instance()->UnbindFrameBuffer(draw);
		glDepthMask(GL_FALSE);
	}

} 