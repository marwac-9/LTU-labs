#include <cmath>
#include <memory.h>
#include "Matrix3.h"
#include "Matrix4F.h"
#include "Matrix3F.h"
#include "Vector3.h"
#include "Vector4.h"
#include "mLoc.h"

namespace mwm
{
Matrix4F::Matrix4F(const Matrix4F& matrix)
{
	*this = matrix;
}

Matrix4F::Matrix4F()
{
	memset(this->_matrix, 0, sizeof this->_matrix);
}

Matrix4F::~Matrix4F()
{

}

/*! \fn operator[] overload for indexing */
float* Matrix4F::operator[] (int index)
{
	return this->_matrix[index];
}

/*! \fn operator[] overload for indexing */
float Matrix4F::operator[](loc const& mLoc)
{
	return this->_matrix[mLoc.x][mLoc.y];
}

/*! \fn operator() overload for indexing*/
float Matrix4F::operator() (int row, int col)
{
	return this->_matrix[row][col];
}

/*! \fn transpose matrix returns new matrix*/
Matrix4F Matrix4F::operator~ ()
{
	Matrix4F realTemp;
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			//cout << first._matrix[r][c] << " result är:" <<  result._matrix[r][c] << endl;
			realTemp._matrix[r][c] = this->_matrix[c][r];
		}
	}
	return realTemp;
}

