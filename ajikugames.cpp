#include "ajikugames.h"
#include "DxLib.h"
#include "cam.h"

void ajikugames::DrawLogo(int& index, int& framecount, int& scenestate, int frame, int lenght) {
	if (!CheckSoundMem(logosound)) PlaySoundMem(logosound, DX_PLAYTYPE_BACK);
	framecount++;
	if (framecount >= 5) {
		index++;
		if (index >= 4) index = 0;
		framecount = 0;
	}
	DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 1.5, 0, logo[index], true);
	if (frame > lenght * 2) scenestate++;
}