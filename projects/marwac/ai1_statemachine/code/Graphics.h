#pragma once 
#include "Color.h"
#include "Position.h"
#include "Size.h"
class Graphics 
{
public:
	void SetColor(int red, int green, int blue) { color.SetColor(red, green, blue); }
	void SetSize(int w, int h) { size.SetSize(w, h); }
	Color GetColor() const { return color; }
	Size GetSize() const { return size; }
private:
	Color color;
	Size size;
};
