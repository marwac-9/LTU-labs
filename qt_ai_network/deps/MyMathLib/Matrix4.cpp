#include <cmath>
#include <memory.h>
#include "Matrix3.h"
#include "Matrix4.h"
#include "Matrix4F.h"
#include "Vector3.h"
#include "Vector4.h"
#include "mLoc.h"

namespace mwm
{
Matrix4::Matrix4(const Matrix4& matrix)
{
	*this = matrix;
}

/*! \fn in constructor matrix values are set to 0 with memset*/
Matrix4::Matrix4()
{
	memset(this->_matrix, 0, sizeof this->_matrix);
}

Matrix4::~Matrix4()
{

}

/*! \fn converts double matrix to float */
Matrix4F Matrix4::toFloat() const
{
	Matrix4F temp;
	temp._matrix[0][0] = (float)this->_matrix[0][0]; 	temp._matrix[0][1] = (float)this->_matrix[0][1]; 	temp._matrix[0][2] = (float)this->_matrix[0][2];	temp._matrix[0][3] = (float)this->_matrix[0][3];
	temp._matrix[1][0] = (float)this->_matrix[1][0]; 	temp._matrix[1][1] = (float)this->_matrix[1][1]; 	temp._matrix[1][2] = (float)this->_matrix[1][2];	temp._matrix[1][3] = (float)this->_matrix[1][3];
	temp._matrix[2][0] = (float)this->_matrix[2][0]; 	temp._matrix[2][1] = (float)this->_matrix[2][1]; 	temp._matrix[2][2] = (float)this->_matrix[2][2];	temp._matrix[2][3] = (float)this->_matrix[2][3];
	temp._matrix[3][0] = (float)this->_matrix[3][0];	temp._matrix[3][1] = (float)this->_matrix[3][1];	temp._matrix[3][2] = (float)this->_matrix[3][2];	temp._matrix[3][3] = (float)this->_matrix[3][3];
	return temp;
}

/*! \fn operator[] overload for indexing */
double* Matrix4::operator[] (int index)
{
	return this->_matrix[index];
}

/*! \fn operator[] overload for indexing */
double Matrix4::operator[](loc const& mLoc) const
{
	return this->_matrix[mLoc.x][mLoc.y];
}

/*! \fn operator() overload for indexing*/
double Matrix4::operator() (int row, int col) const
{
	return this->_matrix[row][col];
}

/*! \fn transpose matrix returns new matrix*/
Matrix4 Matrix4::operator~ () const
{
	Matrix4 realTemp;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			//cout << first._matrix[r][c] << " result �r:" <<  result._matrix[r][c] << endl;
			realTemp._matrix[r][c] = this->_matrix[c][r];
		}
	}
	return realTemp;
}

/*! \fn num*matrix returns new matrix*/
Matrix4 operator*(const double& leftDouble, const Matrix4& rightMatrix)
{
	Matrix4 temp;
	for (int i = 0; i < 4; i++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp._matrix[i][c] = leftDouble * rightMatrix._matrix[i][c];
		}
	}
	return temp;
}

/*! \fn matrix*num returns new matrix*/
Matrix4 Matrix4::operator* (const double& rightDouble) const
{
	Matrix4 temp;
	for (int i = 0; i < 4; i++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp._matrix[i][c] = this->_matrix[i][c] * rightDouble;

		}
	}
	return temp;

}

/*! \fn matrix*matrix returns new matrix*/
Matrix4 Matrix4::operator* (const Matrix4& rightMatrix) const// matrix multi
{
	Matrix4 temp;

	//multiply
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			for (int k = 0; k < 4; k++)
			{
				temp._matrix[r][c] = temp._matrix[r][c] + this->_matrix[r][k] * rightMatrix._matrix[k][c];

			}
		}
	}
	return temp;
}

