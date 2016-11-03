#pragma once
namespace mwm
{
class Vector3;
class Matrix3F;
struct loc;
/*! class for constructing matrix*/
class Matrix3

{
public:
	double _matrix[3][3];

	Matrix3(const Matrix3&);
	Matrix3(); //!< in constructor matrix values are set to 0 with memset
	~Matrix3();

	Matrix3 operator~ (); //!< transpose matrix returns new matrix
	Matrix3 operator+ (const Matrix3& rightMatrix);
	Matrix3& operator= (const Matrix3& rightMatrix); //!< copy matrix returns new matrix
	bool operator== (const Matrix3& rightMatrix); //!< check if matrices are identical
	Matrix3 operator* (const Matrix3& rightMatrix); //!< matrix*matrix returns new matrix
	double operator() (int row, int col);//!< operator() overload for indexing
	Matrix3 operator* (const double& rightDouble); //!< matrix*num returns new matrix
	Vector3 operator* (const Vector3& rightVector) const; //!< matrix*vector returns new vector
	double operator[] (loc const& cLoc); //!< operator[] overload for indexing
	double* operator[] (int index); //!< operator[] overload for indexing

	friend Matrix3 operator* (const double& leftDouble, const Matrix3& rightMatrix); //!< num*matrix returns new matrix

	Matrix3 inverse() const; //!< calculates inverse of matrix4x4 and returns as new one
	Matrix3F toFloat(); //!< just converts double matrix to float matrix
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
	static Matrix3 CuboidInertiaTensor(double mass, Vector3& dimensions);
	static double det(double a, double b, double c, double d, double e, double f, double g, double h, double i); //!< calculates determinant of 3x3 matrix
	static Matrix3 perspective(const double &near, const double &far, const double &fov); //!< function returning perspective projection specified with given parameters
	static Matrix3 orthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom); //!< function returninng orthographic projection specified with given parameters
	static Matrix3 translate(double x, double y, double z); //!< returns translation matrix with specified translation values
	static Matrix3 scale(double x, double y, double z); //!< function returning new scale matrix with specified scale values
	static Matrix3 scale(const Vector3& rightVect); //!< function returning new scale matrix with specified scale values
	static Matrix3 rotateX(double angle); //!< function returning rotation matrix with specified rotation angle along X axis
	static Matrix3 rotateY(double angle); //!< function returning rotation matrix with specified rotation angle along Y axis
	static Matrix3 rotateZ(double angle); //!< function returning rotation matrix with specified rotation angle along Z axis
	static Matrix3 rotateAngle(Vector3& thisVector, double angle); //!< function returning rotation matrix with specified rotation angle along specified axis(vector)
	static Matrix3 identityMatrix();

};
}
