#include <cmath>
#include "Vector2.h"
#include "Vector3.h"

namespace mwm
{
Vector3::Vector3(float x, float y, float z)
{
	vect[0] = x;
	vect[1] = y;
	vect[2] = z;
}

Vector3::Vector3(const Vector2& vec, float z)
{
	vect[0] = vec.x;
	vect[1] = vec.y;
	vect[2] = z;
}

Vector3::Vector3(double x, double y, double z)
{
	vect[0] = (float)x;
	vect[1] = (float)y;
	vect[2] = (float)z;
}

Vector3::~Vector3()
{
}

/*! \fn add vectors amd return new one*/
Vector3 Vector3::operator+ (const Vector3& rightVector) const
{
	return Vector3(this->vect[0] + rightVector.vect[0], this->vect[1] + rightVector.vect[1], this->vect[2] + rightVector.vect[2]);
}

/*! \fn substract vectors and return new one*/
Vector3 Vector3::operator- (const Vector3& rightVector) const
{

	return Vector3(this->vect[0] - rightVector.vect[0], this->vect[1] - rightVector.vect[1], this->vect[2] - rightVector.vect[2]);
}
/*! \fn dot product returns scalar*/
float Vector3::dotAKAscalar(const Vector3& rightVector) const
{
	float x1 = this->x * rightVector.x;
	float y1 = this->y * rightVector.y;
	float z1 = this->z * rightVector.z;
	return x1 + y1 + z1;
}

/*! \fn function returning length of instanced vector*/
float Vector3::vectLengt() const
{
	float x = this->vect[0];
	float y = this->vect[1];
	float z = this->vect[2];
	return sqrt(x*x + y*y + z*z);
}

float Vector3::squareMag() const
{
	float x = this->vect[0];
	float y = this->vect[1];
	float z = this->vect[2];
	return (x*x + y*y + z*z);
}

/*! \fn function returning new normalized vector*/
Vector3 Vector3::vectNormalize() const
{
	float vLength = this->vectLengt();
	float x = this->vect[0] / vLength;
	float y = this->vect[1] / vLength;
	float z = this->vect[2] / vLength;
	return Vector3(x, y, z);
}
/*! \fn cross product function returning normal vector*/
Vector3 Vector3::crossProd(const Vector3& rightVector) const
{
	float x = +((this->vect[1] * rightVector.vect[2]) - (rightVector.vect[1] * this->vect[2]));
	float y = -((this->vect[0] * rightVector.vect[2]) - (rightVector.vect[0] * this->vect[2]));
	float z = +((this->vect[0] * rightVector.vect[1]) - (rightVector.vect[0] * this->vect[1]));
	return Vector3(x, y, z);
}

/*! \fn function to convert 3D vector to 4D vector*/
Vector4 Vector3::vec3TOvec4(const Vector3& _3Dvector, float w)
{
	return Vector4(_3Dvector.vect[0], _3Dvector.vect[1], _3Dvector.vect[2], w);
}

/*! \fn vector*num returns new matrix*/
Vector3 Vector3::operator* (const float& rightFloat) const
{
	float x = this->vect[0] * rightFloat;
	float y = this->vect[1] * rightFloat;
	float z = this->vect[2] * rightFloat;
	return Vector3(x, y, z);
}

Vector3 Vector3::operator*(const Vector3& rightVector) const
{
	return Vector3(x*rightVector.x, y*rightVector.y, z*rightVector.z);
}

/*! \fn num*vector returns new matrix*/
Vector3 operator* (const float& leftFloat, const Vector3& rightVector)
{
	float x = rightVector.vect[0] * leftFloat;
	float y = rightVector.vect[1] * leftFloat;
	float z = rightVector.vect[2] * leftFloat;
	return Vector3(x, y, z);
}

/*! \fn vector/num returns new matrix*/
Vector3 Vector3::operator/ (const float& rightFloat) const
{
	float x = this->vect[0] / rightFloat;
	float y = this->vect[1] / rightFloat;
	float z = this->vect[2] / rightFloat;
	return Vector3(x, y, z);
}

void Vector3::operator+=(const Vector3& v)
{
	for (int i = 0; i < 3; i++)
	{
		vect[i] += v.vect[i];
	}
}

void Vector3::operator-=(const Vector3& v)
{
	for (int i = 0; i < 3; i++)
	{
		vect[i] -= v.vect[i];
	}
}

void Vector3::operator*=(const float& number)
{
	for (int i = 0; i < 3; i++)
	{
		vect[i] *= number;
	}
}

void Vector3::operator/=(const float& number)
{
	Vector3 temp = *this;
	for (int i = 0; i < 3; i++)
	{
		vect[i] /= number;
	}
}

bool Vector3::operator==(const Vector3& v)const
{
	for (int i = 0; i < 3; i++)
	{
		if (vect[i] != v.vect[i]){
			return false;
		}
	}
	return true;
}

/*! \fn operator[] overload for indexing */
float& Vector3::operator[] (int index)
{
	return this->vect[index];
}
}
