#pragma once
namespace mwm
{
class Vector3;
class Vector2
{
public:
	union
	{
		struct{ float x, y; };
		float vect[2];
	};


	Vector2(float x = 0, float y = 0);
	Vector2(double x, double y);
	Vector2(const Vector2& vect);
	~Vector2();
	Vector2 operator+ (const Vector2& rightVector) const; //!< add vectors amd return new one
	Vector2 operator- (const Vector2& rightVector) const; //!< substract vectors and return new one
	float dotAKAscalar(const Vector2& rightVector) const; //!< dot product returns scalar
	float vectLengt() const; //!< function returning length of instanced vector
	Vector2 vectNormalize() const; //!< function returning new normalized vector
	Vector3 vec2TOvec3(Vector2 _2Dvector, float z = 0); //!< function to convert 3D vector to 4D vector

	Vector2 operator* (const float& rightFloat) const; //!< vector*num returns new matrix
	friend Vector2 operator* (const float& leftFloat, const Vector2& rightVector); //!< num*matrix returns new matrix
	Vector2 operator/ (const float& rightFloat) const; //!< vector/num returns new matrix

protected:
};
}
