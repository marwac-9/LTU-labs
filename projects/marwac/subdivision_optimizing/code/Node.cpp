#include "Node.h"

using namespace mwm;

Node::Node()
{
	this->TransformationMatrix = Matrix4::identityMatrix();
	this->TopDownTransform = Matrix4::identityMatrix();
}

Node::~Node()
{
}

void Node::UpdateNodeMatrix(const Matrix4& ParentMatrix)
{
	//Matrix4 copy = ParentMatrix;
	this->TopDownTransform = this->TransformationMatrix*ParentMatrix;
	for (size_t i = 0; i < children.size(); i++)
	{
		children.at(i)->UpdateNodeMatrix(TopDownTransform);
	}
}

void Node::addChild(Node* child)
{
	this->children.push_back(child);
}