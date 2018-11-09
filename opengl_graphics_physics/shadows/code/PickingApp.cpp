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
		
		LoadScene6();

		Times::Instance()->currentTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowWidth/2.f, windowHeight/2.f+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera();

		Scene::Instance()->Update();

		ImGui_ImplGlfwGL3_Init(this->window->GetGLFWWindow(), false);

        while (running)
        {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->window->Update();
			ImGui_ImplGlfwGL3_NewFrame();

			Times::Instance()->Update(glfwGetTime());
			
			Monitor(this->window);

			//is cursor window locked
			CameraManager::Instance()->Update(Times::Instance()->deltaTime);
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);

			if (scene4loaded) Vortex();
			
			Scene::Instance()->Update();
			PhysicsManager::Instance()->Update(Times::Instance()->dtInv);

			Render::Instance()->UpdateEBOs();
			
			GenerateGUI(); // <-- (generate) to screen
			
			PassPickingTexture(); //picking
			if (altButtonToggle) PickingTest();

			DrawDepthPass();

			if(blurShadowMap) blurredShadowTexture = Render::Instance()->PingPongBlur(shadowTexture, shadowMapBlurLevel, blurShadowMapSize, GraphicsStorage::shaderIDs["fastBlurShadow"]);
			//render to screen
			//glViewport(0, 0, windowWidth, windowHeight);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); i clear screen above instead, fbo is cleared inside passes
			DrawColorDebugPass(); // color || debug
			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			DrawMaps(windowWidth, windowHeight);

			ImGui::Render(); // <-- (draw) to screen

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
					currentCamera->holdingForward = false;
					currentCamera->holdingBackward = false;
					currentCamera->holdingRight = false;
					currentCamera->holdingLeft = false;
					currentCamera->holdingUp = false;
					currentCamera->holdingDown = false;
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
			else if (key == GLFW_KEY_6) {
				scene4loaded = false;
				LoadScene6();
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
				cube->SetPosition(Vector3(0, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
			}
		}
    }

    void
    PickingApp::Monitor(Display::Window* window)
    {
		if (window->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) Times::Instance()->timeModifier += 0.0005;
		if (window->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) Times::Instance()->timeModifier -= 0.0005;
		if (window->GetKey(GLFW_KEY_UP) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, 0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_DOWN) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.f, -0.05f, 0.f));
		if (window->GetKey(GLFW_KEY_LEFT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(0.05f, 0.f, 0.f));
		if (window->GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) if (lastPickedObject) lastPickedObject->Translate(Vector3(-0.05f, 0.f, 0.f));
		
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
	PickingApp::DrawDepthPass()
	{
		//left right bottom top near far
		oleft = -orthoSize, oright = orthoSize, obottom = -orthoSize, otop = orthoSize, onear = -orthoSize, ofar = orthoSize;
		Matrix4 lightProjectionMatrix = Matrix4::orthographic(onear, ofar, oleft, oright, otop, obottom);

		Quaternion qRotX = Quaternion(xAngle, Vector3(1.f, 0.f, 0.f));
		Quaternion qRotY = Quaternion(yAngle, Vector3(0.f, 1.f, 0.f));
		
		if (directionalLightObject != nullptr)
		{
			directionalLightObject->SetOrientation(qRotY*qRotX); //object representing directional light and it's direction, we could just set all variables on object and then get later from it instead, both position and orientation
		}

		Matrix4 lightRotationMatrix = (qRotY*qRotX).ConvertToMatrix();

		Vector3 lightForward = lightRotationMatrix.getForward(); //because back is forward, stupid me, this is positive forward
		lightInvDir = -1.0 * lightForward.toFloat();

		Matrix4 lightViewMatrix = Matrix4::lookAt(Vector3(), lightForward, Vector3(0,1,0));

		FBOManager::Instance()->BindFrameBuffer(draw, Render::Instance()->dirShadowMapBuffer->handle);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		glViewport(0, 0, shadowTexture->width, shadowTexture->height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix4 model = Matrix4::translate(-1.0 * currentCamera->GetPosition2());

		Matrix4 ViewProjection = model * lightViewMatrix * lightProjectionMatrix;

		biasedDirLightViewProjection = (ViewProjection*Matrix4::biasMatrix()).toFloat();
	
		DrawDepth(ViewProjection);

		glViewport(0, 0, windowWidth, windowHeight);

		FBOManager::Instance()->UnbindFrameBuffer(draw);
	}

    void
	PickingApp::DrawColorDebugPass()
    {
		//we set shadowMap for color shader only once for all objects
		GLuint colorShader = GraphicsStorage::shaderIDs["color"];
		GLuint wireframeShader = GraphicsStorage::shaderIDs["wireframe"];
		ShaderManager::Instance()->SetCurrentShader(colorShader);

		GLuint LightDir = glGetUniformLocation(colorShader, "LightInvDirection_worldspace");
		glUniform3f(LightDir, lightInvDir.x, lightInvDir.y, lightInvDir.z);

		GLuint CameraPos = glGetUniformLocation(colorShader, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint shadowDistance = glGetUniformLocation(colorShader, "shadowDistance");
		glUniform1f(shadowDistance, orthoSize);

		GLuint shadowFadeSize = glGetUniformLocation(colorShader, "transitionDistance");
		glUniform1f(shadowFadeSize, shadowFadeRange);

		GLuint DepthBiasMatrixHandle = glGetUniformLocation(colorShader, "DepthBiasMVP");
		glUniformMatrix4fv(DepthBiasMatrixHandle, 1, GL_FALSE, &biasedDirLightViewProjection[0][0]);

		//depth texture
		GLuint ShadowMapHandle = glGetUniformLocation(colorShader, "shadowMapSampler");
		glActiveTexture(GL_TEXTURE1);
		if (blurShadowMap) glBindTexture(GL_TEXTURE_2D, blurredShadowTexture->handle);
		else glBindTexture(GL_TEXTURE_2D, shadowTexture->handle);
		glUniform1i(ShadowMapHandle, 1);

		if (debug)
		{
			ShaderManager::Instance()->SetCurrentShader(colorShader);
			Draw();
			ShaderManager::Instance()->SetCurrentShader(wireframeShader);
			DrawDebug();
		}
		else
		{
			ShaderManager::Instance()->SetCurrentShader(colorShader);
			Draw();
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    PickingApp::PassPickingTexture()
    {
		FBOManager::Instance()->BindFrameBuffer(draw, frameBuffer->handle);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(GraphicsStorage::shaderIDs["picking"]);
		DrawPicking();
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
			frameBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RED_INTEGER, GL_UNSIGNED_INT, &Pixel, pickingTexture->attachment);
			pickedID = Pixel;

			//std::cout << pickedID << std::endl;
			if (lastPickedObject != nullptr) //reset previously picked object color
			{
				lastPickedObject->mat->color = Vector3F(0.f, 0.f, 0.f);
			}
			if (Scene::Instance()->pickingList.find(pickedID) != Scene::Instance()->pickingList.end())
			{
				lastPickedObject = Scene::Instance()->pickingList[pickedID];
				lastPickedObject->mat->color = Vector3F(2.f, 1.f, 0.f);

				Vector3F world_position;
				frameBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RGB, GL_FLOAT, world_position.vect, worldPosTexture->attachment);
				Vector3 dWorldPos = Vector3(world_position.x, world_position.y, world_position.z);
				Vector3 impulse = (dWorldPos - currentCamera->GetPosition2()).vectNormalize();
				if (RigidBody* body = this->lastPickedObject->GetComponent<RigidBody>()) body->ApplyImpulse(impulse, 20.f, dWorldPos);
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

        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);

        this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
    }

    void
	PickingApp::Draw()
    {
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		objectsRendered = Render::Instance()->draw(Scene::Instance()->renderList, CameraManager::Instance()->ViewProjection, currentShaderID);
    }

	void
	PickingApp::DrawPicking()
	{
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		Render::Instance()->drawPicking(Scene::Instance()->pickingList, CameraManager::Instance()->ViewProjection, currentShaderID);
	}

	void
	PickingApp::DrawDepth(const Matrix4& ViewProjection)
	{
		GLuint currentShaderID = GraphicsStorage::shaderIDs["depth"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		Render::Instance()->drawDepth(Scene::Instance()->renderList, ViewProjection, currentShaderID);
	}

	void
	PickingApp::DrawDebug()
	{
		GLuint wireframeShader = GraphicsStorage::shaderIDs["wireframe"];

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
	}

	void 
	PickingApp::LoadScene1()
	{
		//A plank suspended on a static box.	
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube");
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
		//plane->SetOrientation(Quaternion(0.7f, Vector3(0.f, 0.f, 1.f)));
		
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
	PickingApp::LoadScene6()
	{
		Clear();

		//Object* cube1 = Scene::Instance()->addObject("cube");
		directionalLightObject = Scene::Instance()->addDirectionalLight();
		

		Object* plane = Scene::Instance()->addObject("fatplane", Vector3(0.f, -10.f, 0.f));
		//Object* sphere1 = Scene::Instance()->addObject("sphere");
		//sphere1->mat->SetShininess(20.f);
		//sphere1->mat->SetSpecularIntensity(3.f);
		//sphere1->SetScale(Vector3(30.0, 30.0, 30.0));

		Material* newMaterial = new Material();
		newMaterial->AssignTexture(GraphicsStorage::textures.at(5));
		newMaterial->tileX = 50;
		newMaterial->tileY = 50;
		GraphicsStorage::materials.push_back(newMaterial);
		plane->AssignMaterial(newMaterial);
		plane->SetScale(Vector3(100.0, 1.0, 100.0));

		Object* cone = Scene::Instance()->addObjectTo(directionalLightObject, "cone");
		//cone->mat->SetColor(0.f, 100.f, 0.f);
		//cone->mat->SetDiffuseIntensity(1.f);
		cone->SetScale(Vector3(3, 3, 10));

		float rS = 1.f;


		for (int i = 0; i < 70; i++)
		{

			Vector3 pos = Scene::Instance()->generateRandomIntervallVectorSpherical(10, (100 + 0 + 22) * 100);


			double len = pos.vectLengt();
			Object* sphere = Scene::Instance()->addObject("icosphere", pos);
			sphere->mat->SetShininess(20.f);
			sphere->mat->SetSpecularIntensity(3.f);

			for (int j = 0; j < 3; j++)
			{
				Vector3 childPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-len, (int)len) / 4.f;
				double childLen = childPos.vectLengt();
				Object* child = Scene::Instance()->addObjectTo(sphere, "icosphere", childPos);
				child->mat->SetShininess(20.f);
				child->mat->SetSpecularIntensity(3.f);

				for (int k = 0; k < 5; k++)
				{
					Vector3 childOfChildPos = Scene::Instance()->generateRandomIntervallVectorCubic((int)-childLen, (int)childLen) / 2.f;
					Object* childOfChild = Scene::Instance()->addObjectTo(child, "sphere", childOfChildPos);
					childOfChild->mat->SetShininess(20.f);
					childOfChild->mat->SetSpecularIntensity(3.f);
				}
			}
		}
	}

	void 
	PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
		directionalLightObject = nullptr;
	}

	void
	PickingApp::Vortex()
	{
		for (auto& obj : Scene::Instance()->renderList)
		{
			if (RigidBody* body = obj->GetComponent<RigidBody>())
			{
				Vector3 dir = obj->GetWorldPosition() - Vector3(0.f, -10.f, 0.f);
				body->ApplyImpulse(dir*-1.f, obj->GetWorldPosition());
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
		currentCamera->Update(Times::Instance()->timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void
	PickingApp::GenerateGUI()
	{
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		float start = 0;
		float stop = 360;


		ImGui::SliderFloat("xAngle", &xAngle, start, stop);
		ImGui::SliderFloat("yAngle", &yAngle, start, stop);

		ImGui::SliderFloat("Fov", &fov, 0.0f, 180.f);
		ImGui::SliderFloat("Near plane", &near, 0.0f, 5.f);
		ImGui::SliderFloat("Far plane", &far, 0.0f, 2000.f);
		ImGui::SliderFloat("Ortho size", &orthoSize, 0.0f, 2000.f);
		ImGui::Checkbox("Blur ShadowMap", &blurShadowMap);
		ImGui::SliderFloat("Blur Shadow Size", &blurShadowMapSize, 0.0f, 5.f);
		ImGui::SliderInt("Bloom Level", &shadowMapBlurLevel, 0, 3);
		ImGui::SliderFloat("Shadow Fade Range", &shadowFadeRange, 0.0f, 50.f);

		ImGui::End();
	}

	void
	PickingApp::SetUpBuffers(int windowWidth, int windowHeight)
	{
		frameBuffer = FBOManager::Instance()->GenerateFBO();
		pickingTexture = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_R32UI, windowWidth, windowHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL, GL_COLOR_ATTACHMENT0)); //picking
		pickingTexture->AddDefaultTextureParameters();
		worldPosTexture = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1)); //position
		worldPosTexture->AddDefaultTextureParameters();
		Texture* depthTexture = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT)); //depth
		depthTexture->AddDefaultTextureParameters();
		frameBuffer->GenerateAndAddTextures();
		frameBuffer->CheckAndCleanup();

		FrameBuffer* shadowBuffer = Render::Instance()->AddDirectionalShadowMapBuffer(4096, 4096);
		Render::Instance()->AddMultiBlurBuffer(this->windowWidth, this->windowHeight);
		Render::Instance()->AddPingPongBuffer(4096, 4096);
		shadowTexture = shadowBuffer->textures[0];

		Render::Instance()->GenerateEBOs();
	}

	void
	PickingApp::DrawMaps(int width, int height)
	{
		ShaderManager::Instance()->SetCurrentShader(GraphicsStorage::shaderIDs["depthPanel"]);

		float fHeight = (float)height;
		float fWidth = (float)width;
		int y = (int)(fHeight*0.1f);
		int glWidth = (int)(fWidth *0.1f);
		int glHeight = (int)(fHeight*0.1f);

		if (blurShadowMap) DebugDraw::Instance()->DrawMap(0, 0, glWidth, glHeight, blurredShadowTexture->handle, width, height);
		else DebugDraw::Instance()->DrawMap(0, 0, glWidth, glHeight, shadowTexture->handle, width, height);
	}
} // namespace Example