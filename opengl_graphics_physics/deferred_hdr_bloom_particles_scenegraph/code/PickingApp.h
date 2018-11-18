//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include "gl_window.h"
#include "MyMathLib.h"
#include <vector>
#include "Attenuation.h"

class ParticleSystem;
class LineSystem;
class BoundingBoxSystem;
class BoundingBox;
class Object;
class Camera;
class Node;
class FastLine;
class PointSystem; 
class FrameBuffer;
class Texture;
class DirectionalLight;
class SpotLight;
class PointLight;

enum loadedScene
{
	none,
	scene0Loaded,
	scene1Loaded,
	scene2Loaded,
	scene3Loaded,
	scene4Loaded,
	scene5Loaded,
	scene6Loaded,
	scene7Loaded,
	scene8Loaded,
	scene9Loaded
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

		void DrawDebugInstanced();
        void PickingTest();

		void DrawGeometryPass();
		void PIDController();
		void DrawLightPass();
		void BlitDepthToScreenPass();
		void BlitToScreenPass();
		void FireLightProjectile();
		void DrawGeometryMaps(int width, int height);

		void DrawHDR(Texture* texture);

		void GenerateGUI();
		void DrawGSkybox();
		void DrawSkybox();

        void InitGL();
        void ClearBuffers();
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
		void LoadScene7();
		void LoadScene8();
		void LoadScene9();
		void LoadScene0();
		void Vortex();

		void DrawParticles();
		void SpawnSomeLights();
		bool altButtonToggle = true;
        //camera type 1 == fps, 2 == tps 3 == monitoring camera
        int cameraMode = 1;
		Camera* currentCamera;
        bool running = false;
		bool drawBB = false;
		bool pausedPhysics = false;
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
		bool drawLines = false;
		bool drawPoints = false;
		bool drawParticles = true;
		bool drawMaps = true;
        Object* lastPickedObject = nullptr;
		int objectsRendered = 0;
		int lightsRendered = 0;
		int particlesRendered = 0;
		unsigned int pickedID = 0;

		loadedScene currentScene = none;
		float planeDir = -1;
		bool minimized = false;
		double updateTime = 0.0;
		double renderTime = 0.0;
		float near = 0.1f;
		float far = 2000.f;
		float fov = 45.0f;
		float xAngled = 108.f;
		float yAngled = 162.f;
		float xAngled2 = 108.f;
		float yAngled2 = 282.6f;
		std::vector<ParticleSystem*> particleSystems;
		void DrawFastLineSystems();
		void DrawFastPointSystems();
		void GenerateFastLines();
		void GenerateAndDrawFastLineChildren(Node* parentPos, Node* child);
		int increment = 0;
		int prevGridPos[3];

		DirectionalLight* directionalLightComp;
		DirectionalLight* directionalLightComp2;
		SpotLight* spotLightComp;

		Object* spotLight1;
		float xAngles = 178.f;
		float yAngles = 200.f;
		float posX = 10.f;
		float posY = 10.f;
		float posZ = 0.f;

		Object* qXCube1;
		Object* qYCube1;
		Object* qZCube1;

		Object* qXCube2;
		Object* qYCube2;
		Object* qZCube2;
		float xCubeAngle = 0;
		float yCubeAngle = 0;
		float zCubeAngle = 0;

		Object* pointLightTest;
		PointLight* pointLightCompTest;
		float pointScale = 100.f;
		float pposX = 0.f;
		float pposY = 10.f;
		float pposZ = 0.f;

		float spotLightCutOff = 12.5;
		float spotLightOuterCutOff = 17.5;
		float spotSZ = 50;

		float softScale = 0.5f;
		float contrastPower = 0.5f;

		Object* skybox = nullptr;
		bool post = true;
		float blurBloomSize = 1.5f;
		int bloomLevel = 3;

		Texture* diffuseTexture;
		Texture* normalTexture;
		Texture* metDiffIntShinSpecIntTexture;
		
		FrameBuffer* lightAndPostBuffer;
		Texture* finalColorTexture;
		Texture* brightLightTexture;
		Texture* blurredBrightTexture;
		
		FrameBuffer* geometryBuffer;
		Texture* pickingTexture;
		Texture* worldPosTexture;
		Texture* depthTexture;
		Attenuation lightAttenuation;
		
	};
} // namespace Example