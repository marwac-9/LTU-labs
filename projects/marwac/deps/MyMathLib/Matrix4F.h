#pragma once
namespace mwm
{
class Vector4;
class Vector3;
class Matrix3F;
struct loc;

class Matrix4F
{
public:
	float _matrix[4][4];

	Matrix4F(const Matrix4F&);
	Matrix4F();
	~Matrix4F();

	Matrix4F operator~ (); //!< transpose matrix returns new matrix
	Matrix4F& operator= (const Matrix4F& rightMatrix); //!< copy matrix returns new matrix
	bool operator== (const Matrix4F& rightMatrix); //!< check if matrices are identical
	Matrix4F operator* (const Matrix4F& rightMatrix); //!< matrix*matrix returns new matrix
	float operator() (int row, int col);//!< operator() overload for indexing
	Matrix4F operator* (const float& rightFloat); //!< matrix*num returns new matrix
	Vector4 operator* (const Vector4& rightVector); //!< matrix*vector returns new vector
	float operator[] (loc const& cLoc); //!< operator[] overload for indexing
	float* operator[] (int index); //!< operator[] overload for indexing
	Matrix4F inverse() const; //!< calculates inverse of matrix4x4 and returns as new one
	friend Matrix4F operator* (const float& leftFloat, const Matrix4F& rightMatrix); //!< num*matrix returns new matrix
	Matrix3F ConvertToMatrix3() const;
	Vector3 getScale() const;
	Vector3 getPosition() const;
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

	static Matrix4F perspective(const float &near, const float &far, const float &fov); //!< function returning perspective projection specified with given parameters
	static Matrix4F orthographic(const float &near, const float &far, const float &left, const float &right, const float &top, const float &bottom); //!< function returninng orthographic projection specified with given parameters
	static Matrix4F translate(float x, float y, float z); //!< returns translation matrix with specified translation values
	static Matrix4F scale(float x, float y, float z); //!< function returning new scale matrix with specified scale values
	static Matrix4F rotateX(float angle); //!< function returning rotation matrix with specified rotation angle along X axis
	static Matrix4F rotateY(float angle); //!< function returning rotation matrix with specified rotation angle along Y axis
	static Matrix4F rotateZ(float angle); //!< function returning rotation matrix with specified rotation angle along Z axis
	static Matrix4F rotateAngle(Vector3& thisVector, float angle); //!< function returning rotation matrix with specified rotation angle along specified axis(vector)

	static Matrix4F nolookAt(Vector3 eye, Vector3 target, Vector3 up); //!< lookAt matrix not optimized
	static Matrix4F lookAt(Vector3 eye, Vector3 target, Vector3 up); //!< lookAt matrix optimized
	static Matrix4F FPScam(Vector3 eye, float pitch, float yaw); //!< fps camera matrix
	static Matrix4F Frustum(float l, float r, float b, float t, float n, float f); //!< frustum matrix used to produce perspective matrix with aspect ratio
	static Matrix4F OpenGLPersp(float fov, float imageAspectRatio, float near, float far); //!< perspective matrix with aspect ratio

	static Matrix4F sFrustum(float left, float right, float bottom, float top, float near, float far); //!< transposed frustum matrix used to produce transposed perspective matrix with aspect ratio
	static Matrix4F sOpenGLPersp(float fov, float imageAspectRatio, float near, float far); //!< transposed perspective matrix with aspect ratio
	static Matrix4F sPerspective(const float &near, const float &far, const float &fov); //!< function returning transposed perspective 
	static Matrix4F sTranslate(float x, float y, float z); //!< returns transposed translation matrix with specified translation values

	static Matrix4F identityMatrix(); //!< identity matrix 


private:

};
}


