#include <math.h>
#include "quaternion.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Matrix3.h"

namespace mwm
{
Quaternion::Quaternion()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
}

Quaternion::Quaternion(float angle, const Vector3 &axis)
{
	this->x = axis.x * sinf((angle) / 2.f);
	this->y = axis.y * sinf((angle) / 2.f);
	this->z = axis.z * sinf((angle) / 2.f);
	this->w = cosf((angle) / 2.f);
}

Quaternion::Quaternion(const float w, const float x, const float y, const float z) : w(w), x(x), y(y), z(z)
{}

Quaternion::~Quaternion(void)
{
}

void Quaternion::Insert(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Quaternion::InsertAt(unsigned int index, float value)
{
	this->quaternion[index] = value;
}

Quaternion Quaternion::operator*(Quaternion& v) const
{
	Quaternion result;

	result.x = w*v.x + x*v.w + y*v.z - z*v.y; //x
	result.y = w*v.y - x*v.z + y*v.w + z*v.x; //y
	result.z = w*v.z + x*v.y - y*v.x + z*v.w; //z
	result.w = w*v.w - x*v.x - y*v.y - z*v.z; //w

	return result;
}

Quaternion Quaternion::operator*(const float& number) const
{
	Quaternion result;
	for (int i = 0; i < 4; i++){
		result[i] = this->quaternion[i] * number;
	}
	return result;
}

void Quaternion::operator*=(Quaternion& v)
{
	this->x = this->w*v.x + this->x*v.w + this->y*v.z - this->z*v.y; //x
	this->y = this->w*v.y - this->x*v.z + this->y*v.w + this->z*v.x; //y
	this->z = this->w*v.z + this->x*v.y - this->y*v.x + this->z*v.w; //z
	this->w = this->w*v.w - this->x*v.x - this->y*v.y - this->z*v.z; //w
}

float Quaternion::Magnitude()
{
	return sqrt(x*x + y*y + z*z + w*w);
}

void Quaternion::Normalize()
{
	float length = Magnitude();
	for (int i = 0; i < 4; i++)
	{
		this->quaternion[i] = this->quaternion[i] / length;
	}
}

Quaternion Quaternion::Normalized()
{
	Quaternion result;

	float length = sqrt(x*x + y*y + z*z + w*w);

	for (int i = 0; i < 4; i++)
	{
		result[i] = this->quaternion[i] / length;
	}

	return result;
}

Matrix4 Quaternion::ConvertToMatrix()
{
	Matrix4 rotation;
	rotation[0][0] = 1 - 2 * y*y - 2 * z*z;
	rotation[1][0] = (2 * x * y) - (2 * w * z);
	rotation[2][0] = (2 * x * z) + (2 * w * y);
	rotation[3][0] = 0;

	rotation[0][1] = (2 * x * y) + (2 * w * z);
	rotation[1][1] = 1 - 2 * x*x - 2 * z*z;
	rotation[2][1] = (2 * y * z) - (2 * w * x);
	rotation[3][1] = 0;

	rotation[0][2] = (2 * x * z) - (2 * w * y);
	rotation[1][2] = (2 * y * z) + (2 * w * x);
	rotation[2][2] = 1 - 2 * x*x - 2 * y*y;
	rotation[3][2] = 0;

	rotation[0][3] = 0;
	rotation[1][3] = 0;
	rotation[2][3] = 0;
	rotation[3][3] = 1;

	return rotation;
}

float& Quaternion::operator[](unsigned int index)
{
	return this->quaternion[index];
}

void Quaternion::addScaledVector(const Vector3& vector, float scale)
{
	Quaternion q(0, vector.x * scale, vector.y * scale, vector.z * scale);

	q *= *this;

	w += q.w * 0.5f;
	x += q.x * 0.5f;
	y += q.y * 0.5f;
	z += q.z * 0.5f;
}

Matrix3 Quaternion::ConvertToMatrix3()
{
	Matrix3 rotation;
	rotation[0][0] = 1 - 2 * y*y - 2 * z*z;
	rotation[1][0] = (2 * x * y) - (2 * w * z);
	rotation[2][0] = (2 * x * z) + (2 * w * y);

	rotation[0][1] = (2 * x * y) + (2 * w * z);
	rotation[1][1] = 1 - 2 * x*x - 2 * z*z;
	rotation[2][1] = (2 * y * z) - (2 * w * x);

	rotation[0][2] = (2 * x * z) - (2 * w * y);
	rotation[1][2] = (2 * y * z) + (2 * w * x);
	rotation[2][2] = 1 - 2 * x*x - 2 * y*y;

	return rotation;
}

}
