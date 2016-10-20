#pragma once
#include "MyMathLib.h"
#include "Object.h"
#include "BoundingBox.h"

struct ObjectPoint
{
	Object* obj;
	bool isMin = false;
	ObjectPoint(){}
	ObjectPoint(Object* ent, bool min){ obj = ent; isMin = min; }
	float value(int axis)
	{
		if (isMin)
		{
			return obj->obb.mm.min[axis];
		}
		else
		{
			return obj->obb.mm.max[axis];
		}
	}
};