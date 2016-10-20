#include "Location.h"

bool Location::compare(Location* loc)
{
	return (this->GetPosition().x == loc->GetPosition().x && this->GetPosition().y == loc->GetPosition().y);
}