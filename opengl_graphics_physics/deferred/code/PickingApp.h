//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"

class BoundingBox;
class Object;
class Camera;

enum loadedScene
{
	scene1Loaded,
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

		void Draw(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void DrawDebug(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);

		void PassPickingTexture(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
        void PickingTest();

		void DrawGeometryPass(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);

		void DrawLightPass(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix, const mwm::Vector3& camPos);
		void DrawPointLights(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix, const mwm::Vector3& camPos);
		void DrawDirectionalLights(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void StencilPass(Object* pointLight, const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix, const mwm::Vector3& camPos);
		void PointLightPass(Object* pointLight, const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix, const mwm::Vector3& camPos);
		void BlitToScreenPass();

		void IntegrateAndUpdateBoxes(float timestep);
		void IntegrateLights(float timestep);
        void InitGL();
        void ClearBuffers();
        void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera(float timeStep);
		mwm::Vector3 ConvertMousePosToWorld();
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void LoadScene4();
		void Vortex();
		
		void ActivateTextures();
		void MoveObjectUpNDown();
		void DisableTextures();
		void DrawGeometry(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void SpawnSomeLights();
		void LoadShaders();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 1;

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
		mwm::Matrix4 ProjectionMatrix;
		mwm::Matrix4 ViewMatrix;
		Camera* currentCamera;
        Object* lastPickedObject = nullptr;
		float timeModifier = 0.f;
		int objectsRendered = 0;
		int lightsRendered = 0;
		unsigned int pickedID = 0;
		BoundingBox* boundingBox;
		GLuint LightID;

		loadedScene currentScene = none;
		mwm::Vector3 lightInvDir = mwm::Vector3(-1.f, 1.f, 1.f);
		float planeDir = -1;
		double timeCounter;
	};
} // namespace Example