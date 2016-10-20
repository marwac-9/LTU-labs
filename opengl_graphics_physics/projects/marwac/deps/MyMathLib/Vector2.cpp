#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
namespace mwm
{
Vector2::Vector2(float x, float y)
{
	vect[0] = x;
	vect[1] = y;
}

Vector2::Vector2(const Vector2& vec)
{
	vect[0] = vec.x;
	vect[1] = vec.y;
}

Vector2::Vector2(double x, double y)
{
	vect[0] = (float)x;
	vect[1] = (float)y;
}

Vector2::~Vector2()
{
}

/*! \fn add vectors amd return new one*/
Vector2 Vector2::operator+ (const Vector2& rightVector) const
{
	return Vector2(this->vect[0] + rightVector.vect[0], this->vect[1] + rightVector.vect[1]);
}

/*! \fn substract vectors and return new one*/
Vector2 Vector2::operator- (const Vector2& rightVector) const
{

	return Vector2(this->vect[0] - rightVector.vect[0], this->vect[1] - rightVector.vect[1]);
}
/*! \fn dot product returns scalar*/
float Vector2::dotAKAscalar(const Vector2& rightVector) const
{
	float x = this->vect[0] * rightVector.vect[0];
	float y = this->vect[1] * rightVector.vect[1];
	return x + y;
}

/*! \fn function returning length of instanced vector*/
float Vector2::vectLengt() const
{
	float x = this->vect[0];
	float y = this->vect[1];
	return sqrt(x*x + y*y);
}
/*! \fn function returning new normalized vector*/
Vector2 Vector2::vectNormalize() const
{
	float vLength = this->vectLengt();
	float x = this->vect[0] / vLength;
	float y = this->vect[1] / vLength;
	return Vector2(x, y);
}

/*! \fn function to convert 2D vector to 3D vector*/
Vector3 Vector2::vec2TOvec3(Vector2 _2Dvector, float z)
{
	return Vector3(_2Dvector.vect[0], _2Dvector.vect[1], z);
}

/*! \fn vector*num returns new matrix*/
Vector2 Vector2::operator* (const float& rightFloat) const
{
	float u = this->vect[0] * rightFloat;
	float v = this->vect[1] * rightFloat;
	return Vector2(u, v);
}

/*! \fn num*vector returns new matrix*/
Vector2 operator* (const float& leftFloat, const Vector2& rightVector)
{
	float u = rightVector.vect[0] * leftFloat;
	float v = rightVector.vect[1] * leftFloat;
	return Vector2(u, v);
}

/*! \fn vector/num returns new matrix*/
Vector2 Vector2::operator/ (const float& rightFloat) const
{
	float u = this->vect[0] / rightFloat;
	float v = this->vect[1] / rightFloat;
	return Vector2(u, v);
}
}


