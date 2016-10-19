#include "Edge.h"
#include "Vertex.h"

using namespace mwm;

Edge::Edge()
{
}

Edge::~Edge()
{
}

Vector2 Edge::Midpoint()
{
	Vector2 midpoint = (this->vertex->pos + this->next->vertex->pos) / 2.f;
	return midpoint;
}