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
            this->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, 0.1f, 100.f);

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

        FBOManager::Instance()->SetUpFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpBlurFrameBuffer(this->windowWidth, this->windowHeight);
		FBOManager::Instance()->SetUpDeferredFrameBuffer(this->windowWidth, this->windowHeight);

        Object* Scene = Scene::Instance()->build();

		GraphicsManager::LoadAllAssets();

		DebugDraw::Instance()->LoadPrimitives();

		boundingBox = &DebugDraw::Instance()->boundingBox;
		
		LoadScene4();

		float timeStep = 0.016f + timeModifier;
		float dtInv = 1.f / timeStep;
        // For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera(timeStep);

		double fps_timer = 0;
		Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());
		timeCounter = 0.0;
        while (running)
        {
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			glDisable(GL_BLEND);
            this->window->Update();

            // Measure FPS
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
			timeCounter += deltaTime;

			Monitor(this->window);

			//is cursor window locked
            if (altButtonToggle)
            {
				// Compute the view matrix from keyboard and mouse input
                currentCamera->Update((float)deltaTime);
            }
            ViewMatrix = currentCamera->getViewMatrix();
			FrustumManager::Instance()->ExtractPlanes(ProjectionMatrix, ViewMatrix);
			
			timeStep = 0.016f + timeModifier;
			dtInv = 1.f / timeStep;
			if (paused) timeStep = 0.f, dtInv = 0;

			switch (currentScene)
			{
				case scene1Loaded:
					if (currentTime - fps_timer >= 0.2f){
						//SpawnSomeLights();
					}
					break;

				case scene4Loaded:
					if (currentTime - fps_timer >= 0.2f){
						Vortex();
					}
					MoveObjectUpNDown();
					break;				
			}
			
			PhysicsManager::Instance()->SortAndSweep();
			PhysicsManager::Instance()->NarrowTestSAT(dtInv);
			
			IntegrateAndUpdateBoxes(timeStep);
			
			//IntegrateLights(timeStep);
			
			Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());
			Scene::Instance()->MainPointLight->node.UpdateNodeMatrix(Matrix4::identityMatrix());
			Scene::Instance()->MainDirectionalLight->node.UpdateNodeMatrix(Matrix4::identityMatrix());
			PassPickingTexture(ProjectionMatrix, ViewMatrix); //picking

			DrawGeometryPass(ProjectionMatrix, ViewMatrix);

			PickingTest();

			if (debug) DrawDebug(ProjectionMatrix, ViewMatrix);
			
			DrawLightPass(ProjectionMatrix, ViewMatrix, currentCamera->GetPosition2());

			BlitToScreenPass();

			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			
			FBOManager::Instance()->DrawGeometryMaps(windowWidth, windowHeight);

			if (currentTime - fps_timer >= 0.2f){ 
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " Lights rendered: " + std::to_string(lightsRendered) + " FPS: " + std::to_string(1.f / deltaTime) + " TimeStep: " + std::to_string(timeStep) + " PickedID: " + std::to_string(pickedID) + (paused ? " PAUSED" : ""));
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
				LoadScene1();
			}
			else if (key == GLFW_KEY_2) {
				LoadScene2();
			}
			else if (key == GLFW_KEY_3) {
				LoadScene3();
			}
			else if (key == GLFW_KEY_4) {
				LoadScene4();
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
				timeModifier = 0.f;
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
		if (window->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) timeModifier += 0.0005f;
		if (window->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) timeModifier -= 0.0005f;
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
    PickingApp::PassPickingTexture(const Matrix4& Projection, const Matrix4& View)
    {
		
		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		Draw(Projection, View);
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
			unsigned char Pixel[4];
			//inverted y coordinate because glfw 0,0 starts at topleft while opengl texture 0,0 starts at bottomleft
			FBOManager::Instance()->ReadPixelID((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, Pixel);
			pickedID = Pixel[0] + Pixel[1] * 256 + Pixel[2] * 256 * 256;

			//std::cout << pickedID << std::endl;
			if(lastPickedObject != nullptr) //reset previously picked object color
			{
				if (std::find(Scene::Instance()->pointLights.begin(), Scene::Instance()->pointLights.end(), lastPickedObject) == Scene::Instance()->pointLights.end())
				{
					lastPickedObject->mat->color = Vector3(0.f, 0.f, 0.f);
				}
			}  
			if(Scene::Instance()->objectsToRender.find(pickedID) != Scene::Instance()->objectsToRender.end())
			{
				lastPickedObject = Scene::Instance()->objectsToRender[pickedID];
				lastPickedObject->mat->color = Vector3(2.f,1.f,0.f);
			
				Vector3 world_position;
				FBOManager::Instance()->ReadWorldPos((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, world_position.vect);
				//Vector3 mouseInWorld = ConvertMousePosToWorld();
			
				Vector3 impulse = (world_position - currentCamera->GetPosition()).vectNormalize();
				this->lastPickedObject->ApplyImpulse(impulse, 20.f, world_position);
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

		Vector4 my_mouse_in_world_space = this->ProjectionMatrix.inverse() * mouse_p0s;
		my_mouse_in_world_space = this->ViewMatrix.inverse() * my_mouse_in_world_space;
		my_mouse_in_world_space = my_mouse_in_world_space / my_mouse_in_world_space[3];

		Vector3 my_mouse_in_world_space_vec3(my_mouse_in_world_space[0],my_mouse_in_world_space[1], my_mouse_in_world_space[2]);
		return my_mouse_in_world_space_vec3;
	}

    void
    PickingApp::InitGL()
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
		glUniform3f(LightDir, lightInvDir.x, lightInvDir.y, lightInvDir.z);

        this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
        ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, 0.1f, 200.0f);
		DebugDraw::Instance()->Projection = &ProjectionMatrix;
		DebugDraw::Instance()->View = &ViewMatrix;
    }

    void
	PickingApp::Draw(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
    {
        objectsRendered = 0;
        for(auto& obj : Scene::Instance()->objectsToRender)
        {
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetPosition(), obj.second->radius)) {
                obj.second->draw(ProjectionMatrix, ViewMatrix);
                objectsRendered++;
            }
        }
    }

	void PickingApp::DrawGeometry(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetPosition(), obj.second->radius)) {
				obj.second->drawGeometry(ProjectionMatrix, ViewMatrix);
				objectsRendered++;
			}
		}
	}

	void
	PickingApp::DrawDebug(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		FBOManager::Instance()->BindGeometryBuffer(draw);
		GLenum DrawDebugBuffers[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(2, DrawDebugBuffers);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		
		GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
		GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
		ShaderManager::Instance()->SetCurrentShader(wireframeShader);
		for (auto& obj : PhysicsManager::Instance()->satOverlaps)
		{
			obj.ent1->aabb.color = Vector3(1.f, 0.f, 0.f);
			obj.ent2->aabb.color = Vector3(1.f, 0.f, 0.f);
		}

		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetPosition(), obj.second->radius))
			{
				boundingBox->mat->SetColor(obj.second->obb.color);
				boundingBox->Draw(Matrix4::scale(obj.second->GetMeshDimensions())*obj.second->node.TopDownTransform, ViewMatrix, ProjectionMatrix, wireframeShader);
				boundingBox->mat->SetColor(obj.second->aabb.color);
				boundingBox->Draw(obj.second->aabb.model, ViewMatrix, ProjectionMatrix, wireframeShader);
			}
		}

		glDepthMask(GL_FALSE);
		ShaderManager::Instance()->SetCurrentShader(prevShader);
		FBOManager::Instance()->UnbindFrameBuffer(draw);
	}

    void 
	PickingApp::IntegrateAndUpdateBoxes(float timestep)
    {
		for(auto& obj : Scene::Instance()->objectsToRender)
		{
			obj.second->IntegrateRunge3(timestep, PhysicsManager::Instance()->gravity);
			obj.second->UpdateBoundingBoxes(DebugDraw::Instance()->boundingBox);
			obj.second->UpdateInertiaTensor();
		}
    }

	void 
	PickingApp::LoadScene1()
	{
		currentScene = scene1Loaded;
		//A plank suspended on a static box.	
		Clear();

		Object* sphere = Scene::Instance()->addPhysicObject("sphere", Vector3(0.f, -7.f, 0.f));
		sphere->isKinematic = true;
		sphere->SetMass(FLT_MAX);

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);
		//directionalLight->mat->SetSpecularColor(0, 0, 0);
		//directionalLight->mat->SetColor(0.5,0,0.5);
		//directionalLight->mat->SetDiffuseIntensity(1.5f);
		Object* pointLight = Scene::Instance()->addPointLight(Vector3(0.f, -7.f, 3.f));
		//Object* sphere1 = Scene::Instance()->addObject("sphere");
		//sphere1->isKinematic = true;
		//sphere1->SetMass(FLT_MAX);
		//pointLight->node.addChild(&sphere1->node);

		//pointLight->mat->SetSpecularColor(10, 10, 10);
		lastPickedObject = pointLight;
		//pointLight = Scene::Instance()->addPointLight(Vector3(8, 5, 10));
		//pointLight->mat->SetColor(1, 1, 1);
		

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		plane->SetScale(25, 2, 25);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;
	}

	void 
	PickingApp::LoadScene2()
	{
		currentScene = scene1Loaded;
		//A stack of boxes.
		Clear();

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetDiffuseIntensity(0.5f);
		Object* pointLight = Scene::Instance()->addPointLight(Vector3(4.f, 8.f, 4.f));
		pointLight->SetScale(10, 10, 10);
		lastPickedObject = pointLight;

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, 0.f, 0.f));
		plane->SetScale(25, 2, 25);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;

		for (int i = 0; i < 10; i++)
		{
			Object* cube = Scene::Instance()->addPhysicObject("spaceship");
			cube->SetPosition(Vector3(0.f, i * 2 + plane->GetPosition().y, 0.f));
		}
	}

	void 
	PickingApp::LoadScene3()
	{
		currentScene = scene1Loaded;
		//Boxes sliding of a static plane oriented at an angle.
		Clear();
		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		//directionalLight->mat->SetDiffuseIntensity(0);
		Object* pointLight = Scene::Instance()->addPointLight(Vector3(4.f, -5.f, 4.f));
		lastPickedObject = pointLight;

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		plane->SetScale(25, 2, 25);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;
		plane->SetOrientation(Quaternion(0.7f, Vector3(0.f, 0.f, 1.f)));

		Scene::Instance()->addRandomlyPhysicObjects("cube", 50);
	}

	void PickingApp::LoadScene4()
	{
		currentScene = scene4Loaded;
		Clear();

		Object* directionalLight = Scene::Instance()->addDirectionalLight(lightInvDir);
		directionalLight->mat->SetColor(0.2f, 0.2f, 0.2f);

		Scene::Instance()->addRandomlyPointLights(300);
		Scene::Instance()->addRandomlyObjects("sphere", 500);

		Object* plane = Scene::Instance()->addObject("cube");
		plane->SetScale(25.f, 0.2f, 25.f);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;
		lastPickedObject = plane;
	}

	void 
	PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
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
			Vector3 dir = obj->GetPosition() - Vector3(0.f, -10.f, 0.f);
			obj->ApplyImpulse(dir.vectNormalize()*-2000.f, obj->GetPosition());
		}
	}

	void PickingApp::DrawGeometryPass(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		//start frame
		FBOManager::Instance()->BindGeometryBuffer(draw);
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		glClear(GL_COLOR_BUFFER_BIT);

		//bind for geometry pass
		FBOManager::Instance()->BindGeometryBuffer(draw);

		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, DrawBuffers);
		//glViewport(0, 0, 2048, 2048);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["geometry"]);
		DrawGeometry(ProjectionMatrix, ViewMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDepthMask(GL_FALSE);
		//glDisable(GL_DEPTH_TEST);
		FBOManager::Instance()->UnbindFrameBuffer(draw); //we don't have to unbind we work all the way with the buffer but i prefer to do it anyway and enable when needed
	}

	void PickingApp::DrawLightPass(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix, const Vector3& camPos)
	{
		//commented out stuff that is now set up in different place
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_ONE, GL_ONE);

		FBOManager::Instance()->BindGeometryBuffer(draw); // we draw now to the geometry buffer since it has the final color where the light is composited
		//glClear(GL_COLOR_BUFFER_BIT);

		DrawPointLights(ProjectionMatrix, ViewMatrix, camPos);
		DrawDirectionalLights(ProjectionMatrix, ViewMatrix);
		DisableTextures();
		FBOManager::Instance()->UnbindFrameBuffer(draw);
	}

	void PickingApp::DrawPointLights(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix, const Vector3& camPos)
	{
		glEnable(GL_STENCIL_TEST);
		int pointLightsNum = Scene::Instance()->pointLights.size();
		lightsRendered = 0;
		for (int i = 0; i < pointLightsNum; i++)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(Scene::Instance()->pointLights[i]->GetPosition(), Scene::Instance()->pointLights[i]->radius)) {
				StencilPass(Scene::Instance()->pointLights[i], ProjectionMatrix, ViewMatrix, camPos); //sets up stencil pass
				PointLightPass(Scene::Instance()->pointLights[i], ProjectionMatrix, ViewMatrix, camPos); //sets up point light pass	
				lightsRendered++;
			}
		}
		glDisable(GL_STENCIL_TEST);
	}

	void PickingApp::StencilPass(Object* pointLight, const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix, const Vector3& camPos)
	{
		//enable stencil shader 
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["stencil"]);

		glDrawBuffer(GL_NONE); //don't write to geometry buffers

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_CULL_FACE);

		glClear(GL_STENCIL_BUFFER_BIT);

		// We need the stencil test to be enabled but we want it
		// to succeed always. Only the depth test matters.
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		pointLight->drawLight(ProjectionMatrix, ViewMatrix, camPos);
	}

	void PickingApp::PointLightPass(Object* pointLight, const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix, const Vector3& camPos)
	{
		//enable drawing of final color in geometry buffer 
		glDrawBuffer(GL_COLOR_ATTACHMENT4);

		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["pointLight"]);
		ActivateTextures();

		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		GLuint screenSize = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);
		pointLight->drawLight(ProjectionMatrix, ViewMatrix, camPos);
		
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);
	}

	void PickingApp::DrawDirectionalLights(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT4);
		//set 
		//directional shader
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["directionalLight"]);
		ActivateTextures();

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		GLuint screenSize = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);

		//lightinvdir
		GLuint LightDir = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);
		
		int directionalLightsNum = Scene::Instance()->directionalLights.size();
		for (int i = 0; i < directionalLightsNum; i++)
		{
			Scene::Instance()->directionalLights[i]->drawLight(Matrix4::identityMatrix(), Matrix4::identityMatrix(), Vector3());
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
		FBOManager::Instance()->UnbindFrameBuffer(read);
	}

	void PickingApp::MoveObjectUpNDown()
	{
		if (lastPickedObject != nullptr)
		{
			if (lastPickedObject->GetPosition().y < -17) planeDir = 1;
			else if (lastPickedObject->GetPosition().y > 10) planeDir = -1;

			lastPickedObject->Translate(0.f, 0.1f*planeDir, 0.f);
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
		Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVector(-20, 20), Scene::Instance()->generateRandomIntervallVector(0, 255) / 155.f);
		Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVector(-20, 20), Scene::Instance()->generateRandomIntervallVector(0, 255) / 155.f);
		Scene::Instance()->addPointLight(Scene::Instance()->generateRandomIntervallVector(-20, 20), Scene::Instance()->generateRandomIntervallVector(0, 255) / 155.f);
	}

	void PickingApp::IntegrateLights(float timestep)
	{
		for (auto& obj : Scene::Instance()->pointLights)
		{
			obj->IntegrateRunge3(timestep, PhysicsManager::Instance()->gravity);
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

	void PickingApp::SetUpCamera(float timeStep)
	{
		currentCamera = new Camera(Vector3(0.f, 10.f, 60.f), windowWidth, windowHeight);
		currentCamera->Update(timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
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
	}

	



} // namespace Example