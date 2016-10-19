#include "Object.h"
#include "MyMathLib.h"
#include "Mesh.h"

using namespace mwm;

Object::Object()
{

}

Object::~Object()
{
}

void Object::AssignMesh(Mesh* mesh)
{
	this->mesh = mesh;
}