/*! \fn num*matrix returns new matrix*/
Matrix4F operator*(const float& leftDouble, const Matrix4F& rightMatrix)
{
	Matrix4F temp;
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
Matrix4F Matrix4F::operator* (const float& rightFloat)
{
	Matrix4F temp;
	for (int i = 0; i < 4; i++)
	{
		for (int c = 0; c < 4; c++)
		{
			temp._matrix[i][c] = this->_matrix[i][c] * rightFloat;

		}
	}
	return temp;

}

/*! \fn matrix*matrix returns new matrix*/
Matrix4F Matrix4F::operator* (const Matrix4F& rightMatrix) // matrix multi
{
	Matrix4F temp;

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
Vector4 Matrix4F::operator* (const Vector4& rightVector) // matrix multi
{
	float vx = rightVector.vect[0];
	float vy = rightVector.vect[1];
	float vz = rightVector.vect[2];
	float vw = rightVector.vect[3];

	float _x = this->_matrix[0][0] * vx + this->_matrix[1][0] * vy + this->_matrix[2][0] * vz + this->_matrix[3][0] * vw;
	float _y = this->_matrix[0][1] * vx + this->_matrix[1][1] * vy + this->_matrix[2][1] * vz + this->_matrix[3][1] * vw;
	float _z = this->_matrix[0][2] * vx + this->_matrix[1][2] * vy + this->_matrix[2][2] * vz + this->_matrix[3][2] * vw;
	float _w = this->_matrix[0][3] * vx + this->_matrix[1][3] * vy + this->_matrix[2][3] * vz + this->_matrix[3][3] * vw;
	return Vector4(_x, _y, _z, _w);
}

/*! \fn copy matrix returns new matrix*/
Matrix4F& Matrix4F::operator= (const Matrix4F& rightMatrix)
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
bool Matrix4F::operator== (const Matrix4F& rightMatrix)
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
Matrix4F Matrix4F::rotateX(float angle)
{
	float sAngle = -angle;
	float PI = 3.14159265f;
	float cosAng = cosf(sAngle * PI / 180.0f);
	float sinAng = sinf(sAngle * PI / 180.0f);
	Matrix4F rotationMatrixX;
	//row1
	rotationMatrixX._matrix[0][0] = 1.f;
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
	rotationMatrixX._matrix[3][3] = 1.f;
	return rotationMatrixX;

}

/*! \fn function returning rotation matrix with specified rotation angle along Y axis*/
Matrix4F Matrix4F::rotateY(float angle)
{
	float sAngle = -angle;
	float PI = 3.14159265f;
	float cosAng = cosf(sAngle * PI / 180.0f);
	float sinAng = sinf(sAngle * PI / 180.0f);
	Matrix4F rotationMatrixY;
	//row1
	rotationMatrixY._matrix[0][0] = cosAng;
	//rotationMatrixY._matrix[0][1] = 0;
	rotationMatrixY._matrix[0][2] = sinAng;
	//row2
	//rotationMatrixY._matrix[1][0] = 0;
	rotationMatrixY._matrix[1][1] = 1;
	//rotationMatrixY._matrix[1][2] = 0;
	//row3
	rotationMatrixY._matrix[2][0] = -sinAng;
	//rotationMatrixY._matrix[2][1] = 0;
	rotationMatrixY._matrix[2][2] = cosAng;
	return rotationMatrixY;
}

/*! \fn function returning rotation matrix with specified rotation angle along Z axis*/
Matrix4F Matrix4F::rotateZ(float angle)
{
	float sAngle = -angle;
	float PI = 3.14159265f;
	float cosAng = cosf(sAngle * PI / 180.0f);
	float sinAng = sinf(sAngle * PI / 180.0f);
	Matrix4F rotationMatrixZ;
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
	rotationMatrixZ._matrix[2][2] = 1.f;

	return rotationMatrixZ;
}

/*! \fn function returning rotation matrix with specified rotation angle along specified axis(vector)*/
Matrix4F Matrix4F::rotateAngle(Vector3& thisVector, float angle)
{
	float sAngle = -angle;
	float PI = 3.14159265f;
	float cosAng = cosf(sAngle * PI / 180.0f);
	float sinAng = sinf(sAngle * PI / 180.0f);
	float T = 1.f - cosAng;
	Vector3 normalizedVector = thisVector.normalizeSSE();
	float x = normalizedVector.vect[0];
	float y = normalizedVector.vect[1];
	float z = normalizedVector.vect[2];
	Matrix4F rotationMatrix;
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
Matrix4F Matrix4F::translate(float x, float y, float z)
{
	Matrix4F translation;
	translation._matrix[0][0] = 1.f;
	translation._matrix[1][1] = 1.f;
	translation._matrix[2][2] = 1.f;
	translation._matrix[3][3] = 1.f;
	translation._matrix[0][3] = x;
	translation._matrix[1][3] = y;
	translation._matrix[2][3] = z;
	return translation;
}

/*! \fn function returning new scale matrix with specified scale values*/
Matrix4F Matrix4F::scale(float x, float y, float z)
{
	Matrix4F scaling;
	scaling._matrix[0][0] = x;
	scaling._matrix[1][1] = y;
	scaling._matrix[2][2] = z;
	scaling._matrix[3][3] = 1.f;
	return scaling;
}

/*! \fn calculates inverse of matrix4x4 and returns as new one*/
Matrix4F Matrix4F::inverse() const
{
	//find determinant
	float a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
	a = this->_matrix[0][0]; 	b = this->_matrix[0][1]; 	c = this->_matrix[0][2];	d = this->_matrix[0][3];
	e = this->_matrix[1][0]; 	f = this->_matrix[1][1]; 	g = this->_matrix[1][2];	h = this->_matrix[1][3];
	i = this->_matrix[2][0]; 	j = this->_matrix[2][1]; 	k = this->_matrix[2][2];	l = this->_matrix[2][3];
	m = this->_matrix[3][0];	n = this->_matrix[3][1];	o = this->_matrix[3][2];	p = this->_matrix[3][3];

	float det = a*(Matrix3F::det(f, g, h, j, k, l, n, o, p)) - b*(Matrix3F::det(e, g, h, i, k, l, m, o, p)) + c*(Matrix3F::det(e, f, h, i, j, l, m, n, p)) - d*(Matrix3F::det(e, f, g, i, j, k, m, n, o));
	//cout << det << endl;
	Matrix4F temp;
	//matrix of minors(matrices 3x3, calculating their determinants), transpose and change signs in one
	temp._matrix[0][0] = (Matrix3F::det(f, g, h, j, k, l, n, o, p));	temp._matrix[0][1] = -(Matrix3F::det(b, c, d, j, k, l, n, o, p));	temp._matrix[0][2] = (Matrix3F::det(b, c, d, f, g, h, n, o, p));	temp._matrix[0][3] = -(Matrix3F::det(b, c, d, f, g, h, j, k, l));			// + - + -
	temp._matrix[1][0] = -(Matrix3F::det(e, g, h, i, k, l, m, o, p));	temp._matrix[1][1] = (Matrix3F::det(a, c, d, i, k, l, m, o, p));	temp._matrix[1][2] = -(Matrix3F::det(a, c, d, e, g, h, m, o, p));	temp._matrix[1][3] = (Matrix3F::det(a, c, d, e, g, h, i, k, l));			// - + - +
	temp._matrix[2][0] = (Matrix3F::det(e, f, h, i, j, l, m, n, p));	temp._matrix[2][1] = -(Matrix3F::det(a, b, d, i, j, l, m, n, p));	temp._matrix[2][2] = (Matrix3F::det(a, b, d, e, f, h, m, n, p));	temp._matrix[2][3] = -(Matrix3F::det(a, b, d, e, f, h, i, j, l)); 		// + - + -
	temp._matrix[3][0] = -(Matrix3F::det(e, f, g, i, j, k, m, n, o));	temp._matrix[3][1] = (Matrix3F::det(a, b, c, i, j, k, m, n, o));	temp._matrix[3][2] = -(Matrix3F::det(a, b, c, e, f, g, m, n, o));	temp._matrix[3][3] = (Matrix3F::det(a, b, c, e, f, g, i, j, k));			// - + - +
	float oneByDet = 1 / det;

	temp = oneByDet*temp;

	return temp;
}

/*! \fn function returning perspective projection specified with given parameters*/
Matrix4F Matrix4F::perspective(const float &near, const float &far, const float &fov)
{
	float PI = 3.14159265f;
	float S = float(1 / tan((fov * 0.5*(PI / 180))));
	Matrix4F temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);
	temp._matrix[3][2] = -far * near / (far - near);
	temp._matrix[2][3] = -1;
	//temp._matrix[3][3] = 0;

	return temp;
}

/*! \fn function returninng orthographic projection specified with given parameters*/
Matrix4F Matrix4F::orthographic(const float &near, const float &far, const float &left, const float &right, const float &top, const float &bottom)
{
	Matrix4F temp;
	temp._matrix[0][0] = 2 / (right - left);
	temp._matrix[1][1] = 2 / (top - bottom);
	temp._matrix[2][2] = -2 / (far - near);
	temp._matrix[0][3] = -((right + left) / (right - left));
	temp._matrix[1][3] = -((top + bottom) / (top - bottom));
	temp._matrix[2][3] = -((far + near) / (far - near));
	temp._matrix[3][3] = 1;
	return temp;
}

/*! \fn identity matrix */
Matrix4F Matrix4F::identityMatrix()
{
	Matrix4F temp;
	temp._matrix[0][0] = 1;
	temp._matrix[1][1] = 1;
	temp._matrix[2][2] = 1;
	temp._matrix[3][3] = 1;
	return temp;
}

/*! \fn returns transposed translation matrix with specified translation values*/
Matrix4F Matrix4F::sTranslate(float x, float y, float z)
{
	Matrix4F translation;
	translation._matrix[0][0] = 1;
	translation._matrix[1][1] = 1;
	translation._matrix[2][2] = 1;
	translation._matrix[3][3] = 1;
	translation._matrix[0][3] = x;
	translation._matrix[1][3] = y;
	translation._matrix[2][3] = z;
	return translation;
}

/*! \fn returns transposed perspective projection matrix specified with given parameters*/
Matrix4F Matrix4F::sPerspective(const float &near, const float &far, const float &fov)
{
	float PI = 3.14159265f;
	float S = 1.f / tan((fov * 0.5f*(PI / 180.f)));
	Matrix4F temp;
	temp._matrix[0][0] = S;
	temp._matrix[1][1] = S;
	temp._matrix[2][2] = -far / (far - near);

	temp._matrix[2][3] = -far * near / (far - near);
	temp._matrix[3][2] = -1;

	temp._matrix[3][3] = 0;

	return temp;
}

/*! \fn lookAt matrix not optimized*/
Matrix4F Matrix4F::nolookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalizeSSE();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalizeSSE(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4F orientation;
	orientation[0][0] = xaxis.vect[0];
	orientation[0][1] = yaxis.vect[0];
	orientation[0][2] = zaxis.vect[0];

	orientation[1][0] = xaxis.vect[1];
	orientation[1][1] = yaxis.vect[1];
	orientation[1][2] = zaxis.vect[1];

	orientation[2][0] = xaxis.vect[2];
	orientation[2][1] = yaxis.vect[2];
	orientation[2][2] = zaxis.vect[2];

	orientation[3][3] = 1;

	// Create a 4x4 translation matrix.
	// The eye position is negated which is equivalent
	// to the inverse of the translation matrix.
	// T(v)^-1 == T(-v)
	Matrix4F translation;
	translation[0][0] = 1;

	translation[1][1] = 1;

	translation[2][2] = 1;

	translation[3][0] = -eye.vect[0];
	translation[3][1] = -eye.vect[1];
	translation[3][2] = -eye.vect[2];
	translation[3][3] = 1;

	// Combine the orientation and translation to compute
	// the final view matrix
	return (translation * orientation);

}

/*! \fn lookAt matrix optimized*/
Matrix4F Matrix4F::lookAt(Vector3 eye, Vector3 target, Vector3 up)
{

	Vector3 zaxis = (eye - target).normalizeSSE();    // The "forward" vector.
	Vector3 xaxis = up.crossProd(zaxis).normalizeSSE(); // The "right" vector.  normal(cross(up, zaxis));
	Vector3 yaxis = zaxis.crossProd(xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse
	// if the matrix is orthonormalized (in this case, it is).
	Matrix4F orientation;
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
	orientation[3][3] = 1;

	// the final view matrix
	return orientation;

}

// Pitch should be in the range of [-90 ... 90] degrees and yaw
// should be in the range of [0 ... 360] degrees.
/*! \fn fps cam matrix*/
Matrix4F Matrix4F::FPScam(Vector3 eye, float pitch, float yaw)
{
	// If the pitch and yaw angles are in degrees,
	// they need to be converted to radians. Here
	// I assume the values are already converted to radians.
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	Vector3 xaxis = Vector3(cosYaw, 0.f, -sinYaw);
	Vector3 yaxis = Vector3(sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);
	Vector3 zaxis = Vector3(sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw);

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	Matrix4F FPSView;
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
	FPSView[3][3] = 1;

	return FPSView;
}

/*! \fn fustrum matrix used for building perspective with aspect ratio*/
Matrix4F Matrix4F::Frustum(float left, float right, float bottom, float top, float near, float far)
{
	Matrix4F fustrum;

	fustrum._matrix[0][0] = 2 * near / (right - left);
	//fustrum._matrix[0][1] = 0;
	//fustrum._matrix[0][2] = 0;
	//fustrum._matrix[0][3] = 0;

	//fustrum._matrix[1][0] = 0;
	fustrum._matrix[1][1] = 2 * near / (top - bottom);
	//fustrum._matrix[1][2] = 0;
	//fustrum._matrix[1][3] = 0;

	fustrum._matrix[2][0] = (right + left) / (right - left);
	fustrum._matrix[2][1] = (top + bottom) / (top - bottom);
	fustrum._matrix[2][2] = -(far + near) / (far - near);
	fustrum._matrix[2][3] = -1;

	//fustrum._matrix[3][0] = 0;
	//fustrum._matrix[3][1] = 0;
	fustrum._matrix[3][2] = -2 * far * near / (far - near);
	//fustrum._matrix[3][3] = 0;

	return fustrum;
}

/*! \fn transposed fustrum matrix used for building transposed perspective with aspect ratio*/
Matrix4F Matrix4F::sFrustum(float left, float right, float bottom, float top, float near, float far)
{
	Matrix4F fustrum;

	fustrum._matrix[0][0] = 2.f * near / (right - left);
	fustrum._matrix[0][1] = 0.f;
	fustrum._matrix[0][2] = (right + left) / (right - left);
	fustrum._matrix[0][3] = 0.f;

	fustrum._matrix[1][0] = 0.f;
	fustrum._matrix[1][1] = 2.f * near / (top - bottom);
	fustrum._matrix[1][2] = (top + bottom) / (top - bottom);
	fustrum._matrix[1][3] = 0.f;

	fustrum._matrix[2][0] = 0.f;
	fustrum._matrix[2][1] = 0.f;
	fustrum._matrix[2][2] = -(far + near) / (far - near);
	fustrum._matrix[2][3] = -2.f * far * near / (far - near);

	fustrum._matrix[3][0] = 0.f;
	fustrum._matrix[3][1] = 0.f;
	fustrum._matrix[3][2] = -1.f;
	fustrum._matrix[3][3] = 0.f;

	return fustrum;
}

/*! \fn transposed perspective matrix with aspect ratio*/
Matrix4F Matrix4F::sOpenGLPersp(float fov, float imageAspectRatio, float near, float far)
{
	float PI = 3.14159265f;
	float scale = tanf(fov * 0.5f*(PI / 180.f)) * near;
	float right = imageAspectRatio * scale;
	float left = -right;
	float top = scale;
	float bottom = -top;
	return sFrustum(left, right, bottom, top, near, far);
}

/*! \fn perspective matrix with aspect ratio*/
Matrix4F Matrix4F::OpenGLPersp(float fov, float imageAspectRatio, float near, float far)
{
	float PI = 3.14159265f;
	float scale = tanf(fov * 0.5f*(PI / 180)) * near;
	float right = imageAspectRatio * scale;
	float left = -right;
	float top = scale;
	float bottom = -top;
	return Frustum(left, right, bottom, top, near, far);
}

//no need to transpose that either
Matrix3F Matrix4F::ConvertToMatrix3() const
{
	Matrix3F mat3;
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

Vector3 Matrix4F::getScale() const
{
	return Vector3(_matrix[0][0], _matrix[1][1], _matrix[2][2]);
}


Vector3 Matrix4F::getPosition() const
{
	return Vector3(_matrix[3][0], _matrix[3][1], _matrix[3][2]);
}

Vector3 Matrix4F::getRight() const
{
	return Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).normalizeSSE();
}

Vector3 Matrix4F::getUp() const
{
	return Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).normalizeSSE();
}

Vector3 Matrix4F::getForwardNegZ() const
{
	return Vector3(_matrix[2][0] * -1.f, _matrix[2][1] * -1.f, _matrix[2][2] * -1.f).normalizeSSE();
}

Vector3 Matrix4F::getBackPosZ() const
{
	return Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).normalizeSSE();
}

void Matrix4F::setUp(const Vector3& axis)
{
	_matrix[1][0] = axis.x;
	_matrix[1][1] = axis.y;
	_matrix[1][2] = axis.z;
}

void Matrix4F::setRight(const Vector3& axis)
{
	_matrix[0][0] = axis.x;
	_matrix[0][1] = axis.y;
	_matrix[0][2] = axis.z;
}

void Matrix4F::setForward(const Vector3& axis)
{
	_matrix[2][0] = axis.x;
	_matrix[2][1] = axis.y;
	_matrix[2][2] = axis.z;
}

Vector3 Matrix4F::getAxis(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]);
}

Vector3 Matrix4F::getAxisNormalized(int axis) const
{
	return Vector3(_matrix[axis][0], _matrix[axis][1], _matrix[axis][2]).normalizeSSE();
}

Vector3 Matrix4F::extractScale() const
{
	float scaleX = Vector3(_matrix[0][0], _matrix[0][1], _matrix[0][2]).vectLengthSSE();
	float scaleY = Vector3(_matrix[1][0], _matrix[1][1], _matrix[1][2]).vectLengthSSE();
	float scaleZ = Vector3(_matrix[2][0], _matrix[2][1], _matrix[2][2]).vectLengthSSE();
	return Vector3(scaleX, scaleY, scaleZ);
}
}

