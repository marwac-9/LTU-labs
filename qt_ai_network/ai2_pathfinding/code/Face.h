#pragma once
#include "MyMathLib.h"

class Edge;

class Face
{
public:
	Edge* edge;
	Face();
	~Face();
	mwm::Vector2 getMidPointAverage();
	mwm::Vector2 getMidPointMiniMaxi();
	mwm::Vector2 min(mwm::Vector2 &v1, mwm::Vector2 &v2);
	mwm::Vector2 max(mwm::Vector2 &v1, mwm::Vector2 &v2);
	bool joined = false;
private:
	
};

