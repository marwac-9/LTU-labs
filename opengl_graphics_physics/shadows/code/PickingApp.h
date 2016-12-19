//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
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

		void DrawColorDebugPass();

		void Draw();
		void DrawDebug();
		void DrawDepth(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void PassPickingTexture();
        void PickingTest();
		
		void DrawDepthPass();
		void BlurShadowMap();
		void UpdateComponents();
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
		void LoadShaders();
		void Vortex();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 2;

        bool running = false;
		bool debug = true;
		bool paused = false;
		bool wireframe = false;
        Display::Window* window;
        bool isLeftMouseButtonPressed = false;
        double leftMouseX;
        double leftMouseY;
        int windowWidth;
        int windowHeight;
		float windowMidX;
		float windowMidY;

		Camera* currentCamera;
        Object* lastPickedObject = nullptr;
		int objectsRendered = 0;
		unsigned int pickedID = 0;
		BoundingBox* boundingBox;
		GLuint LightID;
		bool scene4loaded = false;
		mwm::Vector3 lightInvDir = mwm::Vector3(-1.f, 1.f, 1.f);
    };
} // namespace Example