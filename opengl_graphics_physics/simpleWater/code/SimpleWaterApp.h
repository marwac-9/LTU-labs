//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "gl_window.h"
#include "MyMathLib.h"

#include <vector>
class BoundingBox;
class Object;
class OBJ;
class Mesh;
class HalfEdgeMesh;
class Camera;

namespace SimpleWater
{
	class SimpleWaterApp : public Core::App
	{
	public:
		/// constructor
		SimpleWaterApp();
		/// destructor
		~SimpleWaterApp();

		/// open app
		bool Open();
		/// run app
		void Run();
	private:
		void Clear();

		void Draw();
		void InitGL();
		void ClearBuffers();
		void KeyCallback(int key, int scancode, int action, int mods);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		void LoadScene1();
		void LoadShaders();
		void DrawWater();
		bool windowLocked = true;

		bool running = false;
		bool wireframe = false;
		Display::Window* window;
		double leftMouseX;
		double leftMouseY;
		int windowWidth;
		int windowHeight;
		float windowMidX;
		float windowMidY;
		float near = 0.1f;
		float far = 1000.f;

		Camera* currentCamera;
		
		mwm::Vector3 lightPos = mwm::Vector3();
		int objectsRendered = 0;

		Object* water;

		GLuint reflectionBufferHandle;
		GLuint refractionBufferHandle;
		GLuint depthTextureBufferHandle;
		GLuint depthBufferHandle;
		GLuint frameBufferHandle;
		Object* selectedObject;
		Object* skybox;
		bool minimized = false;

		std::vector<Object*> dynamicObjects;
		void DrawReflection();
		void DrawRefraction();
		void SetUpFrameBuffer(int windowWidth, int windowHeight);
		void DrawTextures(int width, int height);
		void DrawSkybox();
		void DrawGUI();
		Mesh* GenerateWaterMesh(int width, int height);
		void UpdateTextureBuffers(int windowWidth, int windowHeight);
		//shader variables:
		mwm::Vector3 water_color = mwm::Vector3(0.0, 0.6, 0.5);
		float water_speed = 0.16f;
		float speed_multiplier = 1.0f;
		float water_tiling = 6.f;
		float sun_angle = 260.f;
		float sun_height = 2.f;
		float light_power = 1.f;
		mwm::Vector3 light_color = mwm::Vector3(1.0, 0.89, 0.74);
		mwm::Vector3 lightInvDir = mwm::Vector3(-15.f, 2.f, 0.f);
		float wave_strength = 0.04f;
		float wave_distortion = 4.f;
		float max_depth_transparent = 4.f;
		float water_color_refraction_blend = 23.f;
		float fresnelAdjustment = 1.f;
		float soften_normals = 3.0;
		int waterSize = 2;
		

#pragma pack (push)
#pragma pack(1)
		struct VertexData
		{
			float x, z;
			//float u2, v2;
			VertexData(){}
			//VertexData(float x1, float z1, float u_1, float v_1, float u_2, float v_2) : x(x1), z(z1), u1(u_1), v1(v_1), u2(u_2), v2(v_2)
			VertexData(float x1, float z1) : x(x1), z(z1)
			{}
		};
#pragma pack (pop) 

	};
} // namespace 