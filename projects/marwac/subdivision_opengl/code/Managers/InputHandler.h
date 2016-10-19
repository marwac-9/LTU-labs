#pragma once

namespace mwm
{
	class Matrix4;
}

class InputHandler
{
public:
	InputHandler();
	~InputHandler();
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);//the variable that controlls mouse steal
	static void monitor(GLFWwindow* window, mwm::Matrix4& ViewMatrix);
	static bool altButtonToggle;
	//camera type 1 == fps, 2 == tps 3 == monitoring camera
	static int cameraMode;
private:

};

