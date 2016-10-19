#pragma once
#include <emmintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>
namespace mwm
{
class Vector2;
class Vector3;
/*! class for constructing 4D vector*/
class Vector4
{
public:
	union
	{
		struct{ float x, y, z, w; };
		float vect[4];
		__m128 v;
	};


	Vector4(float x = 0, float y = 0, float z = 0, float w = 0);
	Vector4(double x, double y, double z, double w);
	Vector4(const Vector2& v1, const Vector2& v2);
	Vector4(const Vector3& v1, float w = 0);
	~Vector4();
	Vector4 operator+ (const Vector4& rightVector) const; //!< add vectors amd return new one
	Vector4 operator- (const Vector4& rightVector) const; //!< substract vectors and return new one
	float dotAKAscalar(const Vector4& rightVector) const; //!< dot product returns scalar
	float vectLengt() const; //!< function returning length of instanced vector
	float squareMag() const;
	Vector4 vectNormalize() const; //!< function returning new normalized vector

	Vector4 operator* (const float& rightFloat) const; //!< vector*num returns new matrix
	friend Vector4 operator* (const float& leftFloat, const Vector4& rightVector); //!< vector*matrix returns new matrix
	Vector4 operator/ (const float& rightFloat) const; //!< vector/num returns new matrix

	Vector3 get_xyz() const;
	float& operator[] (int index); //!< operator[] overload for indexing

protected:
};
}


