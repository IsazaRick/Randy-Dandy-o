#pragma once

class player
{
public:
	int x, y;
	int prevX, prevY;
	int size;
	int aliveflag = FALSE;
	int HP;
	int speed;
	int dir = 1;
	int framecount = 0, framerate = 0, index = 0;
	int attacktimer = 60;
};