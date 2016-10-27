#include "MyMathLib.h"

struct OBB
{
	mwm::Vector3 halfExtent;//only obb
	mwm::Matrix3 model;
	//prob should include halfExtend rather than scale, halfExtent = 0.5*scale
	//prob should include rot matrix instead of model it should not have the scale in it
	mwm::MinMax mm;
	mwm::Vector3 color;
};

struct AABB
{
	mwm::Matrix4 model;
	mwm::Vector3 color;
};