#include "SimpleWaterApp.h"
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
#include "Camera.h"
#include "Frustum.h"
#include "Render.h"
#include "CameraManager.h"
#include <chrono>
#include "FBOManager.h"
#include "FrameBuffer.h"
#include "Times.h"

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

			FBOManager::Instance()->UpdateTextureBuffers(this->windowWidth, this->windowHeight);
			currentCamera->UpdateSize(width, height);
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

		SetUpBuffers(this->windowWidth, this->windowHeight);

		GraphicsManager::LoadAllAssets();
		LoadScene1();
		
		Times::Instance()->currentTime = glfwGetTime();
		window->SetCursorMode(GLFW_CURSOR_DISABLED);
		SetUpCamera();

		Scene::Instance()->Update();

		glfwSwapInterval(0); //unlock fps
		ImGui_ImplGlfwGL3_Init(this->window->GetGLFWWindow(), false);
		window->SetTitle("Simple Water");

		while (running)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			this->window->Update();
			if (minimized) continue;
			ImGui_ImplGlfwGL3_NewFrame();

			Times::Instance()->Update(glfwGetTime());

			Monitor(this->window);

			//is cursor window locked
			CameraManager::Instance()->Update(Times::Instance()->deltaTime);
			FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);
			
			Scene::Instance()->Update();
			//for HDR //draw current "to screen" to another texture draw water there too, then draw quad to screen from this texture
			//for Bloom //draw current "to screen" to yet another texture in same shaders
			GenerateGUI(); // <-- (generate) to screen
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->handle);
			DrawReflection(); // <-- to fb texture
			DrawRefraction(); // <-- to fb texture
			DrawSkybox(); // <-- to pf texture
			Draw(); // <-- to pf texture
			DrawWater(); // <-- to pf textures
			if (post)
			{
				blurredBrightTexture = Render::Instance()->MultiBlur(brightLightTexture, blurLevel, blurSize, GraphicsStorage::shaderIDs["fastBlur"]); //blur bright color
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				DrawHDR(blurredBrightTexture); // <-- to screen from hdr and bloom textures
			}
			else
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				ShaderManager::Instance()->SetCurrentShader(GraphicsStorage::shaderIDs["quadToScreen"]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, hdrBufferTextureHandle);
				DebugDraw::Instance()->DrawQuad();
			}
			
			
			DrawTextures(windowWidth, windowHeight);
			ImGui::Render(); // <-- (draw) to screen
			this->window->SwapBuffers();
		}
		GraphicsStorage::Clear();
		ImGui_ImplGlfwGL3_Shutdown();
		this->window->Close();
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
		currentCamera->fov = fov;
		currentCamera->near = near;
		currentCamera->far = far;
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
	}

	void
	SimpleWaterApp::LoadScene1()
	{
		Clear();

		//water object
		water = Scene::Instance()->addChild();
		dynamicObjects.push_back(water);

		Material* waterMaterial = new Material();
		water->AssignMaterial(waterMaterial);
		Mesh* waterMesh = GenerateWaterMesh(waterSize, waterSize);
		dynamicMeshes.push_back(waterMesh);

		water->AssignMesh(waterMesh);
		water->SetScale(Vector3(100.f, 1.f, 100.f));
		water->SetPosition(Vector3(-50.f, 0.f, 50.f));
		water->SetOrientation(Quaternion(90.f, Vector3(0.f, 1.f, 0.f)));

		waterMaterial->SetShininess(10.f);
		waterMaterial->SetSpecularIntensity(0.55f);
		waterMaterial->tileX = 6.f;
		waterMaterial->tileY = 6.f;
		GraphicsStorage::materials.push_back(waterMaterial);

		Object* sphere2 = Scene::Instance()->addObject("sphere", Vector3(0.f, -5.f, 0.f));
		sphere2->mat->SetShininess(20.f);
		sphere2->mat->SetSpecularIntensity(3.f);

		sphere2 = Scene::Instance()->addObject("sphere", Vector3(0.f, 5.f, 0.f));
		sphere2->SetScale(Vector3(4.f, 4.f, 4.f));
		sphere2->mat->SetShininess(20.f);
		sphere2->mat->SetSpecularIntensity(3.f);

		DebugDraw::Instance()->box.mat->AssignTexture(GraphicsStorage::cubemaps[0]);

		for (int i = 0; i < 20; i++)
		{
			Object* sphere = Scene::Instance()->addObject("icosphere", Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::axis::y, -5));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f,2.f,2.f));
		}

		for (int i = 0; i < 20; i++)
		{
			Object* sphere = Scene::Instance()->addObject("sphere", Scene::Instance()->generateRandomIntervallVectorFlat(-20, 20, Scene::axis::y, 5));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f, 2.f, 2.f));
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObject("cube", Vector3(30.f, i * 2.f, 30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
			//sphere->mat->SetColor(Vector3(2.f, 2.f, 2.f));
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObject("cube", Vector3(-30.f, i * 2.f, 30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObject("cube", Vector3(-30.f, i * 2.f, -30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 8; i++)
		{
			Object* sphere = Scene::Instance()->addObject("cube", Vector3(30.f, i * 2.f, -30.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		for (int i = 0; i < 12; i++)
		{
			Object* sphere = Scene::Instance()->addObject("cube", Vector3(0.f, -2.f + i * 2.f, 0.f));
			sphere->mat->SetSpecularIntensity(3.0f);
			sphere->mat->SetShininess(20.0f);
		}

		Object* plane = Scene::Instance()->addObject("pond", Vector3(0.f, 0.f, 0.f));
		plane->mat->AssignTexture(GraphicsStorage::textures[3]);
		plane->mat->tileX = 20;
		plane->mat->tileY = 20;
	}

	void
	SimpleWaterApp::SetUpCamera()
	{
		currentCamera = new Camera(Vector3(0.f, 25.f, 66.f), windowWidth, windowHeight);
		currentCamera->Update(Times::Instance()->timeStep);
		window->SetCursorPos(windowMidX, windowMidY+100.0);
		CameraManager::Instance()->AddCamera("default", currentCamera);
		CameraManager::Instance()->SetCurrentCamera("default");
		DebugDraw::Instance()->Projection = &currentCamera->ProjectionMatrix;
		DebugDraw::Instance()->View = &currentCamera->ViewMatrix;
	}

	void
	SimpleWaterApp::GenerateGUI()
	{
		ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::SliderFloat("Fov", &fov, 0.0f, 180.f);
		ImGui::SliderFloat("Near plane", &near, 0.0f, 5.f);
		ImGui::SliderFloat("Far plane", &far, 0.0f, 2000.f);

		ImGui::NewLine();
		ImGui::Checkbox("Post Effects:", &post);
		ImGui::Checkbox("HDR", &Render::Instance()->pb.hdrEnabled);
		ImGui::Checkbox("Bloom", &Render::Instance()->pb.bloomEnabled);
		ImGui::SliderFloat("Bloom Intensity", &Render::Instance()->pb.bloomIntensity, 0.0f, 5.f);
		ImGui::SliderFloat("Exposure", &Render::Instance()->pb.exposure, 0.0f, 5.0f);
		ImGui::SliderFloat("Gamma", &Render::Instance()->pb.gamma, 0.0f, 5.0f);
		ImGui::SliderFloat("Contrast", &Render::Instance()->pb.contrast, -5.0f, 5.0f);
		ImGui::SliderFloat("Brightness", &Render::Instance()->pb.brightness, -5.0f, 5.0f);
		ImGui::SliderFloat("Blur Size", &blurSize, 0.0f, 10.0f);
		ImGui::SliderInt("Blur Level", &blurLevel, 0, 3);

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
		water_speed = (float)Times::Instance()->currentTime * 0.2f * speed_multiplier;
		ImGui::SliderFloat("Water Tile", &water_tiling, 0.0f, 30.0f);
		water->mat->tileX = water_tiling;
		water->mat->tileY = water_tiling;
		ImGui::SliderFloat("Distortion Strength", &wave_distortion, 0.0f, 10.0f);
		wave_strength = wave_distortion / 100.f;
		ImGui::SliderFloat("Shore Transparency", &max_depth_transparent, 0.0f, 10.0f);
		ImGui::SliderFloat("Water Color/Refraction", &water_color_refraction_blend, 0.0f, 100.0f);
		ImGui::SliderFloat("Fresnel (Refl/Refr)", &fresnelAdjustment, 0.0f, 5.0f);
		ImGui::SliderFloat("Soften Normals", &soften_normals, 2.0f, 9.0f);


		ImGui::NewLine();
		ImGui::Text("STATS:");
		ImGui::Text("Objects rendered %d", objectsRendered);
		ImGui::Text("FPS %.3f", 1.0 / Times::Instance()->deltaTime);
		ImGui::Text("TimeStep %.3f", 1.0 / Times::Instance()->timeStep);
		ImGui::End();
	}

	void
	SimpleWaterApp::DrawSkybox()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		Vector4F plane = Vector4F(0.f, 1.f, 0.f, 100000.146f);
		Render::Instance()->drawSkyboxWithClipPlane(postFrameBuffer, DrawBuffers, 2, GraphicsStorage::cubemaps[0], plane, currentCamera->ViewMatrix);
	}

	void
	SimpleWaterApp::Draw()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postFrameBuffer->handle);
		GLuint currentShaderID = GraphicsStorage::shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		float plane[4] = { 0.0, 1.0, 0.0, 10000.f };
		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Matrix4 viewModel = currentCamera->ViewMatrix.inverse();
		Vector3F camPos = viewModel.getPosition().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		objectsRendered = Render::Instance()->draw(Scene::Instance()->renderList, CameraManager::Instance()->ViewProjection, currentShaderID);
	}

	void
	SimpleWaterApp::DrawReflection()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		Vector3 pos = currentCamera->GetInitPos();
		pos.y = -pos.y;
		Vector3 dir = currentCamera->direction;
		dir.y = -dir.y;
		Vector3 right = currentCamera->right;
		Matrix4 View = Matrix4::lookAt(
			pos,
			pos + dir,
			right.crossProd(dir)
		);
		View = View * Matrix4::scale(Vector3(1.f, -1.f, 1.f));

		glEnable(GL_CLIP_PLANE0);
		Vector4F plane = Vector4F(0.0, 1.0, 0.0, 0.f); //plane normal and height

		Render::Instance()->drawSkyboxWithClipPlane(frameBuffer, DrawBuffers, 1, GraphicsStorage::cubemaps[0], plane, View);

		GLuint currentShaderID = GraphicsStorage::shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Matrix4 viewModel = View.inverse();
		Vector3F camPos = viewModel.getPosition().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		Matrix4 ViewProjection = View * currentCamera->ProjectionMatrix;

		FrustumManager::Instance()->ExtractPlanes(ViewProjection); //we do frustum culling against reflected frustum planes

		Render::Instance()->draw(Scene::Instance()->renderList, ViewProjection, currentShaderID);

		glCullFace(GL_BACK);
	}

	void
	SimpleWaterApp::DrawRefraction()
	{
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(1, DrawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint currentShaderID = GraphicsStorage::shaderIDs["color"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		float plane[4] = { 0.0, -1.0, 0.0, 0.146f }; //plane normal and height
		glEnable(GL_CLIP_PLANE0);

		GLuint planeHandle = glGetUniformLocation(currentShaderID, "plane");
		glUniform4fv(planeHandle, 1, &plane[0]);

		GLuint CameraPos = glGetUniformLocation(currentShaderID, "CameraPos");
		Vector3F camPos = currentCamera->GetPosition2().toFloat();
		glUniform3fv(CameraPos, 1, &camPos.x);

		GLuint LightDir = glGetUniformLocation(currentShaderID, "LightInvDirection_worldspace");
		glUniform3fv(LightDir, 1, &lightInvDir.x);

		GLuint liPower = glGetUniformLocation(currentShaderID, "lightPower");
		glUniform1f(liPower, light_power);

		GLuint liColor = glGetUniformLocation(currentShaderID, "lightColor");
		glUniform3fv(liColor, 1, &light_color.x);

		FrustumManager::Instance()->ExtractPlanes(CameraManager::Instance()->ViewProjection);

		Render::Instance()->draw(Scene::Instance()->renderList, CameraManager::Instance()->ViewProjection, currentShaderID);

		glDisable(GL_CLIP_PLANE0);
	}

	void
	SimpleWaterApp::DrawWater()
	{
		GLuint currentShaderID = GraphicsStorage::shaderIDs["water"];
		ShaderManager::Instance()->SetCurrentShader(currentShaderID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionBufferTexture->handle);
		GLuint reflectionSampler = glGetUniformLocation(currentShaderID, "reflectionSampler");
		glUniform1i(reflectionSampler, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionBufferTexture->handle);
		GLuint refractionSampler = glGetUniformLocation(currentShaderID, "refractionSampler");
		glUniform1i(refractionSampler, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GraphicsStorage::textures[1]->handle); //normal
		GLuint normalSampler = glGetUniformLocation(currentShaderID, "normalMapSampler");
		glUniform1i(normalSampler, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, GraphicsStorage::textures[2]->handle); //dudv
		GLuint dudvSampler = glGetUniformLocation(currentShaderID, "dudvMapSampler");
		glUniform1i(dudvSampler, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthTextureBufferTexture->handle);
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

		Matrix4 dModel = water->node.TopDownTransform;
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
	}


	void
	SimpleWaterApp::SetUpBuffers(int windowWidth, int windowHeight)
	{
		frameBuffer = FBOManager::Instance()->GenerateFBO();
		Texture* reflectionTexture  = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT0)); //reflection
		Texture* refractionTexture = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1)); //refraction
		Texture* frameBufferDepthTexture = frameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT)); //depth
		frameBuffer->AddDefaultTextureParameters();
		frameBuffer->GenerateAndAddTextures();
		frameBuffer->CheckAndCleanup();

		reflectionBufferTexture = reflectionTexture;
		refractionBufferTexture = refractionTexture;
		depthTextureBufferTexture = frameBufferDepthTexture;

		postFrameBuffer = FBOManager::Instance()->GenerateFBO();

		hdrTexture = postFrameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT0)); //hdr
		brightLightTexture = postFrameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, GL_RGB, GL_FLOAT, NULL, GL_COLOR_ATTACHMENT1)); //brightLight
		Texture* postDepthBufferTexture = postFrameBuffer->RegisterTexture(new Texture(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, GL_DEPTH_ATTACHMENT)); //post depth

		postFrameBuffer->AddDefaultTextureParameters();
		postFrameBuffer->GenerateAndAddTextures();
		postFrameBuffer->CheckAndCleanup();

		Render::Instance()->AddMultiBlurBuffer(this->windowWidth, this->windowHeight);

		Render::Instance()->GenerateEBOs();
	}

	void
	SimpleWaterApp::DrawHDR(Texture* blurredBrightLightTexture)
	{
		Render::Instance()->drawHDR(hdrTexture, blurredBrightLightTexture);
	}

	void
	SimpleWaterApp::DrawTextures(int width, int height)
	{
		ShaderManager::Instance()->SetCurrentShader(GraphicsStorage::shaderIDs["quadToScreen"]);

		float fHeight = (float)height;
		float fWidth = (float)width;
		int y = (int)(fHeight*0.1f);
		int glWidth = (int)(fWidth *0.1f);
		int glHeight = (int)(fHeight*0.1f);

		Render::Instance()->drawRegion(0, 0, glWidth, glHeight, reflectionBufferTexture);
		Render::Instance()->drawRegion(width - glWidth, 0, glWidth, glHeight, refractionBufferTexture);
		Render::Instance()->drawRegion(width - glWidth, height - glHeight, glWidth, glHeight, blurredBrightTexture);
		Render::Instance()->drawRegion(0, height - glHeight, glWidth, glHeight, hdrTexture);
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
		obj->center_of_mesh = obj->dimensions / 2.f;

		waterMesh->obj = obj;

		return waterMesh;
	}
} // namespace
