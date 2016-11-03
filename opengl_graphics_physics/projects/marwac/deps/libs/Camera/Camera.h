#pragma once

class Camera
{
public:
	Camera(const mwm::Vector3& initPos, int windowWidth, int windowHeight);
	~Camera();
	void Update(float deltaTime);

	void CalculateViewMatrix();

	void ComputeVectors();

	void UpdatePosition(float deltaTime);

	void UpdateOrientation(double mouseX, double mouseY);

	void UpdateSize(int width, int height);

	mwm::Matrix4 getViewMatrix();
	mwm::Matrix4 getProjectionMatrix();
	mwm::Vector3 getDirection();
	mwm::Vector3 getUp();

	mwm::Vector3 GetInitPos();
	mwm::Vector3 GetPosition();
	mwm::Vector3 GetPosition2();

	bool holdingForward;
	bool holdingBackward;
	bool holdingRight;
	bool holdingLeft;
	bool holdingUp;
	bool holdingDown;

	
private:
	mwm::Matrix4 ViewMatrix;
	mwm::Vector3 direction;
	mwm::Vector3 up;
	mwm::Vector3 right;
	mwm::Vector3 initialPosition;

	int windowWidth;
	int windowHeight;
	float windowMidX;
	float windowMidY;

	// Initial horizontal angle : toward -Z
	float horizontalAngle;
	// Initial vertical angle : none
	float verticalAngle;
	// Initial Field of View
	float initialFoV;

	float speed;
	float mouseSpeed;
};
