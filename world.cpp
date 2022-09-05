#include "DxLib.h"
#include "player.h"
#include<math.h>
#include <time.h>
#include "world.h"

int DrawBG(int image, int worldx, int worldy, int camx, int camy)
{
	DrawGraph(worldx - camx, worldy - camy, image, true);
	return 0;
}

void DrawStatic(int xpos, int ypos, int image)
{
	DrawRotaGraph(xpos, ypos, 1, 0, image, true);
}

int checkHit(int playerX, int playerY, int objX, int objY, int playerSize, int objSize) {
	int flag = FALSE;
	float a = pow(playerX - objX, 2);
	if (a < 0) a = -a;
	float b = pow(playerY - objY, 2);
	if (b < 0) b = -b;
	float c = sqrt(a + b);
	if (c <= playerSize + objSize) flag = TRUE;
	else flag = FALSE;

	return flag;
}

int checkDist(int playerX, int playerY, int objX, int objY) {
	float a = pow(playerX - objX, 2);
	if (a < 0) a = -a;
	float b = pow(playerY - objY, 2);
	if (b < 0) b = -b;
	float c = sqrt(a + b);
	round(c);

	return c;
}

int checkHorDist(int x1, int x2) {
	int x = abs(x1 - x2);
	return x;
}

void circleHitBox(int& playerX, int& playerY, int objX, int objY, int playerSize, int objSize) {
	int dist = checkDist(playerX, playerY, objX, objY);
	if (dist < playerSize + objSize) {
		int disp = playerSize + objSize - dist;
		if (objX >= playerX) playerX -= disp;
		if (objX <= playerX) playerX += disp;
		if (objY >= playerY) playerY -= disp;
		if (objY <= playerY) playerY += disp;
		// NOT PERFECT YET
	}
}

void squareHitBox(int x, int y, int sizeX, int sizeY, int& playerX, int& playerY, int playerSize, int prevX, int prevY) {
	int x2 = x + sizeX;
	int y2 = y + sizeY;

	int boxcenterX = x + sizeX / 2;
	int boxcenterY = y + sizeY / 2;

	int up = playerY - playerSize;
	int down = playerY + playerSize;
	int left = playerX - playerSize;
	int right = playerX + playerSize;

	int horflag = FALSE;
	int verflag = FALSE;
	
	if (right >= x && left <= x2) horflag = TRUE;
	if (down >= y && up <= y2) verflag = TRUE;

	// Prev
	int prevup = prevY - playerSize;
	int prevdown = prevY + playerSize;
	int prevleft = prevX - playerSize;
	int prevright = prevX + playerSize;

	int prevhorflag = FALSE;
	int prevverflag = FALSE;

	if (prevright >= x && prevleft <= x2) prevhorflag = TRUE;
	if (prevdown >= y && prevup <= y2) prevverflag = TRUE;

	// Calculations
	if (horflag && verflag) {
		if (prevhorflag) playerY = prevY;
		else if (prevverflag) playerX = prevX;
	}
}

int checkSquareHit(int x, int y, int sizeX, int sizeY, int playerX, int playerY, int playerSize) {
	int x2 = x + sizeX;
	int y2 = y + sizeY;

	int up = playerY - playerSize;
	int down = playerY + playerSize;
	int left = playerX - playerSize;
	int right = playerX + playerSize;

	int horflag = FALSE;
	int verflag = FALSE;

	if (right >= x && left <= x2) horflag = TRUE;
	if (down >= y && up <= y2) verflag = TRUE;

	int flag = FALSE;
	if (horflag && verflag) flag = TRUE;
	return flag;
}

int heldFlag(int key)
{
	int VALUE;
	if (key) VALUE = TRUE;
	else VALUE = FALSE;
	return VALUE;
}

int buttonFlag(int key, int oldkey) {
	int flag = FALSE;
	if (key && !oldkey) flag = TRUE;
	else if (key == oldkey) flag = FALSE;

	return flag;
}

void basicMovement(int& x, int& y, int up, int down, int left, int right, int maxspeed)
{
	if (up) y -= maxspeed;
	if (down) y += maxspeed;
	if (left) x -= maxspeed;
	if (right) x += maxspeed;
}

void DrawReferencePoint(int x, int y, int camx, int camy, int size) {
	DrawCircle(x - camx, y - camy, size, GetColor(255, 100, 200), true);
}

void DrawReferenceBox(int x, int y, int camx, int camy, int sizeX, int sizeY) {
	int x1 = x - camx, y1 = y - camy;
	DrawBox(x1, y1, x1 + sizeX, y1 + sizeY, GetColor(255, 255, 255), true);
}

void SpawnEntity(int startX, int startY, int& x, int& y, int& aliveflag, int& counter, int& HP, int MAXHP) {
	
	counter++;
	aliveflag = TRUE;
	x = startX;
	y = startY;
	HP = MAXHP;
}

void SimpleFollowScript(int& x, int& y, int chaseX, int chaseY, int speed) {
	if (x < chaseX) x += speed;
	else if (x > chaseX) x -= speed;
	if (y < chaseY) y += speed;
	else if (y > chaseY) y -= speed;
}

