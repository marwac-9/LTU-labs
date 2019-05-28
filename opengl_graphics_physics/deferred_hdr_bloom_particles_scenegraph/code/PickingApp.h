//
// Created by marwac-9 on 9/16/15.
//
#include "app.h"
#include "gl_window.h"
#include "MyMathLib.h"
#include <vector>
#include "LightProperties.h"

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
class InstanceSystem;

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

        void PickingTest();

		void PIDController();
		void BlitDepthToScreenPass();
		void BlitToScreenPass();
		void FireLightProjectile();
		void DrawGeometryMaps(int width, int height);

		void Screenshot();
		void Screenshot2();

		void GenerateGUI();

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
		void LoadScene7();
		void LoadScene8();
		void LoadScene9();
		void LoadScene0();
		void Vortex();

		void DrawParticles();
		void SpawnSomeLights();
		bool applicationInputEnabled = true;
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
		float xAngled = 45.f;
		float yAngled = 310.f;
		float xAngled2 = 45.f;
		float yAngled2 = 74.6f;
		float xAngled3 = 213.f;
		float yAngled3 = 155.6f;
		float xAngled4 = 207.f;
		float yAngled4 = 226.6f;
		std::vector<ParticleSystem*> particleSystems;
		int increment = 0;
		int prevGridPos[3];
		DirectionalLight* directionalLightComp;
		DirectionalLight* directionalLightComp2;
		DirectionalLight* directionalLightComp3;
		DirectionalLight* directionalLightComp4;
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

		bool post = true;
		float blurBloomSize = 0.3f;
		int bloomLevel = 1;

		int instancedGeometryDrawn = 0;
		float cubeShininess = 10.f;
		float cubeSpecularIntensity = 0.25f;

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
		Object* testCube1;
		Object* testPyramid;
		Object* testSphere1;
		Object* firstObject = nullptr;
		Object* secondObject = nullptr;

		FrameBuffer* captureFBO;
		unsigned int captureRBO;
		Texture* irradianceCubeMap;
		Texture* prefilteredHDRMap;
		Texture* envCubeMap;
		Texture* brdfTexture;
		std::vector<Texture*> pbrEnvTextures;
	};
} // namespace Example