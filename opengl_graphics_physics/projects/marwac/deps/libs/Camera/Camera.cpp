#include "cmath"
#include "MyMathLib.h"
#include "Camera.h"

using namespace mwm;

Camera::Camera(const mwm::Vector3& initPos, int windowW, int windowH)
{
	// Initial horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// Initial vertical angle : none
	verticalAngle = 0.0f;
	// Initial Field of View
	initialFoV = 45.0f;

	speed = 10.0f; // 3 units / second
	mouseSpeed = 0.002f;

	holdingForward = false;
	holdingBackward = false;
	holdingRight = false;
	holdingLeft = false;
	holdingUp = false;
	holdingDown = false;

	initialPosition = initPos;

	UpdateSize(windowW, windowH);
	UpdateOrientation(windowMidX, windowMidY + 100);
}

Camera::~Camera()
{
}

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

Vector3 Camera::GetInitPos()
{
    return initialPosition;
}

Vector3 Camera::GetPosition() //no scaling of view
{
	Vector3 pos = ViewMatrix.getPosition();
	Matrix3 rotMat = ~(ViewMatrix.ConvertToMatrix3()); //need to transpose for row major

	Vector3 retVec = rotMat * (-1 * pos);
	return retVec;
}

Vector3 Camera::GetPosition2() //if scaled view
{
	Matrix4 viewModel = ViewMatrix.inverse();
	Vector3 cameraPos = viewModel.getPosition();
	return cameraPos;
}

void Camera::Update(float deltaTime){

	UpdatePosition(deltaTime);

	CalculateViewMatrix();		
}

void Camera::UpdateOrientation(double mouseX, double mouseY)
{
	// Compute new orientation

	horizontalAngle += mouseSpeed * (windowMidX - (float)mouseX);
	verticalAngle += mouseSpeed * (windowMidY - (float)mouseY);

	//if monitoring camera
	/*
	if (cameraType == 3)
	{
		verticalAngle = 0.0f;
		horizontalAngle = 3.14f;
	}
	*/

	ComputeVectors();
}

void Camera::UpdatePosition(float deltaTime)
{
	// Move forward
	if (holdingForward){
		initialPosition = initialPosition + (direction * deltaTime * speed);
	}
	// Move backward
	if (holdingBackward){
		initialPosition = initialPosition - (direction * deltaTime * speed);
	}
	// Strafe right
	if (holdingRight){
		initialPosition = initialPosition + (right * deltaTime * speed);
	}
	// Strafe left
	if (holdingLeft){
		initialPosition = initialPosition - (right * deltaTime * speed);
	}
	if (holdingUp){
		initialPosition = initialPosition + (up * deltaTime * speed);
	}
	if (holdingDown){
		initialPosition = initialPosition - (up * deltaTime * speed);
	}
}

void Camera::UpdateSize(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	windowMidX = windowWidth / 2.0f;
	windowMidY = windowHeight / 2.0f;
}

void Camera::ComputeVectors()
{
	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = Vector3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	right = Vector3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0.f,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector
	up = right.crossProd(direction);
}

void Camera::CalculateViewMatrix()
{
	// Camera matrix
	//if (cameraType == 1 || cameraType == 2)
	//{
		ViewMatrix = Matrix4::lookAt(
			initialPosition,           // Camera is here
			initialPosition + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
		/*if (cameraType == 2)
		{
			ViewMatrix = ViewMatrix*Matrix4::translate(0, 0, -10);// we move the camera out after creating the look at (rotation matrix)
		}
	}
	if (cameraType == 3)
	{
		ViewMatrix = Matrix4::lookAt(
			Vector3(0.f, 0.f, 6.f),           // Camera is here
			initialPosition + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
	}*/
}