/*! \fn matrix*vector returns new vector*/
Vector4 Matrix4::operator* (const Vector4& rightVector) const// matrix multi
{
	double vx = rightVector.vect[0];
	double vy = rightVector.vect[1];
	double vz = rightVector.vect[2];
	double vw = rightVector.vect[3];

	double _x = this->_matrix[0][0] * vx + this->_matrix[1][0] * vy + this->_matrix[2][0] * vz + this->_matrix[3][0] * vw;
	double _y = this->_matrix[0][1] * vx + this->_matrix[1][1] * vy + this->_matrix[2][1] * vz + this->_matrix[3][1] * vw;
	double _z = this->_matrix[0][2] * vx + this->_matrix[1][2] * vy + this->_matrix[2][2] * vz + this->_matrix[3][2] * vw;
	double _w = this->_matrix[0][3] * vx + this->_matrix[1][3] * vy + this->_matrix[2][3] * vz + this->_matrix[3][3] * vw;
	return Vector4(_x, _y, _z, _w);
}

/*! \fn copy matrix returns new matrix*/
Matrix4& Matrix4::operator= (const Matrix4& rightMatrix)
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			this->_matrix[r][c] = rightMatrix._matrix[r][c];

		}
	}
	return *this;

}

/*! \fn check if matrices are identical*/
bool Matrix4::operator== (const Matrix4& rightMatrix) const
{
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
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
Matrix4 Matrix4::rotateX(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixX;
	//row1
	rotationMatrixX._matrix[0][0] = 1;
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
	rotationMatrixX._matrix[3][3] = 1;
	return rotationMatrixX;

}

/*! \fn function returning rotation matrix with specified rotation angle along Y axis*/
Matrix4 Matrix4::rotateY(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixY;

	//row1
	rotationMatrixY._matrix[0][0] = cosAng;
	//rotationMatrixY._matrix[0][1] = 0;
	rotationMatrixY._matrix[0][2] = sinAng;
	//rotationMatrixY._matrix[0][3] = 0;
	//row2
	//rotationMatrixY._matrix[1][0] = 0;
	rotationMatrixY._matrix[1][1] = 1.0;
	//rotationMatrixY._matrix[1][2] = 0;
	//rotationMatrixY._matrix[1][3] = 0;
	//row3
	rotationMatrixY._matrix[2][0] = -sinAng;
	//rotationMatrixY._matrix[2][1] = 0;
	rotationMatrixY._matrix[2][2] = cosAng;
	//rotationMatrixY._matrix[2][3] = 1;
	//row4
	//rotationMatrixY._matrix[3][0] = 0;
	//rotationMatrixY._matrix[3][1] = 0;
	//rotationMatrixY._matrix[3][2] = 0;
	rotationMatrixY._matrix[3][3] = 1.0;
	return rotationMatrixY;
}

/*! \fn function returning rotation matrix with specified rotation angle along Z axis*/
Matrix4 Matrix4::rotateZ(const double &angle)
{
	double sAngle = -angle;
	double PI = 3.14159265;
	double cosAng = cos(sAngle * PI / 180.0);
	double sinAng = sin(sAngle * PI / 180.0);
	Matrix4 rotationMatrixZ;
	//row1
	rotationMatrixZ._matrix[0][0] = cosAng;
	rotationMatrixZ._matrix[0][1] = -sinAng;
	//rotationMatrixZ._matrix[0][2] = 0;
	//rotationMatrixZ._matrix[0][3] = 0;
	//row2
	rotationMatrixZ._matrix[1][0] = sinAng;
	rotationMatrixZ._matrix[1][1] = cosAng;
	//rotationMatrixZ._matrix[1][2] = 0;
	//rotationMatrixZ._matrix[1][3] = 0;
	//row3
	//rotationMatrixZ._matrix[2][0] = 0;
	//rotationMatrixZ._matrix[2][1] = 0;
	rotationMatrixZ._matrix[2][2] = 1.0;
	//rotationMatrixZ._matrix[2][3] = 0;
	//row4
	//rotationMatrixZ._matrix[3][0] = 0;
	//rotationMatrixZ._matrix[3][1] = 0;
	//rotationMatrixZ._matrix[3][2] = 0;
	rotationMatrixZ._matrix[3][3] = 1.0;

	return rotationMatrixZ;
}

/*! \fn function returning rotation matrix with specified rotation angle along specified axis(vector)*/
Matrix4 Matrix4::rotateAngle(const Vector3& thisVector, const double &angle)
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
	Matrix4 rotationMatrix;
	//row1
	rotationMatrix._matrix[0][0] = cosAng + (x*x) * T;
	rotationMatrix._matrix[0][1] = x*y * T - z * sinAng;
	rotationMatrix._matrix[0][2] = x*z * T + y * sinAng;
	//rotationMatrix._matrix[0][3] = 0;
	//row2
	rotationMatrix._matrix[1][0] = y*x * T + z * sinAng;
	rotationMatrix._matrix[1][1] = cosAng + y*y * T;
	rotationMatrix._matrix[1][2] = y*z * T - x * sinAng;
	//rotationMatrix._matrix[1][3] = 0;
	//row3
	rotationMatrix._matrix[2][0] = z*x * T - y * sinAng;
	rotationMatrix._matrix[2][1] = z*y * T + x * sinAng;
	rotationMatrix._matrix[2][2] = cosAng + z*z * T;
	//rotationMatrix._matrix[2][3] = 0;
	//row4
	//rotationMatrix._matrix[3][0] = 0;
	//rotationMatrix._matrix[3][1] = 0;
	//rotationMatrix._matrix[3][2] = 0;
	rotationMatrix._matrix[3][3] = 1.0;
	return rotationMatrix;
}


