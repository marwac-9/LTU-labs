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

		void Draw();
		void DrawDebug();

		void PassPickingTexture();
        void PickingTest();

		void DrawGeometryPass();

		void DrawLightPass();
		void DrawPointLights();
		void DrawDirectionalLights();
		void StencilPass(Object* pointLight);
		void PointLightPass(Object* pointLight);
		void BlitToScreenPass();

		void IntegrateAndUpdateBoxes();
		void IntegrateLights();
        void InitGL();
        void ClearBuffers();
        void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		mwm::Vector3 ConvertMousePosToWorld();
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void Vortex();
		
		void ActivateTextures();
		void MovePlaneUpNDown();
		void DisableTextures();
		void DrawGeometry();
		void SpawnSomeLights();
		void LoadShaders();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 1;
		Camera* currentCamera;
        bool running = false;
		bool debug = false;
		bool paused = false;
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
		BoundingBox* boundingBox;
		GLuint LightID;

		loadedScene currentScene = none;
		mwm::Vector3 lightInvDir = mwm::Vector3(-1.f, 1.f, 1.f);
		float planeDir = -1;
	};
} // namespace Example