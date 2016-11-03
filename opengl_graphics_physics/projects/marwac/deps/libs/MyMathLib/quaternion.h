#pragma once
namespace mwm
{
class Vector3;
class Matrix4;
class Matrix3;

class Quaternion
{

public:

	union
	{
		float quaternion[4];
		struct{ float x, y, z, w; };
	};

	Quaternion();
	/// Constructor to set up a quaternion
	Quaternion(float angle, const Vector3 &axis);
	Quaternion(const float w, const float x, const float y, const float z);
	~Quaternion(void);

	/// Set x, y, z, w values of a Vector. W defaults to 1.
	void Insert(float x, float y, float z, float w = 1.f);
	/// Set a value of a component of a vector
	void InsertAt(unsigned int index, float value);

	Quaternion operator*(Quaternion& v) const;
	void operator*=(Quaternion& v);
	Quaternion operator*(const float& number)const;

	float Magnitude();
	/// Normalize this vector
	void Normalize();
	Quaternion Normalized();
	Matrix4 ConvertToMatrix();
	Matrix3 ConvertToMatrix3();

	/// Overloaded access operator for Vector
	float& operator[](unsigned int index);

	void addScaledVector(const Vector3& vector, float scale);
};
}

