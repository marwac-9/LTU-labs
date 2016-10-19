#pragma once
#include <vector>
#include "MyMathLib.h"
class Object;

class Node
{
public:
	Node();
	~Node();
	std::vector<Node*> children;
	void addChild(Node* child);
	mwm::Matrix4 TransformationMatrix;
	mwm::Matrix4 TopDownTransform;
	std::string name;
	void UpdateNodeMatrix(const mwm::Matrix4& ParentMatrix);
	mwm::Vector3 position;
	mwm::Vector3 scale;
	mwm::Quaternion orientation;

	mwm::Vector3 prevPos;
	mwm::Quaternion prevOrientation;
private:

};

