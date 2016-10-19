#pragma once
#include <vector>
#include "MyMathLib.h"

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

private:

};

