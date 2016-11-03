#pragma once
#include "MyMathLib.h"

class Vertex;
class Face;

class Edge
{
public:
	Vertex *vertex, *midVertex;
	Edge *next;
	Edge *prev;
	Edge *pair;
	Face *face;

	Edge();
	~Edge();
	mwm::Vector2 Midpoint();
private:

};

