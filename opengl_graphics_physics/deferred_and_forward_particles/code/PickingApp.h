//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"
#include <vector>

class ParticleSystem;
class BoundingBox;
class Object;
class Camera;
class Texture;
class FrameBuffer;

enum loadedScene
{
	scene1Loaded,
	scene2Loaded,
	scene3Loaded,
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

        void PickingTest();

		void DrawGeometryPass();
		void DrawLightPass();
		void BlitToScreenPass();
		void FireLightProjectile();
		void DrawGeometryMaps(int windowWidth, int windowHeight);

        void InitGL();
		void SetUpBuffers(int windowWidth, int windowHeight);
        void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void Vortex();
		
		void MovePlaneUpNDown();
		void SpawnSomeLights();
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
		float near = 0.1f;
		float far = 2000.f;
		float fov = 45.0f;
		bool minimized = false;
        Object* lastPickedObject = nullptr;
		Object* plane = nullptr;
		int objectsRendered = 0;
		int lightsRendered = 0;
		unsigned int pickedID = 0;

		loadedScene currentScene = none;
		mwm::Vector3F lightInvDir = mwm::Vector3F(-1.f, 1.f, 1.f);
		float planeDir = -1;
		std::vector<ParticleSystem*> particleSystems;

		FrameBuffer* geometryBuffer;
		FrameBuffer* pickingBuffer;

		Texture* pickingTexture;
		Texture* worldPosTexture;

		Texture* diffuseTexture;
		Texture* normalTexture;
		Texture* metDiffIntShinSpecIntTexture;
	};
} // namespace Example
