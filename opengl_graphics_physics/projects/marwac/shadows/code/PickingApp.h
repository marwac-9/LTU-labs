//
// Created by marwac-9 on 9/16/15.
//
#include "core/app.h"
#include "render/window.h"
#include "MyMathLib.h"

class BoundingBox;
class Object;
class Camera;

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

		void DrawColorDebugPass(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void DrawDebugAndColor(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);

		void Draw(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void DrawDebug(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void DrawDepth(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void PassPickingTexture(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
        void PickingTest();
		
		void DrawDepthPass();
		void BlurShadowMap();
		void IntegrateAndUpdateBoxes(float timestep);
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
		void LoadScene5();
		void LoadShaders();
		void Vortex();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 2;

        bool running = false;
		bool debug = true;
		bool paused = false;
        Display::Window* window;
        bool isLeftMouseButtonPressed = false;
        double leftMouseX;
        double leftMouseY;
        int windowWidth;
        int windowHeight;
		float windowMidX;
		float windowMidY;

		mwm::Matrix4 ProjectionMatrix;
		mwm::Matrix4 ViewMatrix;
		Camera* currentCamera;
        Object* lastPickedObject = nullptr;
		float timeModifier = 0.f;
		int objectsRendered = 0;
		int pickedID = 0;
		BoundingBox* boundingBox;
		GLuint LightID;
		bool scene4loaded = false;
		mwm::Vector3 lightInvDir = mwm::Vector3(-1.f, 1.f, 1.f);
    };
} // namespace Example