Matrix4 Matrix4::rotateAngleT(const Vector3& thisVector, const double &angle)
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
	Matrix4 rotationMatrix;
	//row1
	rotationMatrix._matrix[0][0] = cosAng + (x*x) * T;
	rotationMatrix._matrix[1][0] = x*y * T - z * sinAng;
	rotationMatrix._matrix[2][0] = x*z * T + y * sinAng;
	//rotationMatrix._matrix[3][0] = 0.0;
	//row2
	rotationMatrix._matrix[0][1] = y*x * T + z * sinAng;
	rotationMatrix._matrix[1][1] = cosAng + y*y * T;
	rotationMatrix._matrix[2][1] = y*z * T - x * sinAng;
	//rotationMatrix._matrix[3][1] = 0.0;
	//row3
	rotationMatrix._matrix[0][2] = z*x * T - y * sinAng;
	rotationMatrix._matrix[1][2] = z*y * T + x * sinAng;
	rotationMatrix._matrix[2][2] = cosAng + z*z * T;
	//rotationMatrix._matrix[3][2] = 0.0;
	//row4
	//rotationMatrix._matrix[0][3] = 0.0;
	//rotationMatrix._matrix[1][3] = 0.0;
	//rotationMatrix._matrix[2][3] = 0.0;
	rotationMatrix._matrix[3][3] = 1.0;
	return rotationMatrix;
}


/*! \fn returns translation matrix with specified translation values*/
Matrix4 Matrix4::translate(const double &x, const double &y, const double &z)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[3][0] = x;
	translation._matrix[3][1] = y;
	translation._matrix[3][2] = z;
	return translation;
}

