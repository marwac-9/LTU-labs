//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"

#include <vector>
class BoundingBox;
class Object;
class OBJ;
class HalfEdgeMesh;
class Camera;
class FrameBuffer;
class Texture;
class Vao;

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

		
		void InitGL();
		void KeyCallback(int key, int scancode, int action, int mods);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		void LoadScene1();

		void SetUpBuffers(int windowWidth, int windowHeight);

		void GenerateGUI();
		void DrawSkybox();
		void Draw();
		void DrawReflection();
		void DrawGeoUnderWater();
		void DrawWater();
		void DrawHDR(Texture* texture);
		void DrawTextures(int width, int height);

		Vao* GenerateWaterMesh(int width, int height);
		
		bool windowLocked = true;
		bool minimized = false;
		bool running = false;
		bool wireframe = false;
		Display::Window* window;
		bool isLeftMouseButtonPressed = false;
		double leftMouseX;
		double leftMouseY;
		int windowWidth;
		int windowHeight;
		float windowMidX;
		float windowMidY;
		float near = 0.1f;
		float far = 1000.f;
		float fov = 45.f;

		Camera* currentCamera;
		
		mwm::Vector3 lightPos = mwm::Vector3();
		int objectsRendered = 0;

		Object* water = nullptr;

		Texture* reflectionBufferTexture;
		Texture* underWaterBufferTexture;
		Texture* depthTextureBufferTexture;
		GLuint depthBufferHandle;
		FrameBuffer* frameBuffer;

		FrameBuffer* postFrameBuffer;
		Texture* hdrTexture;
		Texture* brightLightTexture;
		Texture* blurredBrightTexture;

		Object* selectedObject = nullptr;
		

		std::vector<Object*> dynamicObjects;
		std::vector<Vao*> dynamicMeshes;
		double waterShaderTime = 0.0;
		//shader variables:
		mwm::Vector3F water_color = mwm::Vector3F(0.0f, 0.6f, 0.5f);
		float water_speed = 0.16f;
		float speed_multiplier = 1.0f;
		float water_tiling = 8.f;
		float sun_angle = 260.f;
		float sun_height = 2.f;
		float light_power = 1.f;
		mwm::Vector3F light_color = mwm::Vector3F(1.0f, 0.89f, 0.74f);
		mwm::Vector3F lightInvDir = mwm::Vector3F(-15.f, 2.f, 0.f);
		float wave_strength = 0.04f;
		float wave_distortion = 4.f;
		float max_depth_transparent = 4.f;
		float water_transparency_depth = 23.f;
		float fresnelAdjustment = 1.f;
		float soften_normals = 3.0;
		int waterSize = 2;
		float blurSize = 1.0f;
		int blurLevel = 0;
		bool post = true;
		const int querycount = 5;
		GLuint queries[5];
		int current_query = 0;
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
