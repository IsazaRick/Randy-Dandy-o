#include "cam.h"
#include<math.h>


void movecam(int& camx, int& camy, int playerx, int playery, int playerspeedx, int playerspeedy) {

	if (abs(camx - playerx) <= (WIN_WIDTH / 2) - camdev) camx -= abs(playerspeedx);
	else if (abs(camx - playerx) >= (WIN_WIDTH / 2) + camdev) camx += abs(playerspeedx);
	if (abs(camy - playery) <= (WIN_HEIGHT / 2) - camdev) camy -= abs(playerspeedy);
	if (abs(camy - playery) >= (WIN_HEIGHT / 2) + camdev) camy += abs(playerspeedy);

	if (camx >= WORLD_WIDTH - WIN_WIDTH) camx = WORLD_WIDTH - WIN_WIDTH;
	else if (camx <= 0) camx = 0;
	if (camy >= WORLD_HEIGHT - WIN_HEIGHT) camy = WORLD_HEIGHT - WIN_HEIGHT;
	else if (camy <= 0) camy = 0;

}