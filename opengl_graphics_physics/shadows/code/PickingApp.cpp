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

		GraphicsManager::LoadAllAssets();

		DebugDraw::Instance()->LoadPrimitives();

		boundingBox = &DebugDraw::Instance()->boundingBox;
		
		LoadScene1();

        // For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowWidth/2.f, windowHeight/2.f+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		double fps_timer = 0;
		Node initNode = Node();
		Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);

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
			
			Time::timeStep = 0.016 + Time::timeModifier;
			if (Time::timeStep == 0.0) Time::dtInv = 0.0;
			else Time::dtInv = 1.0 / Time::timeStep;

			if (paused) Time::timeStep = 0.0, Time::dtInv = 0.0;

			if (scene4loaded) if (Time::currentTime - fps_timer >= 0.2) Vortex();
			
			PhysicsManager::Instance()->SortAndSweep();
			PhysicsManager::Instance()->NarrowTestSAT((float)Time::dtInv);
			
			UpdateComponents();
			Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);
			
			PassPickingTexture(); //picking
			PickingTest();

			DrawDepthPass(); 
			BlurShadowMap();
			//render to screen
			//glViewport(0, 0, windowWidth, windowHeight);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); i clear screen above instead, fbo is cleared inside passes
			DrawColorDebugPass(); // color || debug
			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			FBOManager::Instance()->DrawShadowMap(windowWidth, windowHeight);
			if (Time::currentTime - fps_timer >= 0.2){
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " FPS: " + std::to_string(1.0 / Time::deltaTime) + " TimeStep: " + std::to_string(Time::timeStep) + " PickedID: " + std::to_string(pickedID) + (paused ? " PAUSED" : ""));
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
	PickingApp::BlurShadowMap()
	{
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["blur"]);
		GLuint scaleUniform = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "scaleUniform");

		for (int i = 0; i < 2; i++){

			FBOManager::Instance()->BindBlurFrameBuffer(draw);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 1.0f / (float)windowWidth, 0.0f); //horizontally

			//Bind shadow map to be blurred
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapHandle);
			
			DebugDraw::Instance()->DrawQuad(); 

			FBOManager::Instance()->BindFrameBuffer(draw);
			glDrawBuffer(GL_COLOR_ATTACHMENT2);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 0.0f, 1.0f / (float)windowHeight); //vertically

			//Bind shadow map to be blurredblurred
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapBlurdHandle);

			DebugDraw::Instance()->DrawQuad();
		}
		FBOManager::Instance()->UnbindFrameBuffer(draw);
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
				scene4loaded = false;
				LoadScene1();
			}
			else if (key == GLFW_KEY_2) {
				scene4loaded = false;
				LoadScene2();
			}
			else if (key == GLFW_KEY_3) {
				scene4loaded = false;
				LoadScene3();
			}
			else if (key == GLFW_KEY_4) {
				scene4loaded = true;
				LoadScene4();
			}
			else if (key == GLFW_KEY_5) {
				scene4loaded = false;
				LoadScene5();
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
				cube->SetPosition(Vector3(0, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
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
	PickingApp::DrawDepthPass()
	{

		// Compute the MVP matrix from the light's point of view

		//this projection and view work as directional light

		//left right bottom top near far
		float left = -100, right = 100, bottom = -100, top = 100, near = -100, far = 200;
		Matrix4 depthProjectionMatrix = Matrix4::orthographic(near, far, left, right, top, bottom);

		//eye target up
		Matrix4 depthViewMatrix = Matrix4::lookAt(lightInvDir, Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));
		depthViewMatrix = Matrix4::translate(Vector3(-1.f, 0.f, -1.f)*(CameraManager::Instance()->GetCurrentCamera()->GetPosition2() + Vector3(0.f, -3.f, 26.f)))*depthViewMatrix;

		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(1, DrawBuffers);
		//glViewport(0, 0, 2048, 2048);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawDepth(depthProjectionMatrix, depthViewMatrix);
		FBOManager::Instance()->UnbindFrameBuffer(draw);
		
		//DebugDraw::Instance()->DrawNormal(lightInvDir.vectNormalize(),Vector3(0,0,0));
	}

    void
	PickingApp::DrawColorDebugPass()
    {
		//we set shadowMap for color shader only once for all objects
		GLuint colorShader = ShaderManager::Instance()->shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(colorShader);
		GLuint ShadowMapHandle = glGetUniformLocation(colorShader, "shadowMapSampler");
		//depth texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapHandle);
		glUniform1i(ShadowMapHandle, 1);

		GLuint CameraPos = glGetUniformLocation(colorShader, "CameraPos");
		Vector3 camPos = currentCamera->GetPosition2();
		glUniform3fv(CameraPos, 1, &camPos.x);

		if (debug)
		{
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
			Draw();
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["wireframe"]);
			DrawDebug();
		}
		else
		{
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
			Draw();
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    PickingApp::PassPickingTexture()
    {
		
		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		Draw();
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
			if (lastPickedObject != nullptr) //reset previously picked object color
			{
				lastPickedObject->mat->color = Vector3(0.f, 0.f, 0.f);
			}
			if (Scene::Instance()->objectsToRender.find(pickedID) != Scene::Instance()->objectsToRender.end())
			{
				lastPickedObject = Scene::Instance()->objectsToRender[pickedID];
				lastPickedObject->mat->color = Vector3(2.f, 1.f, 0.f);

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
		Vector4 mouse_p0s(x, y, 0.0, 0.0);
		mouse_p0s[0] = ((float)x / (float)windowWidth)*2.f - 1.f;
		mouse_p0s[1] = (((float)windowHeight - (float)y) / windowHeight)*2.f - 1.f;
		mouse_p0s[2] = -1.f;
		mouse_p0s[3] = 1.f;
		
		Vector4 my_mouse_in_world_space = currentCamera->ProjectionMatrix.inverse() * mouse_p0s;
		my_mouse_in_world_space = currentCamera->getViewMatrix().inverse() * my_mouse_in_world_space;
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
	PickingApp::DrawDepth(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		//Matrix4 model = Matrix4::translate(CameraManager::Instance()->GetCurrentCamera()->GetPosition2());
		//Matrix4 ViewProjection = model*ViewMatrix*ProjectionMatrix;
		Matrix4 ViewProjection = ViewMatrix*ProjectionMatrix;
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["depth"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			Render::drawDepth(obj.second, ViewProjection, currentShaderID);
		}
	}

	void
	PickingApp::DrawDebug()
	{
		GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
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
	}

    void 
	PickingApp::UpdateComponents()
    {
		Scene::Instance()->SceneObject->Update();
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			obj.second->Update();
			obj.second->CalculateRadius();
		}
    }

	void 
	PickingApp::LoadScene1()
	{
		//A plank suspended on a static box.	
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		RigidBody* body = plane->GetComponent<RigidBody>();
		plane->SetScale(Vector3(200.f, 1.f, 200.f));
		body->SetMass(FLT_MAX);
		body->isKinematic = true;
		plane->mat->SetSpecularIntensity(1.f);

		Object* plank = Scene::Instance()->addPhysicObject("cube", plane->GetLocalPosition() + Vector3(0.f, 10.f, 0.f));
		plank->SetScale(Vector3(3.f, 0.5f, 1.f));
		Object* cube = Scene::Instance()->addPhysicObject("cube", plane->GetLocalPosition() + Vector3(0.f, 2.f, 0.f));
		body = cube->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);

		Object* sphere = Scene::Instance()->addPhysicObject("sphere", plane->GetLocalPosition() + Vector3(0.f, 10.f, 30.f));
		sphere->mat->SetSpecularIntensity(1.f);
		sphere->SetScale(Vector3(10.f, 10.f, 10.f));
		body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);

		sphere = Scene::Instance()->addPhysicObject("sphere", plane->GetLocalPosition() + Vector3(60.f, 10.f, 60.f));
		sphere->mat->SetSpecularIntensity(1.f);
		sphere->SetScale(Vector3(10.f, 10.f, 10.f));
		body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);

		sphere = Scene::Instance()->addPhysicObject("sphere", plane->GetLocalPosition() + Vector3(100.f, 10.f, 0.f));
		sphere->mat->SetSpecularIntensity(1.f);
		sphere->SetScale(Vector3(10.f, 10.f, 10.f));
		body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);

		sphere = Scene::Instance()->addPhysicObject("sphere", plane->GetLocalPosition() + Vector3(150.f, 10.f, 0.f));
		sphere->mat->SetSpecularIntensity(1.f);
		sphere->SetScale(Vector3(10.f, 10.f, 10.f));
		body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);

		sphere = Scene::Instance()->addPhysicObject("sphere", plane->GetLocalPosition() + Vector3(200.f, 10.f, 0.f));
		sphere->mat->SetSpecularIntensity(1.f);
		sphere->SetScale(Vector3(10.f, 10.f, 10.f));
		body = sphere->GetComponent<RigidBody>();
		body->isKinematic = true;
		body->SetMass(FLT_MAX);
	}

	void 
	PickingApp::LoadScene2()
	{
		//A stack of boxes.
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		RigidBody* body = plane->GetComponent<RigidBody>();
		plane->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetMass(FLT_MAX);
		body->isKinematic = true;

		for (int i = 0; i < 10; i++)
		{
			Object* cube = Scene::Instance()->addPhysicObject("cube");
			cube->SetPosition(Vector3(0, i * 2 + plane->GetLocalPosition().y, 0));
		}
	}

	void 
	PickingApp::LoadScene3()
	{
		//Boxes sliding of a static plane oriented at an angle.
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -20.f, 0.f));
		RigidBody* body = plane->GetComponent<RigidBody>();
		plane->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetMass(FLT_MAX);
		body->isKinematic = true;
		plane->SetOrientation(Quaternion(0.7f, Vector3(0.f, 0.f, 1.f)));
		
		Scene::Instance()->addRandomlyPhysicObjects("cube", 50);
	}

	void 
	PickingApp::LoadScene4()
	{
		Clear();
		scene4loaded = true;
		Scene::Instance()->addRandomlyPhysicObjects("cube", 50);
	}

	void
	PickingApp::LoadScene5()
	{
		Clear();
		for (int i = 0; i < 700; i++)
		{
			Object* sphere = Scene::Instance()->addObject("sphere", Scene::Instance()->generateRandomIntervallVectorCubic(-20,20));
		}
	}

	void 
	PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
	}

	void
	PickingApp::Vortex()
	{
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			if (RigidBody* body = obj.second->GetComponent<RigidBody>())
			{
				Vector3 dir = obj.second->GetWorldPosition() - Vector3(0.f, 0.f, 0.f);
				body->ApplyImpulse(dir*-20.f, obj.second->GetWorldPosition());
			}
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
		currentCamera = new Camera(Vector3(0.f, -3.f, 26.f), windowWidth, windowHeight);
		currentCamera->Update((float)Time::timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, 0.1f, 200.0f);
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->getViewMatrix();
	}

	void
	PickingApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
		ShaderManager::Instance()->AddShader("picking", GraphicsManager::LoadShaders("Resources/Shaders/VSPicking.glsl", "Resources/Shaders/FSPicking.glsl"));
		ShaderManager::Instance()->AddShader("wireframe", GraphicsManager::LoadShaders("Resources/Shaders/VSBB.glsl", "Resources/Shaders/FSBB.glsl"));
		ShaderManager::Instance()->AddShader("dftext", GraphicsManager::LoadShaders("Resources/Shaders/VSDFText.glsl", "Resources/Shaders/FSDFText.glsl"));
		ShaderManager::Instance()->AddShader("depth", GraphicsManager::LoadShaders("Resources/Shaders/VSDepth.glsl", "Resources/Shaders/FSDepth.glsl"));
		ShaderManager::Instance()->AddShader("depthPanel", GraphicsManager::LoadShaders("Resources/Shaders/VSShadowMapPlane.glsl", "Resources/Shaders/FSShadowMapPlane.glsl"));
		ShaderManager::Instance()->AddShader("blur", GraphicsManager::LoadShaders("Resources/Shaders/VSBlur.glsl", "Resources/Shaders/FSBlur.glsl"));
	}

} // namespace Example