// Include GLFW

#include "config.h"
#include "render/window.h"
#include "cmath"
#include <GLFW/glfw3.h>
#include "MyMathLib.h"
#include "Camera.h"

using namespace mwm;

Camera::Camera()
{
	// Initial horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// Initial vertical angle : none
	verticalAngle = 0.0f;
	// Initial Field of View
	initialFoV = 45.0f;

	speed = 10.0f; // 3 units / second
	mouseSpeed = 0.002f;
}

Camera::~Camera()
{
}

Matrix4 ViewMatrix;
Matrix4 ViewMatrixInverse;
Vector3 direction;
Vector3 up;

Matrix4 Camera::getViewMatrix(){
	return ViewMatrix;
}

Vector3 Camera::getDirection()
{
	return direction;
}

Vector3 Camera::getUp()
{
	return up;
}

void Camera::SetInitPos(const Vector3& pos)
{
	initialPosition = pos;
}

Vector3 Camera::GetInitPos()
{
	return initialPosition;
}

Vector3 Camera::GetPosition() //no scaling of view
{
	Vector3 pos = ViewMatrix.getPosition();
	Matrix3 rotMat = ~(ViewMatrix.ConvertToMatrix3()); //need to transpose for row major

	Vector3 retVec = rotMat * (-1.f * pos);
	return retVec;
}

Vector3 Camera::GetPosition2() //if scaled view, most safe to use
{
	Matrix4 viewModel = ViewMatrix.inverse();
	Vector3 cameraPos = viewModel.getPosition();
	return cameraPos;
}

void Camera::computeViewFromInput(Display::Window* window, int cameraType, double deltatime){

	float deltaTime = (float)deltatime;
	// glfwGetTime is called only once, the first time this function is called
	//static double lastTime = glfwGetTime();
	int windowWidth = 0;
	int windowHeight = 0;
	window->GetWindowSize(&windowWidth, &windowHeight);
	// Compute time difference between current and last frame

	//deltaTime = 0.16f;

	// Get mouse position
	double xpos, ypos = 0;

	window->GetCursorPos(&xpos, &ypos);

	// Reset mouse position for next frame
	window->SetCursorPos((float)windowWidth / 2.f, (float)windowHeight / 2.f);

	// Compute new orientation

	horizontalAngle += mouseSpeed * float((float)windowWidth / 2.0f - (float)xpos);
	verticalAngle += mouseSpeed * float((float)windowHeight / 2.0f - (float)ypos);

	//if monitoring camera
	if (cameraType == 3)
	{
		verticalAngle = 0.0f;
		horizontalAngle = 3.14f;
	}


	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = Vector3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	Vector3 right = Vector3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0.f,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	up = right.crossProd(direction);
	// Move forward
	if (window->GetKey(GLFW_KEY_W) == GLFW_PRESS){
		initialPosition = initialPosition + (direction * deltaTime * speed);
	}
	// Move backward
	if (window->GetKey(GLFW_KEY_S) == GLFW_PRESS){
		initialPosition = initialPosition - (direction * deltaTime * speed);
	}
	// Strafe right
	if (window->GetKey(GLFW_KEY_D) == GLFW_PRESS){
		initialPosition = initialPosition + (right * deltaTime * speed);
	}
	// Strafe left
	if (window->GetKey(GLFW_KEY_A) == GLFW_PRESS){
		initialPosition = initialPosition - (right * deltaTime * speed);
	}
	if (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS){
		initialPosition = initialPosition + (up * deltaTime * speed);
	}
	if (window->GetKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		initialPosition = initialPosition - (up * deltaTime * speed);
	}

	//float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Camera matrix
	if (cameraType == 1 || cameraType == 2)
	{
		ViewMatrix = Matrix4::lookAt(
			initialPosition,           // Camera is here
			initialPosition + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
		if (cameraType == 2)
		{
			ViewMatrix = ViewMatrix*Matrix4::translate(0.f, 0.f, -10.f);// we move the camera out after creating the look at (rotation matrix) TPS orbiting camera
		}
	}
	if (cameraType == 3)
	{
		ViewMatrix = Matrix4::lookAt(
			Vector3(0.f, 0.f, 6.f),           // Camera is here
			initialPosition + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
	}

}