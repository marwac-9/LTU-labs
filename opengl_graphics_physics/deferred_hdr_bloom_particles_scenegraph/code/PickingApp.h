//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"
#include <vector>

class ParticleSystem;
class LineSystem;
class BoundingBoxSystem;
class BoundingBox;
class Object;
class Camera;
class Node;
class FastLine;
class PointSystem; 

enum loadedScene
{
	scene1Loaded,
	scene2Loaded,
	scene3Loaded,
	scene4Loaded,
	scene5Loaded,
	scene6Loaded,
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
		void DrawDebugInstanced();

		void PassPickingTexture();
        void PickingTest();

		void DrawGeometryPass();
		void PIDController();
		void DrawLightPass();
		void DrawPointLights();
		void DrawDirectionalLights();
		void StencilPass(Object* pointLight);
		void PointLightPass(Object* pointLight);
		void BlitToScreenPass();
		void FireLightProjectile();

		void DrawHDR();
		void BlurLight();

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
		void LoadScene4();
		void LoadScene5();
		void LoadScene6();
		void Vortex();
		
		void ActivateTextures();
		void MovePlaneUpNDown();
		void DisableTextures();
		void DrawGeometry();
		void DrawParticles();
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
		bool drawLines = true;
        Object* lastPickedObject = nullptr;
		Object* plane = nullptr;
		int objectsRendered = 0;
		int lightsRendered = 0;
		unsigned int pickedID = 0;
		BoundingBox* boundingBox;
		GLuint LightID;
		Object* pointL;
		loadedScene currentScene = none;
		mwm::Vector3F lightInvDir = mwm::Vector3F(-1.f, 1.f, 1.f);
		float planeDir = -1;
		std::vector<ParticleSystem*> particleSystems;
		std::vector<LineSystem*> lineSystems;
		std::vector<BoundingBoxSystem*> bbSystems;
		std::vector<PointSystem*> pointSystems;
		void DrawFastLineSystems();
		void DrawFastPointSystems();
		void GenerateFastLines();
		void GenerateAndDrawFastLineChildren(Node* parentPos, Node* child);
		int increment = 0;
		int prevGridPos[3];

	};
} // namespace Example