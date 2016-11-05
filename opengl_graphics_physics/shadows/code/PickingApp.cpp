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

        Object* Scene = Scene::Instance()->build();

		GraphicsManager::LoadAllAssets();

		DebugDraw::Instance()->LoadPrimitives();

		boundingBox = &DebugDraw::Instance()->boundingBox;
		
		LoadScene1();

		float timeStep = 0.016f + timeModifier;
		float dtInv = 1.f / timeStep;
        // For speed computation (FPS)
		double lastTime = glfwGetTime();

		//camera rotates based on mouse movement, setting initial mouse pos will always focus camera at the beginning in specific position
		window->SetCursorPos(windowWidth/2.f, windowHeight/2.f+100); 
		window->SetCursorMode(GLFW_CURSOR_DISABLED);

		SetUpCamera(timeStep);

		double fps_timer = 0;
		Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());

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
			
			timeStep = 0.016f + timeModifier;
			dtInv = 1.f / timeStep;
			if (paused) timeStep = 0.f, dtInv = 0;

			if (scene4loaded)
			{
				if (currentTime - fps_timer >= 0.2f){
					Vortex();
				}
			}
			
			PhysicsManager::Instance()->SortAndSweep();
			PhysicsManager::Instance()->NarrowTestSAT(dtInv);
			
			IntegrateAndUpdateBoxes(timeStep);
			Scene::Instance()->SceneObject->node.UpdateNodeMatrix(Matrix4::identityMatrix());
			
			PassPickingTexture(ProjectionMatrix, ViewMatrix); //picking
			PickingTest();

			DrawDepthPass(); 
			BlurShadowMap();
			//render to screen
			//glViewport(0, 0, windowWidth, windowHeight);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); i clear screen above instead, fbo is cleared inside passes
			DrawColorDebugPass(ProjectionMatrix, ViewMatrix); // color || debug
			DebugDraw::Instance()->DrawCrossHair(windowWidth, windowHeight);
			FBOManager::Instance()->DrawShadowMap(windowWidth, windowHeight);
			if (currentTime - fps_timer >= 0.2f){ 
				this->window->SetTitle("Objects rendered: " + std::to_string(objectsRendered) + " FPS: " + std::to_string(1.f / deltaTime) + " TimeStep: " + std::to_string(timeStep) + " PickedID: " + std::to_string(pickedID) + (paused ? " PAUSED" : ""));
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
	PickingApp::BlurShadowMap()
	{
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["blur"]);
		GLuint scaleUniform = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "scaleUniform");
		GLuint ShadowMapSamplerHandle = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "shadowMapSampler");

		for (int i = 0; i < 2; i++){

			FBOManager::Instance()->BindBlurFrameBuffer(draw);
			GLenum DrawBlurBuffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBlurBuffers);
			//glViewport(0, 0, 2048, 2048);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 1.0f / (float)windowWidth, 0.0f); //horizontally

			//Bind shadow map to be blurred
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapHandle);
			glUniform1i(ShadowMapSamplerHandle, 0);

			DebugDraw::Instance()->DrawQuad(); 
			FBOManager::Instance()->UnbindFrameBuffer(draw);
			


			FBOManager::Instance()->BindFrameBuffer(draw);
			GLenum DrawShadowBuffers[] = { GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(1, DrawShadowBuffers);
			//glViewport(0, 0, 2048, 2048);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 0.0f, 1.0f / (float)windowHeight); //vertically

			//Bind shadow map to be blurredblurred
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapBlurdHandle);
			glUniform1i(ShadowMapSamplerHandle, 0);

			DebugDraw::Instance()->DrawQuad();
			FBOManager::Instance()->UnbindFrameBuffer(draw);

		}
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
				cube->SetPosition(Vector3(0, (float)Scene::Instance()->idCounter * 2.f - 10.f + 0.001f, 0.f));
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
	PickingApp::DrawDepthPass()
	{

		// Compute the MVP matrix from the light's point of view

		//this projection and view work as directional light

		//left right bottom top near far
		float left = -40, right = 40, bottom = -40, top = 40, near = -30, far = 40;
		Matrix4 depthProjectionMatrix = Matrix4::orthographic(near, far, left, right, top, bottom);

		//eye target up
		Matrix4 depthViewMatrix = Matrix4::lookAt(lightInvDir, Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f));


		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(1, DrawBuffers);
		//glViewport(0, 0, 2048, 2048);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["depth"]);
		DrawDepth(depthProjectionMatrix, depthViewMatrix);
		FBOManager::Instance()->UnbindFrameBuffer(draw);
		
		//DebugDraw::Instance()->DrawNormal(lightInvDir.vectNormalize(),Vector3(0,0,0));
	}

    void
	PickingApp::DrawColorDebugPass(const Matrix4& Projection, const Matrix4& View)
    {
		//we set shadowMap for color shader only once for all objects
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
		GLuint ShadowMapHandle = glGetUniformLocation(ShaderManager::Instance()->GetCurrentShaderID(), "shadowMapSampler");
		//depth texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, FBOManager::Instance()->shadowMapHandle);
		glUniform1i(ShadowMapHandle, 1);

		if (debug)
		{
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
			Draw(Projection, View);
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["wireframe"]);
			DrawDebug(Projection, View);
		}
		else
		{
			ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["color"]);
			Draw(Projection, View);
		}
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
    }

    void
    PickingApp::PassPickingTexture(const Matrix4& Projection, const Matrix4& View)
    {
		
		FBOManager::Instance()->BindFrameBuffer(draw);
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, DrawBuffers);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["picking"]);
		Draw(Projection, View);
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
		Vector4 mouse_p0s(x, y, 0.0, 0.0);
		mouse_p0s[0] = ((float)x / (float)windowWidth)*2.f - 1.f;
		mouse_p0s[1] = (((float)windowHeight - (float)y) / windowHeight)*2.f - 1.f;
		mouse_p0s[2] = -1.f;
		mouse_p0s[3] = 1.f;

		Vector4 my_mouse_in_world_space = this->ProjectionMatrix.inverse() * mouse_p0s;
		my_mouse_in_world_space = this->ViewMatrix.inverse() * my_mouse_in_world_space;
		my_mouse_in_world_space = my_mouse_in_world_space / my_mouse_in_world_space[3];

		Vector3 my_mouse_in_world_space_vec3(my_mouse_in_world_space[0], my_mouse_in_world_space[1], my_mouse_in_world_space[2]);
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

	void
	PickingApp::DrawDepth(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			obj.second->drawDepth(ProjectionMatrix, ViewMatrix);
		}
	}

	void
	PickingApp::DrawDebug(const Matrix4& ProjectionMatrix, const Matrix4& ViewMatrix)
	{
		GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
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
		//A plank suspended on a static box.	
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		plane->SetScale(25, 2, 25);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;

		Object* plank = Scene::Instance()->addPhysicObject("cube", plane->GetPosition() + Vector3(0.f, 10.f, 0.f));
		plank->SetScale(3, 0.5, 1);
		Object* cube = Scene::Instance()->addPhysicObject("cube", plane->GetPosition() + Vector3(0.f, 3.f, 0.f));
		cube->isKinematic = true;
		cube->SetMass(FLT_MAX);	
	}

	void 
	PickingApp::LoadScene2()
	{
		//A stack of boxes.
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -10.f, 0.f));
		plane->SetScale(25.f, 2.f, 25.f);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;

		for (int i = 0; i < 10; i++)
		{
			Object* cube = Scene::Instance()->addPhysicObject("cube");
			cube->SetPosition(Vector3(0, i * 2 + plane->GetPosition().y, 0));
		}
	}

	void 
	PickingApp::LoadScene3()
	{
		//Boxes sliding of a static plane oriented at an angle.
		Clear();

		Object* plane = Scene::Instance()->addPhysicObject("cube", Vector3(0.f, -20.f, 0.f));
		plane->SetScale(25, 2, 25);
		plane->SetMass(FLT_MAX);
		plane->radius = 50.f;
		plane->isKinematic = true;
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
			Object* sphere = Scene::Instance()->addRandomlyObject("sphere");
			sphere->SetMass(FLT_MAX);
			sphere->isKinematic = true;
		}
	}

	void 
	PickingApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		lastPickedObject = nullptr;
	}

	void
	PickingApp::Vortex()
	{
		for (auto& obj : Scene::Instance()->objectsToRender)
		{
			Vector3 dir = obj.second->GetPosition() - Vector3(0.f,0.f,0.f);
			obj.second->ApplyImpulse(dir*-20.f, obj.second->GetPosition());
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
	PickingApp::SetUpCamera(float timeStep)
	{
		currentCamera = new Camera(Vector3(0.f, -3.f, 26.f), windowWidth, windowHeight);
		currentCamera->Update(timeStep);
		window->SetCursorPos(windowMidX, windowMidY);
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