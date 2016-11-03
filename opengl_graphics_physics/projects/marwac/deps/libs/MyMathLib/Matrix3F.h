#pragma once
namespace mwm
{
class Vector3;
struct loc;
class Matrix3F
{
public:
	float _matrix[3][3];

	Matrix3F(const Matrix3F&);
	Matrix3F();
	~Matrix3F();

	Matrix3F operator~ (); //!< transpose matrix returns new matrix
	Matrix3F operator+ (const Matrix3F& rightMatrix);
	Matrix3F& operator= (const Matrix3F& rightMatrix); //!< copy matrix returns new matrix
	bool operator== (const Matrix3F& rightMatrix); //!< check if matrices are identical
	Matrix3F operator* (const Matrix3F& rightMatrix); //!< matrix*matrix returns new matrix
	float operator() (int row, int col);//!< operator() overload for indexing
	Matrix3F operator* (const float& rightFloat); //!< matrix*num returns new matrix
	Vector3 operator* (const Vector3& rightVector); //!< matrix*vector returns new vector
	float operator[] (loc const& cLoc); //!< operator[] overload for indexing
	float* operator[] (int index); //!< operator[] overload for indexing

	friend Matrix3F operator* (const float& leftFloat, const Matrix3F& rightMatrix); //!< num*matrix returns new matrix

	Matrix3F inverse() const; //!< calculates inverse of matrix4x4 and returns as new one
	Vector3 getUp() const;
	Vector3 getRight() const;
	Vector3 getForwardNegZ() const;
	Vector3 getBackPosZ() const;
	Vector3 getAxis(int axis) const;
	Vector3 getAxisNormalized(int axis) const;
	Vector3 extractScale() const;

	void setUp(const Vector3& axis);
	void setRight(const Vector3& axis);
	void setForward(const Vector3& axis);
	void setAxes(const Vector3& right, const Vector3& up, const Vector3& forward);

	void setSkewSymmetric(const Vector3& vector);

	static Matrix3F CuboidInertiaTensor(float mass, Vector3 dimensions);
	static float det(float a, float b, float c, float d, float e, float f, float g, float h, float i); //!< calculates determinant of 3x3 matrix
	static Matrix3F perspective(const float &near, const float &far, const float &fov); //!< function returning perspective projection specified with given parameters
	static Matrix3F orthographic(const float &near, const float &far, const float &left, const float &right, const float &top, const float &bottom); //!< function returninng orthographic projection specified with given parameters
	static Matrix3F translate(float x, float y, float z); //!< returns translation matrix with specified translation values
	static Matrix3F scale(float x, float y, float z); //!< function returning new scale matrix with specified scale values
	static Matrix3F rotateX(float angle); //!< function returning rotation matrix with specified rotation angle along X axis
	static Matrix3F rotateY(float angle); //!< function returning rotation matrix with specified rotation angle along Y axis
	static Matrix3F rotateZ(float angle); //!< function returning rotation matrix with specified rotation angle along Z axis
	static Matrix3F rotateAngle(Vector3& thisVector, float angle); //!< function returning rotation matrix with specified rotation angle along specified axis(vector)
	static Matrix3F identityMatrix(); //!< identity matrix 


private:

};
}
