#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "InputHandler.h"
#include "Scene.h"
#include "Object.h"
#include "Node.h"
#include <vector>
#include "GraphicsStorage.h"
#include "GraphicsManager.h"
#include <iostream>

using namespace mwm;

InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}

bool InputHandler::altButtonToggle = true;
int InputHandler::cameraMode = 2;

void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		Scene::Instance()->addRandomObject();
	}
	//this is just used to show hide cursor, mouse steal on/off
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
	{
		if (altButtonToggle)
		{
			altButtonToggle = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			int windowHeight = 0;
			int windowWidth = 0;

			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			altButtonToggle = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			// Reset mouse position for next frame
			glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
		}
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		printf("\nWIREFRAME MODE\n");
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		printf("\nSHADED MODE\n");
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		cameraMode = 1;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		cameraMode = 2;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		cameraMode = 3;
	}
	if (key == GLFW_KEY_S && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && action == GLFW_PRESS)
	{
		GraphicsManager::SaveToOBJ(GraphicsStorage::objects.back());
		std::cout << "Last Mesh Saved" << std::endl;
	}
}

void InputHandler::monitor(GLFWwindow* window, Matrix4& ViewMatrix)
{
	if (InputHandler::cameraMode == 1 || InputHandler::cameraMode == 3)
	{
		ViewMatrix = ViewMatrix * Matrix4::translate(0, 0, 0); // first person and monitoring cam would be 0
	}
	if (InputHandler::cameraMode == 2)
	{
		ViewMatrix = ViewMatrix * Matrix4::translate(0, 0, -10); // third person camera move out first person
	}
}