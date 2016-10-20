#include <cmath>
#include <memory.h>
#include "Matrix3.h"
#include "Matrix3F.h"
#include "Vector3.h"
#include "mLoc.h"

namespace mwm
{
Matrix3::Matrix3(const Matrix3& matrix)
{
	*this = matrix;
}

/*! \fn in constructor matrix values are set to 0 with memset*/
Matrix3::Matrix3()
{
	memset(this->_matrix, 0, sizeof this->_matrix);
}

Matrix3::~Matrix3()
{

}

/*! \fn identity matrix */
Matrix3 Matrix3::identityMatrix()
{
	Matrix3 temp;
	temp._matrix[0][0] = 1.0;
	temp._matrix[1][1] = 1.0;
	temp._matrix[2][2] = 1.0;
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn converts double matrix to float */
Matrix3F Matrix3::toFloat()
{
	Matrix3F temp;
	temp._matrix[0][0] = (float)this->_matrix[0][0]; 	temp._matrix[0][1] = (float)this->_matrix[0][1]; 	temp._matrix[0][2] = (float)this->_matrix[0][2];	temp._matrix[0][3] = (float)this->_matrix[0][3];
	temp._matrix[1][0] = (float)this->_matrix[1][0]; 	temp._matrix[1][1] = (float)this->_matrix[1][1]; 	temp._matrix[1][2] = (float)this->_matrix[1][2];	temp._matrix[1][3] = (float)this->_matrix[1][3];
	temp._matrix[2][0] = (float)this->_matrix[2][0]; 	temp._matrix[2][1] = (float)this->_matrix[2][1]; 	temp._matrix[2][2] = (float)this->_matrix[2][2];	temp._matrix[2][3] = (float)this->_matrix[2][3];
	return temp;
}

/*! \fn operator[] overload for indexing */
double* Matrix3::operator[] (int index)
{
	return this->_matrix[index];
}

/*! \fn operator[] overload for indexing */
double Matrix3::operator[](loc const& mLoc)
{
	return this->_matrix[mLoc.x][mLoc.y];
}

/*! \fn operator() overload for indexing*/
double Matrix3::operator() (int row, int col)
{
	return this->_matrix[row][col];
}

/*! \fn transpose matrix returns new matrix*/
Matrix3 Matrix3::operator~ ()
{
	Matrix3 realTemp;
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			//cout << first._matrix[r][c] << " result is:" <<  result._matrix[r][c] << endl;
			realTemp._matrix[r][c] = this->_matrix[c][r];
		}
	}
	return realTemp;
}

Matrix3 Matrix3::operator+(const Matrix3& rightMatrix)
{
	Matrix3 realTemp;
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			realTemp._matrix[r][c] = this->_matrix[r][c] + rightMatrix._matrix[r][c];
		}
	}
	return realTemp;
}

/*! \fn num*matrix returns new matrix*/
Matrix3 operator*(const double& leftDouble, const Matrix3& rightMatrix)
{
	Matrix3 temp;
	for (int i = 0; i < 3; i++)
	{
		for (int c = 0; c < 3; c++)
		{
			temp._matrix[i][c] = leftDouble * rightMatrix._matrix[i][c];
		}
	}
	return temp;
}

/*! \fn matrix*num returns new matrix*/
Matrix3 Matrix3::operator* (const double& rightDouble)
{
	Matrix3 temp;
	for (int i = 0; i < 3; i++)
	{
		for (int c = 0; c < 3; c++)
		{
			temp._matrix[i][c] = this->_matrix[i][c] * rightDouble;

		}
	}
	return temp;

}

/*! \fn matrix*matrix returns new matrix*/
Matrix3 Matrix3::operator* (const Matrix3& rightMatrix) // matrix multi
{
	Matrix3 temp;

	//multiply
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			for (int k = 0; k < 3; k++)
			{
				temp._matrix[r][c] = temp._matrix[r][c] + this->_matrix[r][k] * rightMatrix._matrix[k][c];

			}
		}
	}
	return temp;
}

/*! \fn matrix*vector returns new vector*/
Vector3 Matrix3::operator* (const Vector3& rightVector) const// matrix multi
{
	//row major
	double vx = rightVector.vect[0];
	double vy = rightVector.vect[1];
	double vz = rightVector.vect[2];

	double _x = this->_matrix[0][0] * vx + this->_matrix[1][0] * vy + this->_matrix[2][0] * vz;
	double _y = this->_matrix[0][1] * vx + this->_matrix[1][1] * vy + this->_matrix[2][1] * vz;
	double _z = this->_matrix[0][2] * vx + this->_matrix[1][2] * vy + this->_matrix[2][2] * vz;
	return Vector3(_x, _y, _z);
}

/*! \fn copy matrix returns new matrix*/
Matrix3& Matrix3::operator= (const Matrix3& rightMatrix)
{
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			this->_matrix[r][c] = rightMatrix._matrix[r][c];

		}
	}
	return *this;

}

