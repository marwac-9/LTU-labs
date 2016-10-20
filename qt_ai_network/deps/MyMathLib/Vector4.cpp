#include <cmath>
#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
namespace mwm
{
/*! \fn Vector4 constructor*/
Vector4::Vector4(float x, float y, float z, float w)
{
	vect[0] = x;
	vect[1] = y;
	vect[2] = z;
	vect[3] = w;
}

Vector4::Vector4(const Vector2& v1, const Vector2& v2)
{
	vect[0] = v1.x;
	vect[1] = v1.y;
	vect[2] = v2.x;
	vect[3] = v2.y;
}

Vector4::Vector4(const Vector3& v1, float w)
{
	vect[0] = v1.x;
	vect[1] = v1.y;
	vect[2] = v1.z;
	vect[3] = w;
}

Vector4::Vector4(double x /*= 0*/, double y /*= 0*/, double z /*= 0*/, double w /*= 0*/)
{
	vect[0] = (float)x;
	vect[1] = (float)y;
	vect[2] = (float)z;
	vect[3] = (float)w;
}

Vector4::~Vector4()
{
}

/*! \fn add vectors amd return new one*/
Vector4 Vector4::operator+ (const Vector4& rightVector) const
{
	return Vector4(this->vect[0] + rightVector.vect[0], this->vect[1] + rightVector.vect[1], this->vect[2] + rightVector.vect[2], this->vect[3] + rightVector.vect[3]);
}

/*! \fn substract vectors and return new one*/
Vector4 Vector4::operator- (const Vector4& rightVector) const
{

	return Vector4(this->vect[0] - rightVector.vect[0], this->vect[1] - rightVector.vect[1], this->vect[2] - rightVector.vect[2], this->vect[3] - rightVector.vect[3]);
}

/*! \fn dot product returns scalar*/
float Vector4::dotAKAscalar(const Vector4& rightVector) const
{
	float x = this->vect[0] * rightVector.vect[0];
	float y = this->vect[1] * rightVector.vect[1];
	float z = this->vect[2] * rightVector.vect[2];
	float w = this->vect[3] * rightVector.vect[3];
	return x + y + z + w;
}

/*! \fn function returning length of instanced vector*/
float Vector4::vectLengt() const
{
	float x = this->vect[0];
	float y = this->vect[1];
	float z = this->vect[2];
	float w = this->vect[3];
	return sqrt(x*x + y*y + z*z + w*w);
}

float Vector4::squareMag() const
{
	float x = this->vect[0];
	float y = this->vect[1];
	float z = this->vect[2];
	float w = this->vect[3];
	return (x*x + y*y + z*z + w*w);
}

/*! \fn vector*num returns new matrix*/
Vector4 Vector4::operator* (const float& rightFloat) const
{
	float x = this->vect[0] * rightFloat;
	float y = this->vect[1] * rightFloat;
	float z = this->vect[2] * rightFloat;
	float w = this->vect[3] * rightFloat;
	return Vector4(x, y, z, w);
}

/*! \fn num*vector returns new matrix*/
Vector4 operator* (const float& leftFloat, const Vector4& rightVector)
{
	float x = rightVector.vect[0] * leftFloat;
	float y = rightVector.vect[1] * leftFloat;
	float z = rightVector.vect[2] * leftFloat;
	float w = rightVector.vect[3] * leftFloat;
	return Vector4(x, y, z, w);
}

/*! \fn vector/num returns new matrix*/
Vector4 Vector4::operator/ (const float& rightFloat) const
{
	float x = this->vect[0] / rightFloat;
	float y = this->vect[1] / rightFloat;
	float z = this->vect[2] / rightFloat;
	float w = this->vect[3] / rightFloat;
	return Vector4(x, y, z, w);
}

Vector3 Vector4::get_xyz() const
{
	return Vector3(this->vect[0], this->vect[1], this->vect[2]);
}

/*! \fn function returning new normalized vector*/
Vector4 Vector4::vectNormalize() const
{
	float vLength = this->vectLengt();
	float x = this->vect[0] / vLength;
	float y = this->vect[1] / vLength;
	float z = this->vect[2] / vLength;
	float w = this->vect[3] / vLength;
	return Vector4(x, y, z, w);
}

/*! \fn operator[] overload for indexing */
float& Vector4::operator[] (int index)
{
	return this->vect[index];
}
}