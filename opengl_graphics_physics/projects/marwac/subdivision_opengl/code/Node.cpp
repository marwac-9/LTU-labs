#include "Node.h"
#include "Object.h"
using namespace mwm;
Node::Node()
{
	this->TransformationMatrix = Matrix4::identityMatrix();
	this->TopDownTransform = Matrix4::identityMatrix();
	this->scale = Vector3(1.f, 1.f, 1.f);
	this->position = Vector3(0.f, 0.f, 0.f);
	this->orientation = Quaternion(0, Vector3(1.f, 1.f, 1.f));
}

Node::~Node()
{
}

void Node::UpdateNodeMatrix(const Matrix4& ParentMatrix)
{
	this->TransformationMatrix = Matrix4::scale(this->scale) * this->orientation.ConvertToMatrix() * Matrix4::translate(this->position);
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