/*! \fn check if matrices are identical*/
bool Matrix3::operator== (const Matrix3& rightMatrix)
{
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			if (this->_matrix[r][c] != rightMatrix._matrix[r][c])
			{
				return false;
			}
		}
	}
	return true;
}

/*! \fn function returning rotation matrix with specified rotation angle along X axis*/
Matrix3 Matrix3::rotateX(double angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix3 rotationMatrixX;
	//row1
	rotationMatrixX._matrix[0][0] = 1.0;
	//rotationMatrixX._matrix[0][1] = 0;
	//rotationMatrixX._matrix[0][2] = 0;
	//rotationMatrixX._matrix[0][3] = 0;
	//row2
	//rotationMatrixX._matrix[1][0] = 0;
	rotationMatrixX._matrix[1][1] = cosAng;
	rotationMatrixX._matrix[1][2] = -sinAng;
	//rotationMatrixX._matrix[1][3] = 0;
	//row3
	//rotationMatrixX._matrix[2][0] = 0;
	rotationMatrixX._matrix[2][1] = sinAng;
	rotationMatrixX._matrix[2][2] = cosAng;
	//rotationMatrixX._matrix[2][3] = 0;
	//row4
	//rotationMatrixX._matrix[3][0] = 0;
	//rotationMatrixX._matrix[3][1] = 0;
	//rotationMatrixX._matrix[3][2] = 0;
	//rotationMatrixX._matrix[3][3] = 1;
	return rotationMatrixX;

}

/*! \fn function returning rotation matrix with specified rotation angle along Y axis*/
Matrix3 Matrix3::rotateY(double angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix3 rotationMatrixY;
	//row1
	rotationMatrixY._matrix[0][0] = cosAng;
	//rotationMatrixY._matrix[0][1] = 0;
	rotationMatrixY._matrix[0][2] = sinAng;
	//row2
	//rotationMatrixY._matrix[1][0] = 0;
	rotationMatrixY._matrix[1][1] = 1.0;
	//rotationMatrixY._matrix[1][2] = 0;
	//row3
	rotationMatrixY._matrix[2][0] = -sinAng;
	//rotationMatrixY._matrix[2][1] = 0;
	rotationMatrixY._matrix[2][2] = cosAng;
	return rotationMatrixY;
}

/*! \fn function returning rotation matrix with specified rotation angle along Z axis*/
Matrix3 Matrix3::rotateZ(double angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix3 rotationMatrixZ;
	//row1
	rotationMatrixZ._matrix[0][0] = cosAng;
	rotationMatrixZ._matrix[0][1] = -sinAng;
	//rotationMatrixZ._matrix[0][2] = 0;
	//row2
	rotationMatrixZ._matrix[1][0] = sinAng;
	rotationMatrixZ._matrix[1][1] = cosAng;
	//rotationMatrixZ._matrix[1][2] = 0;
	//row3
	//rotationMatrixZ._matrix[2][0] = 0;
	//rotationMatrixZ._matrix[2][1] = 0;
	rotationMatrixZ._matrix[2][2] = 1.0;

	return rotationMatrixZ;
}

/*! \fn function returning rotation matrix with specified rotation angle along specified axis(vector)*/
Matrix3 Matrix3::rotateAngle(Vector3& thisVector, double angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	double T = 1 - cosAng;
	Vector3 normalizedVector = thisVector.normalizeSSE();
	float x = normalizedVector.vect[0];
	float y = normalizedVector.vect[1];
	float z = normalizedVector.vect[2];
	Matrix3 rotationMatrix;
	//row1
	rotationMatrix._matrix[0][0] = cosAng + (x*x) * T;
	rotationMatrix._matrix[0][1] = x*y * T - z * sinAng;
	rotationMatrix._matrix[0][2] = x*z * T + y * sinAng;
	//row2
	rotationMatrix._matrix[1][0] = y*x * T + z * sinAng;
	rotationMatrix._matrix[1][1] = cosAng + y*y * T;
	rotationMatrix._matrix[1][2] = y*z * T - x * sinAng;
	//row3
	rotationMatrix._matrix[2][0] = z*x * T - y * sinAng;
	rotationMatrix._matrix[2][1] = z*y * T + x * sinAng;
	rotationMatrix._matrix[2][2] = cosAng + z*z * T;
	return rotationMatrix;
}

