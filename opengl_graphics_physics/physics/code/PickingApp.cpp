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
            float aspect = (float)this->windowWidth / (float)this->windowHeight;

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
		
		LoadScene1();

        // For speed computation (FPS)
		Times::Instance()->currentTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowMidX, windowMidY+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);
		SetUpCamera();

		Scene::Instance()->Update();

		ImGui_ImplGlfwGL3_Init(this->window->GetGLFWWindow(), false);
		
		//glfwSwapInterval(0); //unlock fps

        while (running)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->window->Update();

			ImGui_ImplGlfwGL3_NewFrame();

            // Measure FPS
			Times::Instance()->Update(glfwGetTime());
			
			Monitor(this->window);

			CameraManager::Instance()->Update(Times::Instance()->deltaTime);
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);
			
			PassPickingTexture();
			if (altButtonToggle) PickingTest();

			Scene::Instance()->Update();

			PhysicsManager::Instance()->Update(Times::Instance()->dtInv);

			if (scene4loaded) Vortex();

			Render::Instance()->UpdateEBOs();

			GenerateGUI();

			DrawPass2(); // color || debug
			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			
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
				Object* cube = Scene::Instance()->addPhysicObject("sphere", Vector3(0.f, 8.f, 0.f));
				cube->SetPosition(Vector3(0.f, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
			}
		}
        
    }

    void
    PickingApp::Monitor(Display::Window* window)
    {
		if (window->GetKey(GLFW_KEY_KP_ADD) == GLFW_PRESS) Times::Instance()->timeModifier += 0.005;
		if (window->GetKey(GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) Times::Instance()->timeModifier -= 0.005;
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
    PickingApp::DrawPass2()
    {
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
    }

    void
    PickingApp::PassPickingTexture()
    {
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		FBOManager::Instance()->BindFrameBuffer(draw, pickingBuffer->handle);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //do we need to clear it?
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
			pickingBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RED_INTEGER, GL_UNSIGNED_INT, &Pixel, pickingTexture->attachment);
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
				pickingBuffer->ReadPixelData((unsigned int)leftMouseX, this->windowHeight - (unsigned int)leftMouseY, GL_RGB, GL_FLOAT, world_position.vect, worldPosTexture->attachment);
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);

		LoadShaders();

        this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;
    }

    void
    PickingApp::Draw()
    {
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint DirLightInvDirection = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(DirLightInvDirection, 1, &lightInvDir.x);

		GLuint PointLightPos = glGetUniformLocation(currentShaderID, "PointLightPosition_worldspace");
		glUniform3fv(PointLightPos, 1, &pointLightPos.x);

		GLuint PointLightRadius = glGetUniformLocation(currentShaderID, "PointLightRadius");
		glUniform1f(PointLightRadius, pointLightRadius);

		GLuint attenauationConstantp = glGetUniformLocation(currentShaderID, "pointAttenuation.constant");
		glUniform1f(attenauationConstantp, pointLightAttenuation.Constant);

		GLuint attenauationLinearp = glGetUniformLocation(currentShaderID, "pointAttenuation.linear");
		glUniform1f(attenauationLinearp, pointLightAttenuation.Linear);

		GLuint attenauationExponentialp = glGetUniformLocation(currentShaderID, "pointAttenuation.exponential");
		glUniform1f(attenauationExponentialp, pointLightAttenuation.Exponential);

		GLuint SpotLightDirection_worldspace = glGetUniformLocation(currentShaderID, "SpotLightDirection_worldspace");
		//glUniform3fv(SpotLightDirection_worldspace, 1, &spotLightDir.x);
		//Vector3F cameraDir = -1.f * currentCamera->getDirection().toFloat(); //here is the forward direction before being transformed into view matrix
		//Vector3F cameraDir = currentCamera->ViewMatrix.getInvBackZ().toFloat(); //because it's camera view matrix we can't just get the megative forward direction, we have to use the inverse
		glUniform3fv(SpotLightDirection_worldspace, 1, &spotLightInvDir.x);

		GLuint SpotLightPosition_worldspace = glGetUniformLocation(currentShaderID, "SpotLightPosition_worldspace");
		glUniform3fv(SpotLightPosition_worldspace, 1, &spotLightPos.x);
		//glUniform3fv(SpotLightPosition_worldspace, 1, &camPos.x);
		
		GLuint SpotLightCutOff = glGetUniformLocation(currentShaderID, "SpotLightCutOff");
		glUniform1f(SpotLightCutOff, std::cos(MathUtils::ToRadians(spotLightCutOff)));

		GLuint SpotLightOuterCutOff = glGetUniformLocation(currentShaderID, "SpotLightOuterCutOff");
		glUniform1f(SpotLightOuterCutOff, std::cos(MathUtils::ToRadians(spotLightOuterCutOff)));

		GLuint SpotLightRadius = glGetUniformLocation(currentShaderID, "SpotLightRadius");
		glUniform1f(SpotLightRadius, spotLightRadius);

		GLuint attenauationConstant = glGetUniformLocation(currentShaderID, "spotAttenuation.constant");
		glUniform1f(attenauationConstant, spotLightAttenuation.Constant);

		GLuint attenauationLinear = glGetUniformLocation(currentShaderID, "spotAttenuation.linear");
		glUniform1f(attenauationLinear, spotLightAttenuation.Linear);

		GLuint attenauationExponential = glGetUniformLocation(currentShaderID, "spotAttenuation.exponential");
		glUniform1f(attenauationExponential, spotLightAttenuation.Exponential);

		objectsRendered = Render::Instance()->draw(Scene::Instance()->renderList, CameraManager::Instance()->ViewProjection, currentShaderID);
    }

	void
	PickingApp::DrawPicking()
	{
		GLuint currentShaderID = ShaderManager::Instance()->GetCurrentShaderID();
		Render::Instance()->drawPicking(Scene::Instance()->pickingList, CameraManager::Instance()->ViewProjection, currentShaderID);
	}

	void
	PickingApp::DrawDebug()
	{
		GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];

		for (auto& obj : Scene::Instance()->renderList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj->node.centeredPosition, obj->radius)) {
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

	void PickingApp::LoadScene1()
	{
		//A plank suspended on a static box.	
		Clear();

		Object* plane = Scene::Instance()->addObject("fatplane", Vector3(0.f, -1.5f, 0.f));
		plane->mat->SetShininess(30.f);
		plane->mat->SetSpecularIntensity(2.f);
		RigidBody* body = new RigidBody(plane);
		plane->AddComponent(body);
		//plane->SetScale(Vector3(25.f, 2.f, 25.f));
		body->SetMass(FLT_MAX);
		body->isKinematic = true;
		
		
		Object* plank = Scene::Instance()->addPhysicObject("cube", plane->GetLocalPosition() + Vector3(0.f, 5.f, 0.f));
		plank->mat->SetShininess(30.f);
		plank->mat->SetSpecularIntensity(20.f);
		plank->SetScale(Vector3(3.f, 0.5f, 3.f));
		Object* cube = Scene::Instance()->addPhysicObject("cube", plane->GetLocalPosition() + Vector3(0.f, 2.f, 0.f));
		body = cube->GetComponent<RigidBody>();
		body->SetMass(1);
		body->isKinematic = true;
		body->massInverse = 0;
		
		Object * sphere = Scene::Instance()->addObject("sphere", plane->GetLocalPosition() + Vector3(5.f, 2.f, 5.f));
		sphere->mat->SetShininess(30.f);
		sphere->mat->SetSpecularIntensity(2.f);

		spotLightObject = Scene::Instance()->addObject("cube", plane->GetLocalPosition() + Vector3(0.f, 2.f, 0.f));
		directionalLightObject = Scene::Instance()->addObject("cube", plane->GetLocalPosition() + Vector3(0.f, 5.f, 0.f));
	}

	void PickingApp::LoadScene2()
	{
		//A stack of boxes.
		Clear();
		Object* plane = Scene::Instance()->addPhysicObject("fatplane", Vector3(0.f, -10.f, 0.f));
		RigidBody* body = plane->GetComponent<RigidBody>();
		body->SetMass(FLT_MAX);
		body->isKinematic = true;

		for (int i = 0; i < 10; i++)
		{
			Object* cube = Scene::Instance()->addPhysicObject("cube");
			cube->SetPosition(Vector3(0, i * 2 - 10 + 0.001f, 0));
		}
	}

	void PickingApp::LoadScene3()
	{
		//Boxes sliding of a static plane oriented at an angle.
		Clear();
		Object* plane = Scene::Instance()->addPhysicObject("fatplane", Vector3(0.f, -20.f, 0.f));
		RigidBody* body = plane->GetComponent<RigidBody>();
		body->SetMass(FLT_MAX);
		body->isKinematic = true;
		plane->SetOrientation(Quaternion(0.7f, Vector3(0.f, 0.f, 1.f)));

		Scene::Instance()->addRandomlyPhysicObjects("cube", 50);
	}

	void PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		lastPickedObject = nullptr;
		spotLightObject = nullptr;
		directionalLightObject = nullptr;
	}

	void PickingApp::LoadScene4()
	{
		Clear();
		PhysicsManager::Instance()->gravity = Vector3(0.f, -9.f, 0.f);
		scene4loaded = true;
		Scene::Instance()->addRandomlyPhysicObjects("cube", 200);
	}


	void PickingApp::LoadScene5()
	{
		Clear();
		PhysicsManager::Instance()->gravity = Vector3();

		Scene::Instance()->addRandomlyPhysicObjects("icosphere", 600);
	}


	void PickingApp::Vortex()
	{
		for (auto& obj : Scene::Instance()->renderList)
		{
			if (RigidBody* body = obj->GetComponent<RigidBody>())
			{
				Vector3 dir = obj->GetWorldPosition() - Vector3(0.f, -10.f, 0.f);
				body->ApplyImpulse(dir*-1.0, Vector3(0.f, -10.f, 0.f));
			}
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
		currentCamera = new Camera(Vector3(0.f, 10.f, 26.f), windowWidth, windowHeight);
		currentCamera->Update(Times::Instance()->timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void PickingApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl"));
		ShaderManager::Instance()->AddShader("picking", GraphicsManager::LoadShaders("Resources/Shaders/VSPicking.glsl", "Resources/Shaders/FSPicking.glsl"));
		ShaderManager::Instance()->AddShader("wireframe", GraphicsManager::LoadShaders("Resources/Shaders/VSBB.glsl", "Resources/Shaders/FSBB.glsl"));
		ShaderManager::Instance()->AddShader("dftext", GraphicsManager::LoadShaders("Resources/Shaders/VSDFText.glsl", "Resources/Shaders/FSDFText.glsl"));
	}

	void PickingApp::SetUpBuffers(int windowWidth, int windowHeight)
	{
		pickingBuffer = FBOManager::Instance()->GenerateFBO();
		pickingTexture = pickingBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_R32UI, windowWidth, windowHeight, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL, GL_COLOR_ATTACHMENT0)); //picking
		worldPosTexture = pickingBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1)); //position
		pickingBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT)); //depth
		pickingBuffer->AddDefaultTextureParameters();
		pickingBuffer->GenerateAndAddTextures();
		pickingBuffer->CheckAndCleanup();

		Render::Instance()->GenerateEBOs();
	}

	void PickingApp::GenerateGUI()
	{
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Objects rendered %d", objectsRendered);
		ImGui::Text("Picked ID %d", pickedID);

		float start = 0;
		float stop = 360;
		ImGui::SliderFloat("Spot X Angle", &xAngles, start, stop);
		ImGui::SliderFloat("Spot Y Angle", &yAngles, start, stop);
		ImGui::SliderFloat("Spot PosX", &posX, -10.f, 10.f);
		ImGui::SliderFloat("Spot PosY", &posY, -10.f, 10.f);
		ImGui::SliderFloat("Spot PosZ", &posZ, -10.f, 10.f);
		ImGui::SliderFloat("Dir X Angle", &xAngled, start, stop);
		ImGui::SliderFloat("Dir Y Angle", &yAngled, start, stop);
		ImGui::End();

		spotLightPos = Vector3F(posX, posY, posZ);

		Quaternion qXangles = Quaternion(xAngles, Vector3(1.0, 0.0, 0.0));
		Quaternion qYangles = Quaternion(yAngles, Vector3(0.0, 1.0, 0.0));
		Quaternion spotTotalRotation = (qYangles*qXangles);
		Matrix4 spotRotationMatrix = spotTotalRotation.ConvertToMatrix();
		Vector3 spotForwardDirection = spotRotationMatrix.getForward();
		spotLightInvDir = -1.0f * spotForwardDirection.toFloat();

		Quaternion qXangled = Quaternion(xAngled, Vector3(1.0, 0.0, 0.0));
		Quaternion qYangled = Quaternion(yAngled, Vector3(0.0, 1.0, 0.0));
		Quaternion dirTotalRotation = (qYangled*qXangled);
		Matrix4 dirRotationMatrix = dirTotalRotation.ConvertToMatrix();
		Vector3 dirForwardDirection = dirRotationMatrix.getForward();
		lightInvDir = -1.0f * dirForwardDirection.toFloat();

		if (spotLightObject != nullptr) {
			spotLightObject->SetOrientation(spotTotalRotation);
			spotLightObject->SetPosition(Vector3(spotLightPos.x, spotLightPos.y, spotLightPos.z));
		}
		if (directionalLightObject != nullptr) directionalLightObject->SetOrientation(dirTotalRotation);
	}
} // namespace Example