/*! \fn returns translation matrix with specified translation values*/
Matrix4 Matrix4::translate(const Vector3& rightVector)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[3][0] = rightVector.vect[0];
	translation._matrix[3][1] = rightVector.vect[1];
	translation._matrix[3][2] = rightVector.vect[2];
	return translation;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix4 Matrix4::scale(const double &x, const double &y, const double &z)
{
	Matrix4 scaling;
	scaling._matrix[0][0] = x;
	scaling._matrix[1][1] = y;
	scaling._matrix[2][2] = z;
	scaling._matrix[3][3] = 1.0;
	return scaling;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix4 Matrix4::scale(const Vector3& rightVector)
{
	Matrix4 scaling;
	scaling._matrix[0][0] = rightVector.vect[0];
	scaling._matrix[1][1] = rightVector.vect[1];
	scaling._matrix[2][2] = rightVector.vect[2];
	scaling._matrix[3][3] = 1.0;
	return scaling;
}


/*! \fn calculates inverse of matrix4x4 and returns as new one*/
Matrix4 Matrix4::inverse() const
{
	//find determinant
	double a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
	a = this->_matrix[0][0]; 	b = this->_matrix[0][1]; 	c = this->_matrix[0][2];	d = this->_matrix[0][3];
	e = this->_matrix[1][0]; 	f = this->_matrix[1][1]; 	g = this->_matrix[1][2];	h = this->_matrix[1][3];
	i = this->_matrix[2][0]; 	j = this->_matrix[2][1]; 	k = this->_matrix[2][2];	l = this->_matrix[2][3];
	m = this->_matrix[3][0];	n = this->_matrix[3][1];	o = this->_matrix[3][2];	p = this->_matrix[3][3];

	double det = a*(Matrix3::det(f, g, h, j, k, l, n, o, p)) - b*(Matrix3::det(e, g, h, i, k, l, m, o, p)) + c*(Matrix3::det(e, f, h, i, j, l, m, n, p)) - d*(Matrix3::det(e, f, g, i, j, k, m, n, o));
	//cout << det << endl;
	Matrix4 temp;
	//matrix of minors(matrices 3x3, calculating their determinants), transpose and change signs in one
	temp._matrix[0][0] = (Matrix3::det(f, g, h, j, k, l, n, o, p));	temp._matrix[0][1] = -(Matrix3::det(b, c, d, j, k, l, n, o, p));	temp._matrix[0][2] = (Matrix3::det(b, c, d, f, g, h, n, o, p));	temp._matrix[0][3] = -(Matrix3::det(b, c, d, f, g, h, j, k, l));			// + - + -
	temp._matrix[1][0] = -(Matrix3::det(e, g, h, i, k, l, m, o, p));	temp._matrix[1][1] = (Matrix3::det(a, c, d, i, k, l, m, o, p));	temp._matrix[1][2] = -(Matrix3::det(a, c, d, e, g, h, m, o, p));	temp._matrix[1][3] = (Matrix3::det(a, c, d, e, g, h, i, k, l));			// - + - +
	temp._matrix[2][0] = (Matrix3::det(e, f, h, i, j, l, m, n, p));	temp._matrix[2][1] = -(Matrix3::det(a, b, d, i, j, l, m, n, p));	temp._matrix[2][2] = (Matrix3::det(a, b, d, e, f, h, m, n, p));	temp._matrix[2][3] = -(Matrix3::det(a, b, d, e, f, h, i, j, l)); 		// + - + -
	temp._matrix[3][0] = -(Matrix3::det(e, f, g, i, j, k, m, n, o));	temp._matrix[3][1] = (Matrix3::det(a, b, c, i, j, k, m, n, o));	temp._matrix[3][2] = -(Matrix3::det(a, b, c, e, f, g, m, n, o));	temp._matrix[3][3] = (Matrix3::det(a, b, c, e, f, g, i, j, k));			// - + - +
	double oneByDet = 1.0 / det;

	temp = oneByDet*temp;

	return temp;
}

/*! \fn function returning perspective projection specified with given parameters*/
Matrix4 Matrix4::perspective(const double &near, const double &far, const double &fov)
{
	double PI = 3.14159265;
	double S = 1.0 / tan(fov * 0.5*(PI / 180.0));
	Matrix4 temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);
	temp._matrix[3][2] = -far * near / (far - near);
	temp._matrix[2][3] = -1.0;
	//temp._matrix[3][3] = 0;

	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::sOrthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (top - bottom);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[0][3] = -((right + left) / (right - left));
	temp._matrix[1][3] = -((top + bottom) / (top - bottom));
	temp._matrix[2][3] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

Matrix4 Matrix4::sOrthographicTopToBottom(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (bottom - top);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[0][3] = -((right + left) / (right - left));
	temp._matrix[1][3] = -((top + bottom) / (bottom - top));
	temp._matrix[2][3] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::orthographic(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (top - bottom);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[3][0] = -((right + left) / (right - left));
	temp._matrix[3][1] = -((top + bottom) / (top - bottom));
	temp._matrix[3][2] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4 Matrix4::orthographicTopToBottom(const double &near, const double &far, const double &left, const double &right, const double &top, const double &bottom)
{
	Matrix4 temp;
	temp._matrix[0][0] = 2.0 / (right - left);
	temp._matrix[1][1] = 2.0 / (bottom - top);
	temp._matrix[2][2] = -2.0 / (far - near);
	temp._matrix[3][0] = -((right + left) / (right - left));
	temp._matrix[3][1] = -((top + bottom) / (bottom - top));
	temp._matrix[3][2] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn identity matrix */
Matrix4 Matrix4::identityMatrix()
{
	Matrix4 temp;
	temp._matrix[0][0] = 1.0;
	temp._matrix[1][1] = 1.0;
	temp._matrix[2][2] = 1.0;
	temp._matrix[3][3] = 1.0;
	return temp;
}

/*! \fn bias shadowmap matrix*/
Matrix4 Matrix4::biasMatrix()
{
	Matrix4 temp;
	temp._matrix[0][0] = 0.5;
	temp._matrix[1][1] = 0.5;
	temp._matrix[2][2] = 0.5;
	temp._matrix[3][3] = 1.0;
	temp._matrix[3][0] = 0.5;
	temp._matrix[3][1] = 0.5;
	temp._matrix[3][2] = 0.5;
	return temp;
}

/*! \fn returns transposed translation matrix with specified translation values*/
Matrix4 Matrix4::sTranslate(const double &x, const double &y, const double &z)
{
	Matrix4 translation;
	translation._matrix[0][0] = 1.0;
	translation._matrix[1][1] = 1.0;
	translation._matrix[2][2] = 1.0;
	translation._matrix[3][3] = 1.0;
	translation._matrix[0][3] = x;
	translation._matrix[1][3] = y;
	translation._matrix[2][3] = z;
	return translation;
}

/*! \fn returns transposed perspective projection matrix specified with given parameters*/
Matrix4 Matrix4::sPerspective(const double &near, const double &far, const double &fov)
{
	double PI = 3.14159265;
	double S = 1.0 / tan((fov * 0.5*(PI / 180.0)));
	Matrix4 temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);

	temp._matrix[2][3] = -far * near / (far - near);
	temp._matrix[3][2] = -1.0;

	temp._matrix[3][3] = 0.0;

	return temp;
}

/*! \fn lookAt matrix not optimized*/
Matrix4 Matrix4::nolookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalizeSSE();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalizeSSE(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4 orientation;
	orientation[0][0] = xaxis.vect[0];
	orientation[0][1] = yaxis.vect[0];
	orientation[0][2] = zaxis.vect[0];

	orientation[1][0] = xaxis.vect[1];
	orientation[1][1] = yaxis.vect[1];
	orientation[1][2] = zaxis.vect[1];

	orientation[2][0] = xaxis.vect[2];
	orientation[2][1] = yaxis.vect[2];
	orientation[2][2] = zaxis.vect[2];

	orientation[3][3] = 1.0;

	// Create a 4x4 translation matrix.
	// The eye position is negated which is equivalent
	// to the inverse of the translation matrix.
	// T(v)^-1 == T(-v)
	Matrix4 translation;
	translation[0][0] = 1.0;

	translation[1][1] = 1.0;

	translation[2][2] = 1.0;

	translation[3][0] = -eye.vect[0];
	translation[3][1] = -eye.vect[1];
	translation[3][2] = -eye.vect[2];
	translation[3][3] = 1.0;

	// Combine the orientation and translation to compute
	// the final view matrix
	return (translation * orientation);

}

/*! \fn lookAt matrix optimized*/
Matrix4 Matrix4::lookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalizeSSE();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalizeSSE(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4 orientation;
	orientation[0][0] = xaxis.vect[0];
	orientation[0][1] = yaxis.vect[0];
	orientation[0][2] = zaxis.vect[0];

	orientation[1][0] = xaxis.vect[1];
	orientation[1][1] = yaxis.vect[1];
	orientation[1][2] = zaxis.vect[1];

	orientation[2][0] = xaxis.vect[2];
	orientation[2][1] = yaxis.vect[2];
	orientation[2][2] = zaxis.vect[2];

	orientation[3][0] = -(xaxis.dotAKAscalar(eye));
	orientation[3][1] = -(yaxis.dotAKAscalar(eye));
	orientation[3][2] = -(zaxis.dotAKAscalar(eye));
	orientation[3][3] = 1.0;

	// the final view matrix
	return orientation;

}

// Pitch should be in the range of [-90 ... 90] degrees and yaw
// should be in the range of [0 ... 360] degrees.
/*! \fn fps cam matrix*/
Matrix4 Matrix4::FPScam(Vector3 eye, const double &pitch, const double &yaw)
{
	// If the pitch and yaw angles are in degrees,
	// they need to be converted to radians. Here
	// I assume the values are already converted to radians.
	double cosPitch = cos(pitch);
	double sinPitch = sin(pitch);
	double cosYaw = cos(yaw);
	double sinYaw = sin(yaw);

	Vector3 xaxis = Vector3(cosYaw, 0.0, -sinYaw);
	Vector3 yaxis = Vector3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	Vector3 zaxis = Vector3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	Matrix4 FPSView;
	FPSView[0][0] = xaxis.vect[0];
	FPSView[0][1] = yaxis.vect[0];
	FPSView[0][2] = zaxis.vect[0];
	//FPSView[0][3] = 0;

	FPSView[1][0] = xaxis.vect[1];
	FPSView[1][1] = yaxis.vect[1];
	FPSView[1][2] = zaxis.vect[1];
	//FPSView[1][3] = 0;

	FPSView[2][0] = xaxis.vect[2];
	FPSView[2][1] = yaxis.vect[2];
	FPSView[2][2] = zaxis.vect[2];
	//FPSView[2][3] = 0;

	FPSView[3][0] = -(xaxis.dotAKAscalar(eye));
	FPSView[3][1] = -(yaxis.dotAKAscalar(eye));
	FPSView[3][2] = -(zaxis.dotAKAscalar(eye));
	FPSView[3][3] = 1.0;

	return FPSView;
}

/*! \fn fustrum matrix used for building perspective with aspect ratio*/
Matrix4 Matrix4::Frustum(const double &l, const double &r, const double &b, const double &t, const double &n, const double &f)
{
	Matrix4 fustrum;

	fustrum._matrix[0][0] = 2.0 * n / (r - l);
	//fustrum._matrix[0][1] = 0;
	//fustrum._matrix[0][2] = 0;
	//fustrum._matrix[0][3] = 0;

	//fustrum._matrix[1][0] = 0;
	fustrum._matrix[1][1] = 2.0 * n / (t - b);
	//fustrum._matrix[1][2] = 0;
	//fustrum._matrix[1][3] = 0;

	fustrum._matrix[2][0] = (r + l) / (r - l);
	fustrum._matrix[2][1] = (t + b) / (t - b);
	fustrum._matrix[2][2] = -(f + n) / (f - n);
	fustrum._matrix[2][3] = -1.0;

	//fustrum._matrix[3][0] = 0;
	//fustrum._matrix[3][1] = 0;
	fustrum._matrix[3][2] = -2.0 * f * n / (f - n);
	//fustrum._matrix[3][3] = 0;

	return fustrum;
}

/*! \fn transposed fustrum matrix used for building transposed perspective with aspect ratio*/
Matrix4 Matrix4::sFrustum(const double &left, const double &right, const double &bottom, const double &top, const double &near, const double &far)
{
	Matrix4 fustrum;

	fustrum._matrix[0][0] = 2.0 * near / (right - left);
	fustrum._matrix[0][1] = 0.0;
	fustrum._matrix[0][2] = (right + left) / (right - left);
	fustrum._matrix[0][3] = 0.0;

	fustrum._matrix[1][0] = 0.0;
	fustrum._matrix[1][1] = 2.0 * near / (top - bottom);
	fustrum._matrix[1][2] = (top + bottom) / (top - bottom);
	fustrum._matrix[1][3] = 0.0;

	fustrum._matrix[2][0] = 0.0;
	fustrum._matrix[2][1] = 0.0;
	fustrum._matrix[2][2] = -(far + near) / (far - near);
	fustrum._matrix[2][3] = -2.0 * far * near / (far - near);

	fustrum._matrix[3][0] = 0.0;
	fustrum._matrix[3][1] = 0.0;
	fustrum._matrix[3][2] = -1.0;
	fustrum._matrix[3][3] = 0.0;

	return fustrum;
}

/*! \fn transposed perspective matrix with aspect ratio*/
Matrix4 Matrix4::sOpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far)
{
	double PI = 3.14159265;
	double scale = tan(fov * 0.5*(PI / 180.0)) * near;
	double right = imageAspectRatio * scale;
	double left = -right;
	double top = scale;
	double bottom = -top;
	return sFrustum(left, right, bottom, top, near, far);
}

/*! \fn perspective matrix with aspect ratio*/
Matrix4 Matrix4::OpenGLPersp(const double &fov, const double &imageAspectRatio, const double &near, const double &far)
{
	double PI = 3.14159265;
	double scale = tan(fov * 0.5*(PI / 180.0)) * near;
	double right = imageAspectRatio * scale;
	double left = -right;
	double top = scale;
	double bottom = -top;
	return Frustum(left, right, bottom, top, near, far);
}

//no need to transpose that either
Matrix3 Matrix4::ConvertToMatrix3() const
{
	Matrix3 mat3;
	mat3[0][0] = this->_matrix[0][0];
	mat3[0][1] = this->_matrix[0][1];
	mat3[0][2] = this->_matrix[0][2];

	mat3[1][0] = this->_matrix[1][0];
	mat3[1][1] = this->_matrix[1][1];
	mat3[1][2] = this->_matrix[1][2];

	mat3[2][0] = this->_matrix[2][0];
	mat3[2][1] = this->_matrix[2][1];
	mat3[2][2] = this->_matrix[2][2];

	return mat3;
}

Vector3 Matrix4::getScale() const
{
	return Vector3(_matrix[0][0], _matrix[1][1], _matrix[2][2]);
}


Vector3 Matrix4::getPosition() const
{
	return Vector3(_matrix[3][0], _matrix[3][1], _matrix[3][2]);
}

Vector3 Matrix4::getRight() const
{
	return Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalizeSSE();
}

Vector3 Matrix4::getUp() const
{
	return Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalizeSSE();
}

Vector3 Matrix4::getForwardNegZ() const
{
	return Vector3(_matrix[2][0] * -1.0, _matrix[2][1] * -1.0, _matrix[2][2] * -1.0).normalizeSSE();
}

Vector3 Matrix4::getBackPosZ() const
{
	return Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalizeSSE();
}

Vector3 Matrix4::getAxis(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]);
}

Vector3 Matrix4::getAxisNormalized(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]).normalizeSSE();
}

void Matrix4::setUp(const Vector3& axis)
{
	_matrix[1][0] = axis.x;
	_matrix[1][1] = axis.y;
	_matrix[1][2] = axis.z;
}

void Matrix4::setRight(const Vector3& axis)
{
	_matrix[0][0] = axis.x;
	_matrix[0][1] = axis.y;
	_matrix[0][2] = axis.z;
}

void Matrix4::setForward(const Vector3& axis)
{
	_matrix[2][0] = axis.x;
	_matrix[2][1] = axis.y;
	_matrix[2][2] = axis.z;
}

Vector3 Matrix4::extractScale() const
{
	float scaleX = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).vectLengthSSE();
	float scaleY = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).vectLengthSSE();
	float scaleZ = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).vectLengthSSE();
	return Vector3(scaleX, scaleY, scaleZ);
}
}
