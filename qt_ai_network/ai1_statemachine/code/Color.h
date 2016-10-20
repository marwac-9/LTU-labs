#pragma once
struct Color
{
public:
	int r = 255;
	int g = 255;
	int b = 255;
	void SetColor(int &red, int &green, int &blue) { r = red; g = green; b = blue; }
private:
};