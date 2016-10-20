#pragma once
#include <emmintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "Vector4.h"
namespace mwm
{
class Vector4;
class Vector2;
struct loc;
/*! class for constructing 3D vector*/
class Vector3
{
public:

	union
	{
		struct{ float x, y, z; }; //for easy access
		struct{ signed a, b, c; }; //for hashing a^b^c
		float vect[3];
	};
	Vector3(float x = 0, float y = 0, float z = 0);
	Vector3(double x, double y, double z);
	Vector3(const Vector2& vec, float z = 0);
	~Vector3();
	Vector3 operator+ (const Vector3& rightVector) const; //!< add vectors amd return new one
	Vector3 operator- (const Vector3& rightVector) const; //!< substract vectors and return new one
	float dotAKAscalar(const Vector3& rightVector) const; //!< dot product returns scalar
	float vectLengt() const; //!< function returning length of instanced vector
	float squareMag() const;
	Vector3 vectNormalize() const; //!< function returning new normalized vector
	Vector3 crossProd(const Vector3& rightVector) const; //!< cross product function returning normal vector
	static Vector4 vec3TOvec4(const Vector3& _3Dvector, float w = 0); //!< function to convert 3D vector to 4D vector

	Vector3 operator* (const float& rightFloat) const; //!< vector*num returns new matrix
	Vector3 operator* (const Vector3& rightVector) const; //!< vector*num returns new matrix
	friend Vector3 operator* (const float& leftFloat, const Vector3& rightVector); //!< vector*matrix returns new matrix
	Vector3 operator/ (const float& rightFloat) const; //!< vector/num returns new matrix

	Vector3 normalizeSSE() const;
	float vectLengthSSE() const;

	/// Vector3+=Vector3. Addition is affecting the left Vector
	void operator+=(const Vector3& v);
	/// Vector3-=Vector3. Substraction is affecting the left Vector
	void operator-=(const Vector3& v);
	/// Vector3*=number. Multiplication is affecting the left Vector
	void operator*=(const float& number);
	/// Vector3/=number. Division is affecting the left Vector
	void operator/=(const float& number);
	/// Compares two vectors if they are the same
	bool operator==(const Vector3& v)const;
	float& operator[] (int index); //!< operator[] overload for indexing
protected:

};

inline Vector3 Vector3::normalizeSSE() const
{
	__m128 tmp = _mm_set_ps(0, vect[2], vect[1], vect[0]);
	__m128 length = _mm_sqrt_ss(_mm_dp_ps(tmp, tmp, 0x7F));
	__m128 true_length = _mm_shuffle_ps(length, length, _MM_SHUFFLE(0, 0, 0, 0));

	Vector4 normalized;
	normalized.v = _mm_div_ps(tmp, true_length);
	return Vector3(normalized.x,normalized.y,normalized.z);
}

inline float Vector3::vectLengthSSE() const
{
	__m128 tmp = _mm_set_ps(0, vect[2], vect[1], vect[0]);
	Vector4 length;
	length.v = _mm_sqrt_ss(_mm_dp_ps(tmp, tmp, 0x7F));
	return length.x;
}
}

