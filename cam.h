#pragma once

const int WIN_WIDTH = 1400;
const int WIN_HEIGHT = 800;
const int camdev = 128;
const int WORLD_WIDTH = 3200;
const int WORLD_HEIGHT = 6400;

void movecam(int& camx, int& camy, int playerx, int playery, int playerspeedx, int playerspeedy);