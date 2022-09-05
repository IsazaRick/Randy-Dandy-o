#pragma once

int DrawBG(int image, int worldx, int worldy, int camx, int camy);
void DrawStatic(int xpos, int ypos, int image);
int checkHit(int playerX, int playerY, int objX, int objY, int playerSize, int objSize);
int checkSquareHit(int x, int y, int sizeX, int sizeY, int playerX, int playerY, int playerSize);
int checkDist(int playerX, int playerY, int objX, int objY);
int checkHorDist(int x1, int x2);
int heldFlag(int key);
int buttonFlag(int key, int oldkey);
void basicMovement(int& x, int& y, int up, int down, int left, int right, int maxspeed);
void DrawReferencePoint(int x, int y, int camx, int camy, int size);
void DrawReferenceBox(int x, int y, int camx, int camy, int sizeX, int sizeY);
void circleHitBox(int& playerX, int& playerY, int objX, int objY, int playerSize, int objSize);
void squareHitBox(int x, int y, int sizeX, int sizeY, int& playerX, int& playerY, int playerSize, int prevX, int prevY);
void SpawnEntity(int startX, int startY, int& x, int& y, int& aliveflag, int& counter, int& HP, int MAXHP);
void SimpleFollowScript(int& x, int& y, int chaseX, int chaseY, int speed);
int check8Direction(int targetX, int targetY, int x, int y, int originSize);

void spawnOnPlayer(int playerX, int playerY, int& objX, int& objY, int& objDir, int playerDir, int& prevX, int& prevY, int& liveflag);
void Slash(int& slashX, int& slashY, int& liveflag, int objDir, int startX, int startY);
void DrawAnimation(int x, int y, int image[], int& index, int& framecount, int framerate, int firstframe, int ammountofframes, int loopflag, int camX, int camY);
void DrawRotaAnimation(int x, int y, int image[], int& index, int& framecount, int framerate, int firstframe, int ammountofframes, int loopflag, int camX, int camY, float angle);
void SideLoadBar(int startX, int startY, int maxX, int sizeY, int var, int varMaxValue, int color1, int color2, int color3);
void BasicKnockback(int& speedX, int& speedY);