/*! \fn returns translation matrix with specified translation values*/
Matrix3 Matrix3::translate(double x, double y, double z)
{
	Matrix3 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	//translation._matrix[3][3] = 1;
	translation._matrix[0][2] = x;
	translation._matrix[1][2] = y;
	translation._matrix[2][2] = z;
	return translation;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix3 Matrix3::scale(double x, double y, double z)
{
	Matrix3 scaling;
	scaling._matrix[0][0] = x;
	scaling._matrix[1][1] = y;
	scaling._matrix[2][2] = z;
	//scaling._matrix[3][3] = 1;
	return scaling;
}

Matrix3 Matrix3::scale(const Vector3& rightVect)
{
	Matrix3 scaling;
	scaling._matrix[0][0] = rightVect.x;
	scaling._matrix[1][1] = rightVect.y;
	scaling._matrix[2][2] = rightVect.z;
	//scaling._matrix[3][3] = 1;
	return scaling;
}

/*! \fn calculates inverse of matrix3x3 and returns as new one*/
Matrix3 Matrix3::inverse() const
{
	//find determinant
	double a, b, c, d, e, f, g, h, i;
	a = this->_matrix[0][0]; 	b = this->_matrix[0][1]; 	c = this->_matrix[0][2];
	d = this->_matrix[1][0]; 	e = this->_matrix[1][1]; 	f = this->_matrix[1][2];
	g = this->_matrix[2][0]; 	h = this->_matrix[2][1]; 	i = this->_matrix[2][2];

	//double det = a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h; //or a(ei-fh)-b(id-fg)+c(dh-eg)
	double det = Matrix3::det(a, b, c, d, e, f, g, h, i);
	//cout << det << endl;
	Matrix3 temp;
	//matrix of minor, transpose and change signs in one
	temp._matrix[0][0] = (e*i - f*h);	temp._matrix[0][1] = -(b*i - c*h);	temp._matrix[0][2] = (b*f - c*e);			// + - +
	temp._matrix[1][0] = -(d*i - f*g);	temp._matrix[1][1] = (a*i - c*g);	temp._matrix[1][2] = -(a*f - c*d);			// - + -
	temp._matrix[2][0] = (d*h - e*g);	temp._matrix[2][1] = -(a*h - b*g);	temp._matrix[2][2] = (a*e - b*d);			// + - +

	double oneByDet = 1.0 / det;

	temp = oneByDet*temp;

	return temp;
}

/*! \fn calculates determinant of 3x3 matrix*/
double Matrix3::det(double a, double b, double c, double d, double e, double f, double g, double h, double i)
{
	return a*(e*i - f*h) - b*(i*d - f*g) + c*(d*h - e*g);
	//simplified: return (a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h);
}

//no need to transpose
Matrix3 Matrix3::CuboidInertiaTensor(double mass, Vector3& dimensions)
{
	Matrix3 I;
	I[0][0] = (1.f / 12.f) * mass * (dimensions.y*dimensions.y + dimensions.z*dimensions.z);
	I[1][1] = (1.f / 12.f) * mass * (dimensions.x*dimensions.x + dimensions.z*dimensions.z);
	I[2][2] = (1.f / 12.f) * mass * (dimensions.x*dimensions.x + dimensions.y*dimensions.y);

	return I;
}

Vector3 Matrix3::getRight() const
{
	return Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalizeSSE();
}

Vector3 Matrix3::getUp() const
{
	return Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalizeSSE();
}

Vector3 Matrix3::getForwardNegZ() const
{
	return Vector3(_matrix[2][0] * -1.0, _matrix[2][1] * -1.0, _matrix[2][2] * -1.0).normalizeSSE();
}

Vector3 Matrix3::getBackPosZ() const
{
	return Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalizeSSE();
}

void Matrix3::setUp(const Vector3& axis)
{
	_matrix[1][0] = axis.x;
	_matrix[1][1] = axis.y;
	_matrix[1][2] = axis.z;
}

void Matrix3::setRight(const Vector3& axis)
{
	_matrix[0][0] = axis.x;
	_matrix[0][1] = axis.y;
	_matrix[0][2] = axis.z;
}

void Matrix3::setForward(const Vector3& axis)
{
	_matrix[2][0] = axis.x;
	_matrix[2][1] = axis.y;
	_matrix[2][2] = axis.z;
}

void Matrix3::setAxes(const Vector3& right, const Vector3& up, const Vector3& forward)
{
	_matrix[0][0] = right.x;
	_matrix[0][1] = right.y;
	_matrix[0][2] = right.z;

	_matrix[1][0] = up.x;
	_matrix[1][1] = up.y;
	_matrix[1][2] = up.z;

	_matrix[2][0] = forward.x;
	_matrix[2][1] = forward.y;
	_matrix[2][2] = forward.z;
}

/**
* Sets the matrix to be a skew symmetric matrix based on
* the given vector. The skew symmetric matrix is the equivalent
* of the vector product. So if a,b are vectors. a x b = A_s b
* where A_s is the skew symmetric form of a.
*/
void Matrix3::setSkewSymmetric(const Vector3& vector)
{
	_matrix[0][0] = _matrix[1][1] = _matrix[2][2] = 0;
	_matrix[0][1] = -vector.z;
	_matrix[0][2] = vector.y;
	_matrix[1][0] = vector.z;
	_matrix[1][2] = -vector.x;
	_matrix[2][0] = -vector.y;
	_matrix[2][1] = vector.x;
}

Vector3 Matrix3::getAxis(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]);
}

Vector3 Matrix3::getAxisNormalized(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]).normalizeSSE();
}

Vector3 Matrix3::extractScale() const
{
	double scaleX = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).vectLengthSSE();
	double scaleY = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).vectLengthSSE();
	double scaleZ = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).vectLengthSSE();
	return Vector3(scaleX, scaleY, scaleZ);
}
}