int check8Direction(int targetX, int targetY, int x, int y, int originSize) {
	originSize *= 2;
	enum dir {
		up,
		down,
		left,
		right,
		up_left,
		up_right,
		down_left,
		down_right,
	};

	int direction = 1;
	int distX = abs(targetX - x);
	int distY = abs(targetY - y);

	// UP
	if (y > targetY && targetX >= x - originSize && targetX <= x + originSize) direction = up;
	else if (y <= targetY && targetX >= x - originSize && targetX <= x + originSize) direction = down;

	else if (x >= targetX && targetY >= y - originSize && targetY <= y + originSize) direction = left;
	else if (x < targetX && targetY >= y - originSize && targetY <= y + originSize) direction = right;

	else if (y > targetY && targetX < x - originSize) direction = up_left;
	else if (y > targetY && targetX > x + originSize) direction = up_right;
	
	else if (y <= targetY && targetX < x - originSize) direction = down_left;
	else if (y <= targetY && targetX > x + originSize) direction = down_right;
	

	return direction;
}



void spawnOnPlayer(int playerX, int playerY, int& objX, int& objY, int& objDir, int playerDir, int& prevX, int& prevY, int& liveflag) {
	objX = playerX;
	objY = playerY;
	objDir = playerDir;
	prevX = playerX;
	prevY = playerY;
	liveflag = TRUE;
}

void Slash(int& slashX, int& slashY, int& liveflag, int objDir, int startX, int startY) {

	int distance = 128*2;
	int speed = 35;

	enum dir {
		up,
		down,
		left,
		right,
		up_left,
		up_right,
		down_left,
		down_right,
	};

	int endY = 0, endX = 0;

	switch (objDir){
	case up:
		endY = startY - distance;
		if (slashY > endY) slashY -= speed;
		else liveflag = FALSE;
		break;
	case down:
		endY = startY + distance;
		if (slashY < endY) slashY += speed;
		else liveflag = FALSE;
		break;
	case left:
		endX = startX - distance;
		if (slashX > endX) slashX -= speed;
		else liveflag = FALSE;
		break;
	case right:
		endX = startX + distance;
		if (slashX < endX) slashX += speed;
		else liveflag = FALSE;
		break;
	case up_left:
		endX = startX - distance / 2;
		endY = startY - distance / 2;
		if (slashY > endY && slashX > endX) slashY -= speed / 2, slashX -= speed / 2;
		else liveflag = FALSE;
		break;
	case up_right:
		endX = startX + distance / 2;
		endY = startY - distance / 2;
		if (slashY > endY && slashX < endX) slashY -= speed / 2, slashX += speed / 2;
		else liveflag = FALSE;
		break;
	case down_left:
		endX = startX - distance / 2;
		endY = startY + distance / 2;
		if (slashY < endY && slashX > endX) slashY += speed / 2, slashX -= speed / 2;
		else liveflag = FALSE;
		break;
	case down_right:
		endX = startX + distance / 2;
		endY = startY + distance / 2;
		if (slashY < endY && slashX < endX) slashY += speed / 2, slashX += speed / 2;
		else liveflag = FALSE;
		break;
	}
}

void DrawAnimation(int x, int y, int image[], int& index, int& framecount, int framerate, int firstframe, int ammountofframes, int loopflag, int camX, int camY) {
	framecount++;
	if (index < firstframe) index = firstframe;
	if (index > firstframe + ammountofframes && loopflag) index = firstframe;
	if (framecount >= framerate) {
		framecount = 0;
		index++;
		if (loopflag && index >= firstframe + ammountofframes) index = firstframe;
		else if (!loopflag && index >= firstframe + ammountofframes) index = firstframe + ammountofframes - 1;
	}
	DrawRotaGraph(x - camX, y - camY, 1, 0, image[index], true);
}

void DrawRotaAnimation(int x, int y, int image[], int& index, int& framecount, int framerate, int firstframe, int ammountofframes, int loopflag, int camX, int camY, float angle) {
	framecount++;
	if (index < firstframe) index = firstframe;
	if (index > firstframe + ammountofframes) index = firstframe;
	if (framecount >= framerate) {
		framecount = 0;
		index++;
		if (loopflag) if (index >= firstframe + ammountofframes) index = firstframe;
		else if (!loopflag) if (index >= firstframe + ammountofframes) index = firstframe + ammountofframes;
	}
	DrawRotaGraph(x - camX, y - camY, 1, angle, image[index], true);
}


void SideLoadBar(int startX, int startY, int maxX, int sizeY, int var, int varMaxValue, int color1, int color2, int color3) {
	float percent = 0;
	float x;

	percent = (var * 100) / varMaxValue;
	x = (percent * maxX) / 100;

	if (x < 0) x = 0;
	int finalX = roundf(x);

	DrawBox(startX, startY, finalX, startY + sizeY, GetColor(color1, color2, color3), true);
}

void BasicKnockback(int& speedX, int& speedY) {
	speedX = -speedX*1.5;
	speedY = -speedY*1.5;
}