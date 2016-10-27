#pragma once
class Window;

class Camera
{
public:
	Camera();
	~Camera();
	void computeViewFromInput(Display::Window* window, int cameraType, double deltaTime);
	mwm::Matrix4 getViewMatrix();
	mwm::Matrix4 getProjectionMatrix();
	mwm::Vector3 getDirection();
	mwm::Vector3 getUp();

	// Initial horizontal angle : toward -Z
	float horizontalAngle;
	// Initial vertical angle : none
	float verticalAngle;
	// Initial Field of View
	float initialFoV;

	float speed; 
	float mouseSpeed;
	
	void SetInitPos(const mwm::Vector3& pos);
	mwm::Vector3 GetInitPos();
	mwm::Vector3 GetPosition();
	mwm::Vector3 GetPosition2();
private:
	mwm::Vector3 initialPosition;
};
