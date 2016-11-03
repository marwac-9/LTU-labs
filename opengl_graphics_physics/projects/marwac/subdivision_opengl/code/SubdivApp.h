//
// Created by marwac-9 on 9/16/15.
//
#include "core/app.h"
#include "render/window.h"
#include "MyMathLib.h"

#include <vector>
class BoundingBox;
class Object;
class OBJ;
class Mesh;
class HalfEdgeMesh;
class Camera;

namespace Subdivision
{
	class SubdivisionApp : public Core::App
	{
	public:
		/// constructor
		SubdivisionApp();
		/// destructor
		~SubdivisionApp();

		/// open app
		bool Open();
		/// run app
		void Run();
	private:
		void Clear();
		void ClearSubdivisionData();

		void Draw(const mwm::Matrix4& ProjectionMatrix, const mwm::Matrix4& ViewMatrix);
		void InitGL();
		void ClearBuffers();
		void KeyCallback(int key, int scancode, int action, int mods);
		void MouseCallback(double mouseX, double mouseY);
		void Monitor(Display::Window* window);
		void SetUpCamera(float timeStep);
		void LoadScene1();
		void LoadScene2();
		void LoadScene3();
		void Subdivide(OBJ* objToSubdivide);
		void LoadShaders();
		bool altButtonToggle = true;

		bool running = false;
		bool wireframe = false;
		Display::Window* window;
		double leftMouseX;
		double leftMouseY;
		int windowWidth;
		int windowHeight;
		float windowMidX;
		float windowMidY;

		mwm::Matrix4 ProjectionMatrix;
		mwm::Matrix4 ViewMatrix;
		Camera* currentCamera;

		int objectsRendered = 0;

		GLuint LightID;

		std::vector<OBJ*> dynamicOBJs;
		std::vector<Mesh*> dynamicMeshes;
		std::vector<HalfEdgeMesh*> dynamicHEMeshes;
	};
} // namespace 