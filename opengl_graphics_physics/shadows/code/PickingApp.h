//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"

class BoundingBox;
class Object;
class Camera;
class FrameBuffer;
class Texture;

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
		void DrawPicking();
		void DrawDebug();
		void DrawDepth(const mwm::Matrix4& ViewProjectionMatrix);
        void PickingTest();

		void GenerateGUI();
		
		void DrawDepthPass();
		void DrawMaps(int width, int height);
        void InitGL();
		void SetUpBuffers(int windowWidth, int windowHeight);
        void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera();
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void LoadScene4();
		void LoadScene5();
		void LoadScene6();

		void Vortex();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 2;
		bool minimized = false;
        bool running = false;
		bool debug = true;
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

		bool scene4loaded = false;
		mwm::Vector3F lightInvDir = mwm::Vector3F(-1.f, 1.f, 1.f);
		float xAngle = 108.f;
		float yAngle = 168.f;
		float orthoSize = 150.f;
		float oleft = -orthoSize, oright = orthoSize, obottom = -orthoSize, otop = orthoSize, onear = -orthoSize, ofar = orthoSize;
		float near = 0.1f;
		float far = 2000.f;
		float fov = 45.0f;
		bool blurShadowMap = true;
		int shadowMapBlurLevel = 1;
		float blurShadowMapSize = 1.5f;
		float shadowFadeRange = 10.f;

		Object* directionalLightObject;

		mwm::Matrix4F biasedDirLightViewProjection;
		Texture* blurredShadowTexture;
		mwm::Matrix4 lightViewMatrixTest;
		FrameBuffer* frameBuffer;
		Texture* pickingTexture;
		Texture* worldPosTexture;
		Texture* shadowTexture;
    };
} // namespace Example
