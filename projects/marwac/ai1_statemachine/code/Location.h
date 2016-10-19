#pragma once 
#include "Position.h"
#include "Size.h"
#include <string>
#include "Graphics.h"
class Location
{
public:
	std::string name = "newLocation";
	int id;
	bool compare(Location* loc);
	Position GetPosition() const { return pos; }
	Size GetSize() { return graphicProperties.GetSize(); }
	Color GetColor() { return graphicProperties.GetColor(); }
	void SetPosition(int posX, int posY) { pos.SetPosition(posX, posY); }
	void SetSize(int width, int height) { graphicProperties.SetSize(width, height); }
	void SetColor(int red, int green, int blue) { graphicProperties.SetColor(red, green, blue); }
	void SetX(int posX) { pos.SetX(posX); }
	void SetY(int posY) { pos.SetY(posY); }
private:
	Position pos;
	Graphics graphicProperties;
};

