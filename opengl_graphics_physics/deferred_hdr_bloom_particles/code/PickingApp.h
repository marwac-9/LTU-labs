//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "gl_window.h"
#include "MyMathLib.h"
#include <vector>

class ParticleSystem;
class BoundingBox;
class Object;
class Camera;
class FrameBuffer;
class Texture;

enum loadedScene
{
	scene1Loaded,
	scene2Loaded,
	scene3Loaded,
	scene4Loaded,
	none
};

namespace Picking
{
    class PickingApp : public Core::App
    {
    public:
        /// constructor
        PickingApp();
        /// destructor
        ~PickingApp();

        /// open app
        bool Open();
        /// run app
        void Run();
    private:
		void Clear();

		void DrawPicking();
		void DrawDebug();

		void PassPickingTexture();
        void PickingTest();

		void DrawGeometryPass();

		void DrawLightPass();
		void BlitToScreenPass();
		void FireLightProjectile();

		void GenerateGUI();

		void DrawHDR(Texture* texture);

		void SetUpBuffers(int windowWidth, int windowHeight);
		void DrawGeometryMaps(int width, int height);

        void InitGL();
        void ClearBuffers();
        void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void LoadScene4();
		void Vortex();
		
		void MovePlaneUpNDown();
		void DrawParticles();
		void SpawnSomeLights();
		void LoadShaders();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 1;
		Camera* currentCamera;
        bool running = false;
		bool debug = false;
        Display::Window* window;
        bool isLeftMouseButtonPressed = false;
        double leftMouseX;
        double leftMouseY;
        int windowWidth;
        int windowHeight;
		float windowMidX;
		float windowMidY;
		bool wireframe = false;
		bool lightsPhysics = false;

        Object* lastPickedObject = nullptr;
		Object* plane = nullptr;
		int objectsRendered = 0;
		int lightsRendered = 0;
		unsigned int pickedID = 0;

		Object* pointL;
		loadedScene currentScene = none;
		mwm::Vector3F lightInvDir = mwm::Vector3F(-1.f, 1.f, 1.f);
		float planeDir = -1;
		std::vector<ParticleSystem*> particleSystems;
		float near = 0.1f;
		float far = 2000.f;
		float fov = 45.0f;
		int bloomLevel = 3;
		float blurBloomSize = 1.5f;
		bool minimized = false;
		bool hdrEnabled = GL_TRUE;
		float exposure = 1.0f;
		float gamma = 1.2f;

		GLuint diffuseTextureHandle;
		GLuint normalTextureHandle;
		GLuint metDiffIntShinSpecIntTextureHandle;

		FrameBuffer* lightAndPostBuffer;
		GLuint finalColorTextureHandle;
		Texture* brightLightTexture;

		Texture* blurredBrightTexture;

		FrameBuffer* geometryBuffer;
		FrameBuffer* pickingBuffer;
		Texture* pickingTexture;
		Texture* worldPosTexture;
	};
} // namespace Example