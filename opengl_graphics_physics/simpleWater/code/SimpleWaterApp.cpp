#include "SimpleWaterApp.h"
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
#include "Camera.h"
#include "Frustum.h"
#include "Render.h"
#include "CameraManager.h"
#include <chrono>

using namespace mwm;
using namespace Display;

namespace SimpleWater
{

	//------------------------------------------------------------------------------
	/**
	*/
	SimpleWaterApp::SimpleWaterApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	SimpleWaterApp::~SimpleWaterApp()
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	bool
	SimpleWaterApp::Open()
	{
		App::Open();
		this->window = new Display::Window;

		window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mode)
		{
			KeyCallback(key, scancode, action, mode);
		}); 
		
		window->SetCloseFunction([this]()
		{
			this->running = false;
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
			currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.f, aspect, this->near, this->far);
			UpdateTextureBuffers(this->windowWidth, this->windowHeight);
			currentCamera->UpdateSize(width, height);
			CameraManager::Instance()->Update();
		});

		this->window->SetWindowIconifyFunction([this](int iconified){
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
	SimpleWaterApp::Run()
	{

		InitGL();
		SetUpFrameBuffer(this->windowWidth, this->windowHeight);
		SetUpPostBuffer(this->windowWidth, this->windowHeight);
		SetUpBlurrBuffer(this->windowWidth, this->windowHeight);

		GraphicsManager::LoadAllAssets();
		LoadScene1();
		
		double lastTime = glfwGetTime();
		window->SetCursorMode(GLFW_CURSOR_DISABLED);
		SetUpCamera();

		double fps_timer = 0;
		Node initNode = Node();
		Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);
		glfwSwapInterval(0); //unlock fps
		ImGui_ImplGlfwGL3_Init(this->window->GetGLFWWindow(), false);
		window->SetTitle("Simple Water");
		while (running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window->Update();
			if (minimized) continue;
			ImGui_ImplGlfwGL3_NewFrame();

			Time::currentTime = glfwGetTime();
			Time::deltaTime = Time::currentTime - lastTime;

			Monitor(this->window);

			//is cursor window locked
			if (windowLocked) CameraManager::Instance()->Update();
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);
			
			Scene::Instance()->SceneObject->node.UpdateNodeTransform(initNode);
			//for HDR //draw current "to screen" to another texture draw water there too, then draw quad to screen from this texture
			//for Bloom //draw current "to screen" to yet another texture in same shaders
			DrawGUI(); // <-- (generate) to screen
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferHandle);
			DrawReflection(); // <-- to fb texture
			DrawRefraction(); // <-- to fb texture
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFrameBufferHandle);
			DrawSkybox(); // <-- to pf texture
			Draw(); // <-- to pf texture
			DrawWater(); // <-- to pf textures
			if (post)
			{
				BlurLight(); //blur bright color
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				DrawHDR(); // <-- to screen from hdr and bloom textures
			}
			else
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["quadToScreen"]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
				DebugDraw::Instance()->DrawQuad();
			}
			
			
			DrawTextures(windowWidth, windowHeight);
			ImGui::Render(); // <-- (draw) to screen
			this->window->SwapBuffers();

			lastTime = Time::currentTime;			
		}
		this->ClearBuffers();
		GraphicsStorage::ClearMaterials();
		GraphicsStorage::ClearOBJs();
		ImGui_ImplGlfwGL3_Shutdown();
		this->window->Close();
	}

	void
	SimpleWaterApp::ClearBuffers()
	{
		GraphicsStorage::ClearMeshes();
		GraphicsStorage::ClearTextures();
		GraphicsStorage::ClearCubeMaps();
		ShaderManager::Instance()->DeleteShaders();
	}

	void
	SimpleWaterApp::KeyCallback(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_ESCAPE) {
				running = false;
			}
			//this is just used to show hide cursor, mouse steal on/off
			else if (key == GLFW_KEY_LEFT_ALT) {
				if (windowLocked) {
					windowLocked = false;
					window->SetCursorMode(GLFW_CURSOR_NORMAL);
				}
				else {
					windowLocked = true;
					window->SetCursorPos(windowWidth / 2.f, windowHeight / 2.f);
					window->SetCursorMode(GLFW_CURSOR_DISABLED);
				}
			}
			else if (key == GLFW_KEY_1) {
				LoadScene1();
			}
		}
	}

	void
	SimpleWaterApp::Monitor(Display::Window* window)
	{
		currentCamera->holdingForward = (window->GetKey(GLFW_KEY_W) == GLFW_PRESS);
		currentCamera->holdingBackward = (window->GetKey(GLFW_KEY_S) == GLFW_PRESS);
		currentCamera->holdingRight = (window->GetKey(GLFW_KEY_D) == GLFW_PRESS);
		currentCamera->holdingLeft = (window->GetKey(GLFW_KEY_A) == GLFW_PRESS);
		currentCamera->holdingUp = (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS);
		currentCamera->holdingDown = (window->GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);

		if (windowLocked)
		{
			double mouseX, mouseY;
			window->GetCursorPos(&mouseX, &mouseY);
			currentCamera->UpdateOrientation(mouseX, mouseY);
			window->SetCursorPos(windowMidX, windowMidY);
		}
}

	void
	SimpleWaterApp::InitGL()
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

		this->window->GetWindowSize(&this->windowWidth, &this->windowHeight);
		windowMidX = windowWidth / 2.0f;
		windowMidY = windowHeight / 2.0f;

	}

	void
	SimpleWaterApp::Clear()
	{
		Scene::Instance()->Clear();
		PhysicsManager::Instance()->Clear();
		GraphicsStorage::ClearMaterials();
		for (auto& obj : dynamicObjects)
		{
			delete obj;
		}
		dynamicObjects.clear();
		for (auto& mesh : dynamicMeshes)
		{
			delete mesh;
		}
		dynamicMeshes.clear();
		water = nullptr;
		selectedObject = nullptr;
		skybox = nullptr;
	}

	void
	SimpleWaterApp::LoadScene1()
	{
		Clear();

		//water object
		water = new Object();
		dynamicObjects.push_back(water);
		water->SetPosition(Vector3(0.f, 0.f, 0.f));

		Material* newMaterial = new Material();
		water->AssignMaterial(newMaterial);
		Mesh* waterMesh = GenerateWaterMesh(waterSize, waterSize);
		dynamicMeshes.push_back(waterMesh);

		water->AssignMesh(waterMesh);
		water->SetScale(Vector3(100.f, 0.f, 100.f));
		water->SetPosition(Vector3(50.f, 0.f, -50.f));
		water->SetOrientation(Quaternion(1.57f, Vector3(0.f, 1.f, 0.f)));

		water->node.UpdateNodeTransform(Scene::Instance()->SceneObject->node);

		newMaterial->SetShininess(10.f);
		newMaterial->SetSpecularIntensity(0.55f);
		newMaterial->tileX = 6.f;
		newMaterial->tileY = 6.f;
		GraphicsStorage::materials.push_back(newMaterial);
		
		//skybox object
		skybox = new Object();
		dynamicObjects.push_back(skybox);
		skybox->SetPosition(Vector3(0.f, 0.f, 0.f));

		Material* newMaterial2 = new Material();
		newMaterial2->AssignTexture(GraphicsStorage::cubemaps[0]);

		skybox->AssignMaterial(newMaterial2);
		skybox->AssignMesh(GraphicsStorage::meshes["skybox"]);
		newMaterial2->SetShininess(40.f);
		newMaterial2->SetSpecularIntensity(0.5f);
		GraphicsStorage::materials.push_back(newMaterial2);

		Object* sphere2 = Scene::Instance()->addObjectToScene("sphere", Vector3(0.f, -5.f, 0.f));
		sphere2->mat->SetShininess(20.f);
		sphere2->mat->SetSpecularIntensity(3.f);

		sphere2 = Scene::Instance()->addObjectToScene("sphere", Vector3(0.f, 5.f, 0.f));
		sphere2->SetScale(Vector3(4.f, 4.f, 4.f));
		sphere2->mat->SetShininess(20.f);
		sphere2->mat->SetSpecularIntensity(3.f);

		for (int i = 0; i < 20; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("icosphere", Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::axis::y, -5));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f,2.f,2.f));
		}

		for (int i = 0; i < 20; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("sphere", Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::axis::y, 5));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f, 2.f, 2.f));
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("cube", Vector3(30.f, i * 2.f, 30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f, 2.f, 2.f));
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("cube", Vector3(-30.f, i * 2.f, 30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("cube", Vector3(-30.f, i * 2.f, -30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("cube", Vector3(30.f, i * 2.f, -30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 12; i++)
		{
			Object* sphere = Scene::Instance()->addObjectToScene("cube", Vector3(0.f, -2.f + i * 2.f, 0.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		Object* plane = Scene::Instance()->addObjectToScene("pond", Vector3(0.f, -3.f, 0.f));
		plane->mat->AssignTexture(GraphicsStorage::textures[3]);
		plane->mat->tileX = 20;
		plane->mat->tileY = 20;
	}

	void
	SimpleWaterApp::SetUpCamera()
	{
		currentCamera = new Camera(Vector3(0.f, 25.f, 66.f), windowWidth, windowHeight);
		currentCamera->Update((float)Time::timeStep);
		window->SetCursorPos(windowMidX, windowMidY+100.0);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");

		currentCamera->ProjectionMatrix = Matrix4::OpenGLPersp(45.0f, (float)this->windowWidth / (float)this->windowHeight, this->near, this->far);
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void
	SimpleWaterApp::LoadShaders()
	{
		ShaderManager::Instance()->AddShader("color", GraphicsManager::LoadShaders("Resources/Shaders/VSColor.glsl", "Resources/Shaders/FSColor.glsl"));
		ShaderManager::Instance()->AddShader("water", GraphicsManager::LoadShaders("Resources/Shaders/VSWater.glsl", "Resources/Shaders/FSWater.glsl"));
		ShaderManager::Instance()->AddShader("quadToScreen", GraphicsManager::LoadShaders("Resources/Shaders/VSQuadToScreen.glsl", "Resources/Shaders/FSQuadToScreen.glsl"));
		ShaderManager::Instance()->AddShader("skybox", GraphicsManager::LoadShaders("Resources/Shaders/VSSkybox.glsl", "Resources/Shaders/FSSkybox.glsl"));
		ShaderManager::Instance()->AddShader("blur", GraphicsManager::LoadShaders("Resources/Shaders/VSBlur.glsl", "Resources/Shaders/FSBlur.glsl"));
		ShaderManager::Instance()->AddShader("hdrBloom", GraphicsManager::LoadShaders("Resources/Shaders/VSHDRBloom.glsl", "Resources/Shaders/FSHDRBloom.glsl"));
	}

	void
	SimpleWaterApp::SetUpFrameBuffer(int windowWidth, int windowHeight)
	{
		//set up frame buffer

		// Create the FBO
		glGenFramebuffers(1, &frameBufferHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

		glGenTextures(1, &reflectionBufferHandle);
		glBindTexture(GL_TEXTURE_2D, reflectionBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionBufferHandle, 0);

		glGenTextures(1, &refractionBufferHandle);
		glBindTexture(GL_TEXTURE_2D, refractionBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, refractionBufferHandle, 0);

		glGenTextures(1, &depthTextureBufferHandle);
		glBindTexture(GL_TEXTURE_2D, depthTextureBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureBufferHandle, 0);

		// Disable reading to avoid problems with older GPUs
		glReadBuffer(GL_NONE);
		//no color buffer
		glDrawBuffer(GL_NONE);

		// Verify that the FBO is correct
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			return;
		}

		// Restore the default framebuffer
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void
	SimpleWaterApp::SetUpPostBuffer(int windowWidth, int windowHeight)
	{
		//set up frame buffer

		// Create the FBO
		glGenFramebuffers(1, &postFrameBufferHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, postFrameBufferHandle);

		glGenTextures(1, &hdrBufferTextureHandle);
		glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrBufferTextureHandle, 0);

		glGenTextures(1, &brightLightBufferHandle);
		glBindTexture(GL_TEXTURE_2D, brightLightBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightLightBufferHandle, 0);

		glGenTextures(1, &postDepthBufferHandle);
		glBindTexture(GL_TEXTURE_2D, postDepthBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postDepthBufferHandle, 0);

		// Disable reading to avoid problems with older GPUs
		glReadBuffer(GL_NONE);
		//no color buffer
		glDrawBuffer(GL_NONE);

		// Verify that the FBO is correct
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (Status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error, status: 0x%x\n", Status);
			return;
		}

		// Restore the default framebuffer
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void
	SimpleWaterApp::SetUpBlurrBuffer(int windowWidth, int windowHeight)
	{
		//set up blur frame buffer
		// Create the FBO
		glGenFramebuffers(2, blurFrameBufferHandle);
		glGenTextures(2, blurBufferHandle);
		for (int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBufferHandle[i]);
			glBindTexture(GL_TEXTURE_2D, blurBufferHandle[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
			//glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurBufferHandle[i], 0);

			// Disable reading to avoid problems with older GPUs
			glReadBuffer(GL_NONE);
			//no color buffer
			glDrawBuffer(GL_NONE);
			//glDrawBuffer(GL_COLOR_ATTACHMENT0);
			//glDrawBuffer(GL_NONE);
			// Verify that the FBO is correct
			GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

			if (Status != GL_FRAMEBUFFER_COMPLETE) {
				printf("FB error, status: 0x%x\n", Status);
				return;
			}

			// Restore the default framebuffer
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void
	SimpleWaterApp::DrawGUI()
	{
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Objects rendered %d", objectsRendered);

		ImGui::NewLine();
		ImGui::Checkbox("Post Effects:", &post);
		ImGui::Checkbox("HDR", &hdrEnabled);
		ImGui::Checkbox("Bloom", &bloomEnabled);
		ImGui::SliderInt("Bloom Size", &bloomSize, 0, 10);
		ImGui::SliderFloat("Bloom Intensity", &bloomIntensityF, 0.0f, 5.f);
		ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
		ImGui::SliderFloat("Gamma", &gamma, 0.0f, 5.0f);

		ImGui::NewLine();
		ImGui::Text("LIGHTING:");
		ImGui::ColorEdit3("Sun Color", (float*)&light_color);
		ImGui::SliderFloat("Sun Power", &light_power, 0.0f, 10.0f);
		ImGui::SliderFloat("Sun Height", &sun_height, 0.0f, 45.0f);
		ImGui::SliderFloat("Rotate Sun", &sun_angle, 0.0f, 360.0f);
		lightInvDir = Matrix3F::rotateAngle(Vector3F(0.f, 1.f, 0.f), sun_angle) * (Vector3F(-25.f, sun_height, 0.f) - Vector3F(0.f, 0.f, 0.f));


		ImGui::NewLine();
		ImGui::Text("SHADING:");
		ImGui::ColorEdit3("Water Color", (float*)&water_color);
		ImGui::SliderFloat("Water Spec Intensity", (float*)&water->mat->specularIntensity, 0.0f, 5.0f);
		ImGui::SliderFloat("Water Shininess", (float*)&water->mat->shininess, 0.0f, 100.0f);

		ImGui::SliderFloat("Water Speed", &speed_multiplier, 0.0f, 5.0f);
		water_speed = (float)Time::currentTime * 0.2f * speed_multiplier;

		ImGui::SliderFloat("Water Tile", &water_tiling, 0.0f, 30.0f);
		water->mat->tileX = water_tiling;
		water->mat->tileY = water_tiling;

		ImGui::SliderFloat("Distortion Strength", &wave_distortion, 0.0f, 10.0f);
		wave_strength = wave_distortion / 100.f;
		
		ImGui::SliderFloat("Shore Transparency", &max_depth_transparent, 0.0f, 10.0f);

		ImGui::SliderFloat("Water Color/Refraction", &water_color_refraction_blend, 0.0f, 100.0f);

		ImGui::SliderFloat("Fresnel (Refl/Refr)", &fresnelAdjustment, 0.0f, 5.0f);

		ImGui::SliderFloat("Soften Normals", &soften_normals, 2.0f, 9.0f);
		ImGui::End();
	}

	void
	SimpleWaterApp::DrawSkybox()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLuint currentShader = ShaderManager::Instance()->shaderIDs["skybox"];
		ShaderManager::Instance()->SetCurrentShader(currentShader);

		glDepthMask(GL_FALSE);
		Matrix4 View = currentCamera->ViewMatrix;
		Matrix4 Projection = currentCamera->ProjectionMatrix;

		View[3][0] = 0;
		View[3][1] = 0;
		View[3][2] = 0;
		float plane[4] = { 0.0, 1.0, 0.0, 100000.146f }; //water at y=0 //last value is for water height

		GLuint planeHandle = glGetUniformLocation(currentShader, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);
		Matrix4 ViewProjection = View*Projection;
		Render::drawSkybox(skybox, ViewProjection, currentShader);

		glDepthMask(GL_TRUE);
	}

	void
	SimpleWaterApp::Draw()
	{

		Matrix4 View = currentCamera->ViewMatrix;
		Matrix4 Projection = currentCamera->ProjectionMatrix;
		Matrix4 ViewProjection = View*Projection;

		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		float plane[4] = { 0.0, 1.0, 0.0, 10000.f };
		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View.toFloat()[0][0]);

		GLuint fTime = glGetUniformLocation(currentShaderID, "fTime");
		glUniform1f(fTime, (float)Time::currentTime);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Matrix4 viewModel = View.inverse();
		Vector3F camPos = viewModel.getPosition().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint screenSize = glGetUniformLocation(currentShaderID, "screenSize");
		Vector2F scrSize = Vector2F((float)windowWidth, (float)windowHeight);
		glUniform2fv(screenSize, 1, &scrSize.x);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		objectsRendered = 0;
		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, ViewProjection, currentShaderID);
				objectsRendered++;
			}
		}
	}

	void
	SimpleWaterApp::DrawReflection()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float plane[4] = { 0.0, 1.0, 0.0, 0.f }; 
		glEnable(GL_CLIP_PLANE0);

		glCullFace(GL_FRONT);
		Matrix4 View = currentCamera->ViewMatrix;

		Vector3 pos = currentCamera->GetInitPos();
		pos.y = -pos.y;
		Vector3 dir = currentCamera->getDirection();
		dir.y = -dir.y;
		Vector3 right = currentCamera->getRight();
		View = Matrix4::lookAt(
			pos,
			pos + dir,
			right.crossProd(dir)
		);
		View = View*Matrix4::scale(Vector3(1.f, -1.f, 1.f));

		Matrix4 Projection = currentCamera->ProjectionMatrix;
		Matrix4 ViewProjection = View*Projection;

		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["skybox"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		glDepthMask(GL_FALSE);
		Matrix4 skyboxView = View;
		skyboxView[3][0] = 0;
		skyboxView[3][1] = 0;
		skyboxView[3][2] = 0;

		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		Matrix4 ViewProjection2 = skyboxView*Projection;
		Render::drawSkybox(skybox, ViewProjection2, currentShaderID);

		glDepthMask(GL_TRUE);

		currentShaderID = ShaderManager::Instance()->shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View.toFloat()[0][0]);

		GLuint fTime = glGetUniformLocation(currentShaderID, "fTime");
		glUniform1f(fTime, (float)Time::currentTime);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Matrix4 viewModel = View.inverse();
		Vector3F camPos = viewModel.getPosition().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint screenSize = glGetUniformLocation(currentShaderID, "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		FrustumManager::Instance()->ExtractPlanes(ViewProjection); //we do frustum culling against reflected frustum planes
		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, ViewProjection, currentShaderID);
			}
		}

		glCullFace(GL_BACK);
	}

	void
	SimpleWaterApp::DrawRefraction()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(1, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		float plane[4] = { 0.0, -1.0, 0.0, 0.146f }; //water at y=0 //last value is for water height
		glEnable(GL_CLIP_PLANE0);


		Matrix4F View = currentCamera->ViewMatrix.toFloat();

		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint ViewMatrixHandle = glGetUniformLocation(currentShaderID, "V");
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, &View[0][0]);
		GLuint fTime = glGetUniformLocation(currentShaderID, "fTime");
		glUniform1f(fTime, (float)Time::currentTime);
		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);
		GLuint screenSize = glGetUniformLocation(currentShaderID, "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);

		for (auto& obj : Scene::Instance()->pickingList)
		{
			if (FrustumManager::Instance()->isBoundingSphereInView(obj.second->GetWorldPosition(), obj.second->radius)) {
				Render::draw(obj.second, CameraManager::Instance()->ViewProjection, currentShaderID);
			}
		}
		glDisable(GL_CLIP_PLANE0);
	}

	void
	SimpleWaterApp::DrawWater()
	{
		GLuint currentShaderID = ShaderManager::Instance()->shaderIDs["water"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionBufferHandle);
		GLuint reflectionSampler = glGetUniformLocation(currentShaderID, "reflectionSampler");
		glUniform1i(reflectionSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionBufferHandle);
		GLuint refractionSampler = glGetUniformLocation(currentShaderID, "refractionSampler");
		glUniform1i(refractionSampler, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GraphicsStorage::textures[1]->TextureID); //normal
		GLuint normalSampler = glGetUniformLocation(currentShaderID, "normalMapSampler");
		glUniform1i(normalSampler, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, GraphicsStorage::textures[2]->TextureID); //dudv
		GLuint dudvSampler = glGetUniformLocation(currentShaderID, "dudvMapSampler");
		glUniform1i(dudvSampler, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthTextureBufferHandle);
		GLuint depthSampler = glGetUniformLocation(currentShaderID, "depthMapSampler");
		glUniform1i(depthSampler, 4);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);
		GLuint screenSize = glGetUniformLocation(currentShaderID, "screenSize");
		glUniform2f(screenSize, (float)windowWidth, (float)windowHeight);

		GLuint near = glGetUniformLocation(currentShaderID, "near");
		glUniform1f(near, this->near);
		GLuint far = glGetUniformLocation(currentShaderID, "far");
		glUniform1f(far, this->far);

		GLuint waterColor = glGetUniformLocation(currentShaderID, "waterColor");
		glUniform3fv(waterColor, 1, &water_color.x);

		GLuint fTime = glGetUniformLocation(currentShaderID, "fTime");
		glUniform1f(fTime, water_speed);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		GLuint waveStr = glGetUniformLocation(currentShaderID, "waveStrength");
		glUniform1f(waveStr, wave_strength);

		GLuint maxDepthTransparent = glGetUniformLocation(currentShaderID, "maxDepthTransparent");
		glUniform1f(maxDepthTransparent, max_depth_transparent);

		GLuint waterRefractionBlend = glGetUniformLocation(currentShaderID, "waterRefractionDepth");
		glUniform1f(waterRefractionBlend, water_color_refraction_blend);

		Matrix4 dModel = water->CalculateOffsettedModel();
		Matrix4F ModelMatrix = dModel.toFloat();
		Matrix4F MVP = (dModel*CameraManager::Instance()->ViewProjection).toFloat();

		GLuint MatrixHandle = glGetUniformLocation(currentShaderID, "MVP");
		glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);

		GLuint ModelMatrixHandle = glGetUniformLocation(currentShaderID, "M");
		glUniformMatrix4fv(ModelMatrixHandle, 1, GL_FALSE, &ModelMatrix[0][0]);

		GLuint shininess = glGetUniformLocation(currentShaderID, "shininess");
		glUniform1f(shininess, water->mat->shininess);

		GLuint specularIntensity = glGetUniformLocation(currentShaderID, "specularIntensity");
		glUniform1f(specularIntensity, water->mat->specularIntensity);

		GLuint tiling = glGetUniformLocation(currentShaderID, "tiling");
		glUniform2f(tiling, water->mat->tileX, water->mat->tileY);

		GLuint fresnel = glGetUniformLocation(currentShaderID, "fresnelAdjustment");
		glUniform1f(fresnel, fresnelAdjustment);

		GLuint watersize = glGetUniformLocation(currentShaderID, "waterSize");
		glUniform1i(watersize, waterSize);

		GLuint softenNormals = glGetUniformLocation(currentShaderID, "softenNormals");
		glUniform1f(softenNormals, soften_normals);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//bind vao before drawing
		glBindVertexArray(water->mesh->vaoHandle);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, water->mesh->indicesSize, GL_UNSIGNED_INT, (void*)0); // mode, count, type, element array buffer offset

		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SimpleWaterApp::BlurLight()
	{
		GLuint currentShader = ShaderManager::Instance()->shaderIDs["blur"];
		ShaderManager::Instance()->SetCurrentShader(currentShader);

		GLuint scaleUniform = glGetUniformLocation(currentShader, "scaleUniform");

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurFrameBufferHandle[1]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 1.0f / (float)windowWidth, 0.0f); //horizontally

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brightLightBufferHandle);

		DebugDraw::Instance()->DrawQuad();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurFrameBufferHandle[0]); //final blur result is stored in the blurFrameBufferHandle 0
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform2f(scaleUniform, 0.0f, 1.0f / (float)windowHeight); //vertically

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurBufferHandle[1]);

		DebugDraw::Instance()->DrawQuad();

		for (int i = 0; i < bloomSize; i++) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurFrameBufferHandle[1]);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 1.0f / (float)windowWidth, 0.0f); //horizontally

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blurBufferHandle[0]);

			DebugDraw::Instance()->DrawQuad();

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurFrameBufferHandle[0]); //final blur result is stored in the blurFrameBufferHandle 0
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform2f(scaleUniform, 0.0f, 1.0f / (float)windowHeight); //vertically

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blurBufferHandle[1]);

			DebugDraw::Instance()->DrawQuad();
		}
	}

	void
	SimpleWaterApp::DrawHDR()
	{
		//we draw color to the screen
		GLuint hdrBloom = ShaderManager::Instance()->shaderIDs["hdrBloom"];
		ShaderManager::Instance()->SetCurrentShader(hdrBloom);

		GLuint hdrEnabled = glGetUniformLocation(hdrBloom, "hdr");
		GLuint bloomEnabled = glGetUniformLocation(hdrBloom, "bloom");
		GLuint exposure = glGetUniformLocation(hdrBloom, "exposure");
		GLuint gamma = glGetUniformLocation(hdrBloom, "gamma");
		GLuint bloomIntensity = glGetUniformLocation(hdrBloom, "bloomIntensity");		

		glUniform1i(hdrEnabled, this->hdrEnabled);
		glUniform1f(exposure, this->exposure);
		glUniform1f(gamma, this->gamma);
		glUniform1i(bloomEnabled, this->bloomEnabled);
		glUniform1f(bloomIntensity, this->bloomIntensityF);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
		GLuint hdrBuffer = glGetUniformLocation(hdrBloom, "hdrBuffer");
		glUniform1i(hdrBuffer, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurBufferHandle[0]); //blurred bright light(bloom)
		GLuint bloomBuffer = glGetUniformLocation(hdrBloom, "bloomBuffer");
		glUniform1i(bloomBuffer, 1);

		DebugDraw::Instance()->DrawQuad();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void
	SimpleWaterApp::DrawTextures(int width, int height)
	{
		ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["quadToScreen"]);

		float fHeight = (float)height;
		float fWidth = (float)width;
		int y = (int)(fHeight*0.20f);
		int glWidth = (int)(fWidth *0.20f);
		int glHeight = (int)(fHeight*0.20f);

		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, glWidth, glHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(0, 0, glWidth, glHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionBufferHandle);
		DebugDraw::Instance()->DrawQuad();

		glEnable(GL_SCISSOR_TEST);
		glScissor(width - glWidth, 0, glWidth, glHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(width - glWidth, 0, glWidth, glHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, refractionBufferHandle);
		DebugDraw::Instance()->DrawQuad();

		glEnable(GL_SCISSOR_TEST);
		glScissor(width - glWidth, height - glHeight, glWidth, glHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(width - glWidth, height - glHeight, glWidth, glHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blurBufferHandle[0]);
		DebugDraw::Instance()->DrawQuad();

		glEnable(GL_SCISSOR_TEST);
		glScissor(0, height - glHeight, glWidth, glHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glViewport(0, height - glHeight, glWidth, glHeight);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
		DebugDraw::Instance()->DrawQuad();

		glBindTexture(GL_TEXTURE_2D, 0);
		glViewport(0, 0, width, height);
	}

	Mesh* SimpleWaterApp::GenerateWaterMesh(int width, int height)
	{
		std::vector<unsigned int> indices;
		std::vector<VertexData> vertexData;
		Mesh* waterMesh = new Mesh();
		OBJ* obj = new OBJ();

		int vertexCount = width * height;
		int squares = (width - 1) * (height - 1);
		int triangles = squares * 2;
		int indicesCount = triangles * 3;

		vertexData.reserve(vertexCount);
		indices.reserve(indicesCount);

		for (int col = 0; col < width; col++)
		{
			for (int row = 0; row < height; row++)
			{
				//since i store the points column wise the next column starts at index = current column * height
				int currentColumn = height * col;
				vertexData.push_back(VertexData((float)col, (float)row));
				//we never do the last row nor last column, we don't do that with borders since they are already a part border faces that were build in previous loop
				if (col == width - 1 || row == height - 1) continue; //this might be more expensive than writing another for loop set just for indices

				//face 1
				//vertex 0 is current column and current row
				//vertex 1 is current column and current row + 1
				//vertex 2 is current column + 1 and current row + 1
				int nextColumn = height * (col + 1); //or currentColumn + height //will use that later
				indices.push_back(currentColumn + row);
				indices.push_back(currentColumn + row + 1);
				indices.push_back(nextColumn + row + 1); //we need to calculate the next column here
				//face 2
				//vertex 0 is current column + 1 and current row + 1 i.e. same as face 1 vertex 2
				//vertex 1 is current column + 1 and current row
				//vertex 2 is current column and current row i.e. same as face 1 vertex 1
				indices.push_back(nextColumn + row + 1);
				indices.push_back(nextColumn + row);
				indices.push_back(currentColumn + row);
			}
		}

		//Create VAO
		glGenVertexArrays(1, &waterMesh->vaoHandle);
		//Bind VAO
		glBindVertexArray(waterMesh->vaoHandle);

		// 1rst attribute buffer : vertices
		glGenBuffers(1, &waterMesh->vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, waterMesh->vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(VertexData), &vertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		// 2th element buffer Generate a buffer for the indices as well
		glGenBuffers(1, &waterMesh->elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterMesh->elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		waterMesh->indicesSize = (unsigned int)indices.size();

		//Unbind the VAO now that the VBOs have been set up
		glBindVertexArray(0);

		obj->dimensions = Vector3((float)width, 0.f, (float)height);
		obj->center_of_mass = obj->dimensions / 2.f;

		waterMesh->obj = obj;

		return waterMesh;
	}

	void SimpleWaterApp::UpdateTextureBuffers(int windowWidth, int windowHeight)
	{
		glBindTexture(GL_TEXTURE_2D, reflectionBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, refractionBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, depthTextureBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, brightLightBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, blurBufferHandle[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, blurBufferHandle[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		
		glBindTexture(GL_TEXTURE_2D, postDepthBufferHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

} // namespace