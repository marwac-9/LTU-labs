#pragma once
struct Position
{
	int x = 0;
	int y = 0;
	void SetPosition(int posX, int posY) { x = posX; y = posY; }
	void SetX(int posX) { x = posX; }
	void SetY(int posY) { y = posY; }
};