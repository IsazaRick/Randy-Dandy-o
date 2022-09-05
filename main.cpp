#include "DxLib.h"
#include<time.h>
#include "player.h"
#include "cam.h"
#include "world.h"
#include "ajikugames.h"


const char TITLE[] = "Randy Dandy-O";


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetWindowSizeChangeEnableFlag(TRUE, TRUE);
	SetMainWindowText(TITLE);
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);
	SetWindowSizeExtendRate(1.0);
	SetBackgroundColor(0x00, 0x00, 0x00);
	if (DxLib_Init() == -1) { return -1; }
	SetDrawScreen(DX_SCREEN_BACK);
	char keys[256] = {0}, prevkeys[256] = {0};
	int frame = 0;
	srand(time(NULL));
	SetMouseDispFlag(FALSE);

	// Scenestate
	int scenestate = 0;
	int gamestate = 0;
	enum states {
		ajkgames,
		mainmenu,
		gameplay,
	};
	enum gamestates {
		tutorial,
		load,
		game,
		finish,
		fail,
	}; 
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
	enum battleState {
		following,
		aiming,
		shooting,
	};
	int bossState = 0;

	// IMAGES & SOUNDS
	ajikugames ajikugames;
	ajikugames.framecount = 0, ajikugames.index = 0;
	ajikugames.logosound = LoadSoundMem("Resource\\AjikuGames\\ajikugamessound.wav");
	LoadDivGraph("Resource\\AjikuGames\\ajikugameslogo.png", 4, 2, 2, 640, 256, ajikugames.logo, true);
	int reticle = LoadGraph("Resource\\rec.png", true);
	int yellreticle = LoadGraph("Resource\\yell_rec.png", true);
	int shipbody = LoadGraph("Resource\\Ship\\shipbody-big.png", true);
	int shipdetail = LoadGraph("Resource\\Ship\\shipdetail-big-trans.png", true);
	int deadscimitar = LoadGraph("Resource\\Enemies\\deadscimitar.png");
	int pirateimg[80];
	LoadDivGraph("Resource\\player\\pirate_sheet.png", 80, 8, 10, 128, 128, pirateimg, true);
	int scimitarsailorimg[32];
	LoadDivGraph("Resource\\Enemies\\scimitar_sailor.png", 32, 8, 4, 128, 128, scimitarsailorimg, true);
	int blueslash[4];
	LoadDivGraph("Resource\\blueslash.png", 4, 4, 1, 40, 128, blueslash, true);
	int redslash[4];
	LoadDivGraph("Resource\\redslash.png", 4, 4, 1, 40, 128, redslash, true);
	int bigslash[4];
	LoadDivGraph("Resource\\bossslash.png", 4, 4, 1, 64, 192, bigslash, true);
	int water[2];
	LoadDivGraph("Resource\\water-Sheet.png", 2, 1, 2, 64*5, 64, water, true);
	int waterindex = 0, waterframecount = 0;
	int smallship = LoadGraph("Resource\\Ship\\smallship.png", true);
	int enemyshipY = 1600;
	int HUD = LoadGraph("Resource\\HUD.png", true);
	int title = LoadGraph("Resource\\title.png", true);
	int bossimg[56];
	LoadDivGraph("Resource\\Enemies\\Boss-Sheet.png", 56, 8, 7, 192, 192, bossimg, true);
	int bossreticle = LoadGraph("Resource\\bossrec.png", true);
	int bossrecskull = LoadGraph("Resource\\bossrecskull.png", true);
	int recX = 0, recY = 0;

	player pirate;
	pirate.x = 0;
	pirate.y = 0;
	pirate.prevX = 0;
	pirate.prevY = 0;
	pirate.size = 32;
	pirate.speed = 10;
	pirate.framerate = 5;
	pirate.HP = 40;

	int enemiesalive = 0;
	player swordSailor[20];
	for (int x = 0; x < 20; x++) {
		swordSailor[x].x = 0;
		swordSailor[x].y = 0;
		swordSailor[x].prevX = 0;
		swordSailor[x].prevY = 0;
		swordSailor[x].aliveflag = FALSE;
		swordSailor[x].HP = 5;
		swordSailor[x].size = 25;
		swordSailor[x].speed = 4;
		swordSailor[x].framerate = 5;
	}

	player enemySlash[20];
	for (int x = 0; x < 20; x++) {
		enemySlash[x].x = 0;
		enemySlash[x].y = 0;
		enemySlash[x].prevX = 0;
		enemySlash[x].prevY = 0;
		enemySlash[x].aliveflag = FALSE;
		enemySlash[x].size = 32;
		enemySlash[x].speed = 35;
		enemySlash[x].framerate = 5;
	}

	player slashEntity;
	slashEntity.x = 0;
	slashEntity.y = 0;
	slashEntity.size = 32;

	int returnflag = FALSE;

	player boss;
	boss.x = 2300, boss.y = 2180;
	boss.HP = 600;
	boss.size = 100;
	boss.framerate = 6;

	player bossSlash;
	bossSlash.size = 64;

	int spawntimer = 0, spawnfrecuency = 300;

	int cameraX = 0;
	int cameraY = 0;

	int tutorialslash = FALSE;
	int tutorialshoot = FALSE;
	int dead1 = FALSE, dead2 = FALSE;

	// Timers
	int hurttimer = 0, hurtcd = 60, shoottimer = 0, shootcd = 20;
	int bossTimer = 0;

	// Button Flags
	int mouseX = 0, mouseY = 0;
	int mouseMapX = 0, mouseMapY = 0;
	int spaceflag = FALSE, spaceheldflag = FALSE;
	int shootflag = FALSE, slashflag = FALSE, shotflag = FALSE;
	int upflag = FALSE, downflag = FALSE, leftflag = FALSE, rightflag = FALSE;
	int clickstateRight = FALSE, clickstateLeft = FALSE;


	// Sounds
	int slash = LoadSoundMem("Resource\\slash.mp3");
	int shot = LoadSoundMem("Resource\\gunshot.mp3");
	int song = LoadSoundMem("Resource\\titlebmg.mp3");
	ChangeVolumeSoundMem(150, song);

	scenestate = ajkgames;

	while (true) {
		GetHitKeyStateAll(keys);
		ClearDrawScreen();
		srand(time(NULL));
		
		//---------  ここからプログラムを記述  ----------//

		spaceflag = buttonFlag(keys[KEY_INPUT_SPACE], prevkeys[KEY_INPUT_SPACE]);
		slashflag = buttonFlag(GetMouseInput() & MOUSE_INPUT_LEFT, clickstateLeft);
		shootflag = buttonFlag(GetMouseInput() & MOUSE_INPUT_RIGHT, clickstateRight);
		if (shootflag && !shotflag) shotflag = TRUE, PlaySoundMem(shot, DX_PLAYTYPE_BACK);;
		if (shotflag) {
			shoottimer++;
			if (shoottimer >= shootcd) {
				shoottimer = 0;
				shotflag = FALSE;
			}
		}
		

		upflag = heldFlag(keys[KEY_INPUT_W]);
		downflag = heldFlag(keys[KEY_INPUT_S]);
		leftflag = heldFlag(keys[KEY_INPUT_A]);
		rightflag = heldFlag(keys[KEY_INPUT_D]);

		returnflag = buttonFlag(keys[KEY_INPUT_R], prevkeys[KEY_INPUT_R]);

		if (returnflag) scenestate = mainmenu;

		if (shotflag) {
			upflag = FALSE;
			downflag = FALSE;
			leftflag = FALSE;
			rightflag = FALSE;
		}

		int moveflag = FALSE;
		if (upflag || downflag || leftflag || rightflag) moveflag = TRUE;
		else moveflag = FALSE;

		switch (scenestate) {
		case ajkgames:
			ajikugames.DrawLogo(ajikugames.index, ajikugames.framecount, scenestate, frame, 110);
			break;
		case mainmenu:
			DrawBG(title, 0, 0, 0, 0);
			if (spaceflag) {
				scenestate = gameplay;
				gamestate = tutorial;
				dead1 = FALSE;
				dead2 = FALSE;
			}
			break;
		case gameplay:
			//if (!CheckSoundMem(song)) PlaySoundMem(song, DX_PLAYTYPE_BACK);
			switch (gamestate) {
			
			case tutorial:
				waterframecount++;
				if (waterframecount >= 10) {
					waterframecount = 0;
					waterindex++;
					if (waterindex >= 2) waterindex = 0;
				}
				for (int y = 0; y < 101; y++) {
					for (int x = 0; x < 12; x++) {
						DrawStatic(x * 64 * 5 - cameraX, y * 64 - cameraY, water[waterindex]);
					}
				}
				enemyshipY -= 10;
				if (enemyshipY <= WIN_HEIGHT / 2) enemyshipY = WIN_HEIGHT / 2;
				GetMousePoint(&mouseX, &mouseY);
				mouseMapX = mouseX + cameraX;
				mouseMapY = mouseY + cameraY;
				pirate.dir = check8Direction(mouseMapX, mouseMapY, pirate.x, pirate.y, pirate.size);
				// Actions
				if (slashflag && !slashEntity.aliveflag) spawnOnPlayer(pirate.x, pirate.y, slashEntity.x, slashEntity.y, slashEntity.dir, pirate.dir, slashEntity.prevX, slashEntity.prevY, slashEntity.aliveflag), PlaySoundMem(slash, DX_PLAYTYPE_BACK);;
				if (slashEntity.aliveflag) Slash(slashEntity.x, slashEntity.y, slashEntity.aliveflag, slashEntity.dir, slashEntity.prevX, slashEntity.prevY);

				// Draw
				DrawStatic(350, WIN_HEIGHT / 2, smallship);
				DrawStatic(800, enemyshipY, smallship);
				
				cameraX = 0;
				cameraY = 0;
				pirate.x = 450;
				pirate.y = 520;

				if (shotflag && checkHit(mouseX, mouseY, 860, enemyshipY - 120, 3, 64)) dead2 = TRUE;
				if (slashEntity.aliveflag && checkHit(slashEntity.x, slashEntity.y, 720, enemyshipY + 120, 25, 64)) dead1 = TRUE;

				if (!dead1) DrawStatic(720, enemyshipY + 120, scimitarsailorimg[0]);
				if (!dead2) DrawStatic(860, enemyshipY - 120, scimitarsailorimg[0]);

				if (dead1) DrawStatic(720+4, enemyshipY + 120 + 4, deadscimitar);
				if (dead2) DrawStatic(860+4, enemyshipY - 120 + 4, deadscimitar);

				// Player Animations
				if (!slashEntity.aliveflag && !shotflag) {
					switch (pirate.dir) {
					case up:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[4]);
						break;
					case down:
					case down_right:
					case down_left:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[0]);
						break;
					case left:
					case up_left:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[8]);
						break;
					case right:
					case up_right:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[12]);
						break;
					}

				}
				else if (slashEntity.aliveflag && !shootflag) {
					switch (pirate.dir) {
					case up:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 20, 4, TRUE, cameraX, cameraY);
						break;
					case down:
					case down_right:
					case down_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 16, 4, TRUE, cameraX, cameraY);
						break;
					case left:
					case up_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 24, 4, TRUE, cameraX, cameraY);
						break;
					case right:
					case up_right:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 28, 4, TRUE, cameraX, cameraY);
						break;
					}
				}
				else if (shotflag) {
					switch (pirate.dir) {
					case up:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 36, 4, TRUE, cameraX, cameraY);
						break;
					case down:
					case down_right:
					case down_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 32, 4, TRUE, cameraX, cameraY);
						break;
					case left:
					case up_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 40, 4, TRUE, cameraX, cameraY);
						break;
					case right:
					case up_right:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 44, 4, TRUE, cameraX, cameraY);
						break;
					}
				}

				if (slashEntity.aliveflag) {
					switch (slashEntity.dir) {
					case up:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, -1.57);
						break;
					case down:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 1.57);
						break;
					case down_right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 0.8);
						break;
					case down_left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 - 0.8);
						break;
					case left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14);
						break;
					case up_left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 + 0.8);
						break;
					case right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 0);
						break;
					case up_right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, -0.8);
						break;
					}
				}

				SetFontSize(25);
				DrawFormatString(900, 10, GetColor(0, 0, 0), "1. 左クリックでシミターを使う\n\n2. 右クリックでフリントロックを打つ\n\n3. WASDで動く\n\n\n [SPACE] ゲームスタート");

				if (shotflag) DrawStatic(mouseX, mouseY, reticle);
				else DrawStatic(mouseX, mouseY, yellreticle);

				if (dead1 && dead2 && spaceflag) {
					gamestate = load;
					frame = 0;
					pirate.HP = 40;
					boss.HP = 600;
				} 

				break;

			case load:
				// RESET VARIABLES SUCH AS CAMERA HERE
				frame++;
				cameraX = 700 - WIN_WIDTH / 2;
				cameraY = 3770 - WIN_HEIGHT / 2;
				pirate.HP = 40;
				boss.HP = 600;
				bossState = following;
				boss.x = 2300, boss.y = 2180;
				pirate.x = 700, pirate.y = 3770; // Starting X and Y
				for (int x = 0; x < 20; x++) {
					swordSailor[x].x = 0;
					swordSailor[x].y = 0;
					swordSailor[x].aliveflag = FALSE;
					swordSailor[x].HP = 5;
				}
				if (frame >= 60) gamestate = game;
				break;
			
			case game:
				if (!CheckSoundMem(song)) PlaySoundMem(song, DX_PLAYTYPE_BACK);
				waterframecount++;
				if (waterframecount >= 10) {
					waterframecount = 0;
					waterindex++;
					if (waterindex >= 2) waterindex = 0;
				}
				for (int y = 0; y < 101; y++) {
					for (int x = 0; x < 12; x++) {
						DrawStatic(x * 64 * 5 - cameraX, y * 64 - cameraY, water[waterindex]);
					}
				}
				pirate.prevX = pirate.x;
				pirate.prevY = pirate.y;
				basicMovement(pirate.x, pirate.y, upflag, downflag, leftflag, rightflag, pirate.speed); // Pirate Movement
				GetMousePoint(&mouseX, &mouseY);
				mouseMapX = mouseX + cameraX;
				mouseMapY = mouseY + cameraY;
				pirate.dir = check8Direction(mouseMapX, mouseMapY, pirate.x, pirate.y, pirate.size);
				spawntimer++;
				for (int x = 0; x < 20; x++) {
					swordSailor[x].prevX = swordSailor[x].x;
					swordSailor[x].prevY = swordSailor[x].y;

					if (spawntimer >= spawnfrecuency && !swordSailor[x].aliveflag) {
						spawntimer = 0;
						SpawnEntity(1630, 1960, swordSailor[x].x, swordSailor[x].y, swordSailor[x].aliveflag, enemiesalive, swordSailor[x].HP, 10);
					}

					if (swordSailor[x].aliveflag) {
						SimpleFollowScript(swordSailor[x].x, swordSailor[x].y, pirate.x, pirate.y, 4);
						swordSailor[x].attacktimer++;
						swordSailor[x].dir = check8Direction(pirate.x, pirate.y, swordSailor[x].x, swordSailor[x].y, 32);
						if (swordSailor[x].attacktimer >= 120) {
							swordSailor[x].attacktimer = 0;
							PlaySoundMem(slash, DX_PLAYTYPE_BACK);
							if (!enemySlash[x].aliveflag) spawnOnPlayer(swordSailor[x].x, swordSailor[x].y, enemySlash[x].x, enemySlash[x].y, enemySlash[x].dir, swordSailor[x].dir, enemySlash[x].prevX, enemySlash[x].prevY, enemySlash[x].aliveflag);
						}
						if (enemySlash[x].aliveflag) {
							Slash(enemySlash[x].x, enemySlash[x].y, enemySlash[x].aliveflag, enemySlash[x].dir, enemySlash[x].prevX, enemySlash[x].prevY);
							if (checkHit(pirate.x, pirate.y, enemySlash[x].x, enemySlash[x].y, pirate.size, enemySlash[x].size)) pirate.HP--;
						}
						
						// Hitboxes
						circleHitBox(swordSailor[x].x, swordSailor[x].y, pirate.x, pirate.y, pirate.size*3, swordSailor[x].size);
						if (slashEntity.aliveflag && checkHit(slashEntity.x, slashEntity.y, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, slashEntity.size)) swordSailor[x].HP--;
						if (swordSailor[x].HP <= 0) swordSailor[x].aliveflag = FALSE;
						for (int y = 0; y < 20; y++) if (y != x && swordSailor[y].aliveflag) circleHitBox(swordSailor[x].x, swordSailor[x].y, swordSailor[y].x, swordSailor[y].y, swordSailor[x].size, swordSailor[y].size);

						if (shootflag) {
							if (checkHit(mouseMapX, mouseMapY, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, 2)) swordSailor[x].HP -= 5;
						}

						squareHitBox(500, 2850, 125, 3150, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(2635, 2850, 125, 3150, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(550, 1890, 770 - 550, 3330 - 1890, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(710, 5870, 950 - 710, 5950 - 5870, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(600, 5970, 2700 - 600, 6050 - 5970, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(1030, 5280, 2240 - 1030, 5370 - 5280, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(600, 3250, 300, 3480 - 3250, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(2070, 5370, 100, 5450 - 5370, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(1600, 5610, 260, 220, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
						squareHitBox(2380, 3790, 2660 - 2380, 4030 - 3790, swordSailor[x].x, swordSailor[x].y, swordSailor[x].size, swordSailor[x].prevX, swordSailor[x].prevY);
					}
				}

				// Actions
				if (slashflag && !slashEntity.aliveflag) spawnOnPlayer(pirate.x, pirate.y, slashEntity.x, slashEntity.y, slashEntity.dir, pirate.dir, slashEntity.prevX, slashEntity.prevY, slashEntity.aliveflag), PlaySoundMem(slash, DX_PLAYTYPE_BACK);;
				if (slashEntity.aliveflag) Slash(slashEntity.x, slashEntity.y, slashEntity.aliveflag, slashEntity.dir, slashEntity.prevX, slashEntity.prevY);
				
				// Boss
				boss.prevX = boss.x;
				boss.prevY = boss.y;

				switch (bossState) {
				case following:
					
					break;
				case aiming:
					break;
				}

				

				// Hitboxes
				circleHitBox(pirate.x, pirate.y, boss.x, boss.y, pirate.size, boss.size);
				if (slashEntity.aliveflag && checkHit(slashEntity.x, slashEntity.y, boss.x, boss.y, boss.size, boss.size)) boss.HP--;
				for (int y = 0; y < 20; y++) circleHitBox(swordSailor[y].x, swordSailor[y].y, boss.x, boss.y, boss.size, swordSailor[y].size);

				if (shootflag) {
					if (checkHit(mouseMapX, mouseMapY, boss.x, boss.y, boss.size, 2)) boss.HP -= 5;
				}

				squareHitBox(500, 2850, 125, 3150, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(2635, 2850, 125, 3150, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(550, 1890, 770 - 550, 3330 - 1890, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(710, 5870, 950 - 710, 5950 - 5870, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(600, 5970, 2700 - 600, 6050 - 5970, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(1030, 5280, 2240 - 1030, 5370 - 5280, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(600, 3250, 300, 3480 - 3250, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(2070, 5370, 100, 5450 - 5370, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(1600, 5610, 260, 220, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				squareHitBox(2380, 3790, 2660 - 2380, 4030 - 3790, boss.x, boss.y, boss.size, boss.prevX, boss.prevY);
				
				

				// Ship Hitboxes
				squareHitBox(700, 1500, 2000, 500, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(2500, 2000, 150, 810, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(500, 2850, 125, 3150, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(2635, 2850, 125, 3150, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(550, 1890, 770-550, 3330-1890, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(710, 5870, 950-710, 5950 - 5870, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(600, 5970, 2700-600, 6050 - 5970, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(1030, 5280, 2240-1030, 5370-5280, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(600, 3250, 300, 3480 - 3250, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(2070, 5370, 100, 5450 - 5370, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(1600, 5610, 260, 220, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				squareHitBox(2380, 3790, 2660-2380, 4030-3790, pirate.x, pirate.y, pirate.size, pirate.prevX, pirate.prevY);
				circleHitBox(pirate.x, pirate.y, 1230, 5400, pirate.size, pirate.size);
				circleHitBox(pirate.x, pirate.y, 1330, 5400, pirate.size, pirate.size);
				circleHitBox(pirate.x, pirate.y, 1570, 4940, pirate.size, pirate.size);
				circleHitBox(pirate.x, pirate.y, 1520, 4890, pirate.size, pirate.size);
				circleHitBox(pirate.x, pirate.y, 1620, 3300, pirate.size, 94);
				circleHitBox(pirate.x, pirate.y, 1620, 4830, pirate.size, 94);


				// All DrawXX
				movecam(cameraX, cameraY, pirate.x, pirate.y, pirate.speed, pirate.speed); // Moves the camera
				DrawBG(shipbody, 0, 0, cameraX, cameraY); // Draw BG

				// Player Animations
				if (!moveflag && !slashEntity.aliveflag && !shotflag) {
					switch (pirate.dir) {
					case up:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[4]);
						break;
					case down:
					case down_right:
					case down_left:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[0]);
						break;
					case left:
					case up_left:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[8]);
						break;
					case right:
					case up_right:
						DrawStatic(pirate.x - cameraX, pirate.y - cameraY, pirateimg[12]);
						break;
					}
					
				}
				else if (moveflag && !slashEntity.aliveflag && !shootflag) {
					switch (pirate.dir) {
					case up:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 4, 4, TRUE, cameraX, cameraY);
						break;
					case down:
					case down_right:
					case down_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 0, 4, TRUE, cameraX, cameraY);
						break;
					case left:
					case up_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 8, 4, TRUE, cameraX, cameraY);
						break;
					case right:
					case up_right:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 12, 4, TRUE, cameraX, cameraY);
						break;
					}
				}
				else if (slashEntity.aliveflag && !shootflag) {
					switch (pirate.dir) {
					case up:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 20, 4, TRUE, cameraX, cameraY);
						break;
					case down:
					case down_right:
					case down_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 16, 4, TRUE, cameraX, cameraY);
						break;
					case left:
					case up_left:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 24, 4, TRUE, cameraX, cameraY);
						break;
					case right:
					case up_right:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 28, 4, TRUE, cameraX, cameraY);
						break;
					}
				}
				else if (shotflag) {
					switch (pirate.dir) {
					case up:
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 36, 4, TRUE, cameraX, cameraY);
						break;																								   
					case down:																								   
					case down_right:																						   
					case down_left:																							   
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 32, 4, TRUE, cameraX, cameraY);
						break;																								   
					case left:																								   
					case up_left:																							   
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 40, 4, TRUE, cameraX, cameraY);
						break;																								   
					case right:																								   
					case up_right:																							   
						DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 44, 4, TRUE, cameraX, cameraY);
						break;
					}
				}

			
				
				// Boss
				bossTimer++;
				switch (bossState) {
				case following:

					SimpleFollowScript(boss.x, boss.y, pirate.x, pirate.y, 1);
					boss.attacktimer++;
					boss.dir = check8Direction(pirate.x, pirate.y, boss.x, boss.y, 64);
					if (boss.attacktimer >= 200) {
						boss.attacktimer = 0;
						PlaySoundMem(slash, DX_PLAYTYPE_BACK);
						if (!bossSlash.aliveflag) spawnOnPlayer(boss.x, boss.y, bossSlash.x, bossSlash.y, bossSlash.dir, boss.dir, bossSlash.prevX, bossSlash.prevY, bossSlash.aliveflag);
					}
					if (bossSlash.aliveflag) {
						Slash(bossSlash.x, bossSlash.y, bossSlash.aliveflag, bossSlash.dir, bossSlash.prevX, bossSlash.prevY);
						if (checkHit(pirate.x, pirate.y, bossSlash.x, bossSlash.y, pirate.size, bossSlash.size)) pirate.HP--;
					}

					if (!bossSlash.aliveflag) {
						switch (boss.dir) {
						case up:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 4, 4, TRUE, cameraX, cameraY);
							break;
						case down:
						case down_right:
						case down_left:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 0, 4, TRUE, cameraX, cameraY);
							break;
						case left:
						case up_left:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 8, 4, TRUE, cameraX, cameraY);
							break;
						case right:
						case up_right:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 12, 4, TRUE, cameraX, cameraY);
							break;
						}
					}
					else if (bossSlash.aliveflag) {
						switch (boss.dir) {
						case up:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 20, 4, TRUE, cameraX, cameraY);
							break;
						case down:
						case down_right:
						case down_left:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 16, 4, TRUE, cameraX, cameraY);
							break;
						case left:
						case up_left:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 24, 4, TRUE, cameraX, cameraY);
							break;
						case right:
						case up_right:
							DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 28, 4, TRUE, cameraX, cameraY);
							break;
						}
					}

					if (bossTimer >= 900) bossState = aiming, bossTimer = 0, recX = boss.x, recY = boss.y;
					break;
				case aiming:
					boss.dir = check8Direction(pirate.x, pirate.y, boss.x, boss.y, 64);
					switch (boss.dir) {
						case up:
							DrawStatic(boss.x - cameraX, boss.y - cameraY, bossimg[4]);
							break;
						case down:
						case down_right:
						case down_left:
							DrawStatic(boss.x - cameraX, boss.y - cameraY, bossimg[0]);
							break;
						case left:
						case up_left:
							DrawStatic(boss.x - cameraX, boss.y - cameraY, bossimg[8]);
							break;
						case right:
						case up_right:
							DrawStatic(boss.x - cameraX, boss.y - cameraY, bossimg[12]);
							break;
					}

					if (bossTimer >= 300) bossState = shooting, bossTimer = 0;
					SimpleFollowScript(recX, recY, pirate.x, pirate.y, pirate.speed-4);
					DrawStatic(recX - cameraX, recY - cameraY, bossreticle);
					break;
				case shooting:
					switch (boss.dir) {
					case up:
						DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 36, 4, TRUE, cameraX, cameraY);
						break;
					case down:
					case down_right:
					case down_left:
						DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 32, 4, TRUE, cameraX, cameraY);
						break;
					case left:
					case up_left:
						DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 40, 4, TRUE, cameraX, cameraY);
						break;
					case right:
					case up_right:
						DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 44, 4, TRUE, cameraX, cameraY);
						break;
					}
					if (bossTimer == 1) PlaySoundMem(shot, DX_PLAYTYPE_BACK);
					if (bossTimer == 1 && checkHit(pirate.x, pirate.y, recX, recY, pirate.size, pirate.size)) pirate.HP -= 10;
					if (bossTimer >= 24) bossState = following, bossTimer = 0;
					DrawStatic(recX - cameraX, recY - cameraY, bossrecskull);
					break;
				}
				if (bossSlash.aliveflag) {
					switch (bossSlash.dir) {
					case up:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, -1.57);
						break;
					case down:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 1.57);
						break;
					case down_right:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 0.8);
						break;
					case down_left:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 - 0.8);
						break;
					case left:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14);
						break;
					case up_left:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 + 0.8);
						break;
					case right:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, 0);
						break;
					case up_right:
						DrawRotaAnimation(bossSlash.x, bossSlash.y, bigslash, bossSlash.index, bossSlash.framecount, bossSlash.framerate, 0, 4, FALSE, cameraX, cameraY, -0.8);
						break;
					}
				}



				for (int x = 0; x < 20; x++) {
					if (swordSailor[x].aliveflag) {

						if (!enemySlash[x].aliveflag) {
							switch (swordSailor[x].dir) {
							case up:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 4, 4, TRUE, cameraX, cameraY);
								break;
							case down:
							case down_right:
							case down_left:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 0, 4, TRUE, cameraX, cameraY);
								break;
							case left:
							case up_left:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 8, 4, TRUE, cameraX, cameraY);
								break;
							case right:
							case up_right:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 12, 4, TRUE, cameraX, cameraY);
								break;
							}
						}
						else if (enemySlash[x].aliveflag) {
							switch (swordSailor[x].dir) {
							case up:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 20, 4, TRUE, cameraX, cameraY);
								break;
							case down:
							case down_right:
							case down_left:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 16, 4, TRUE, cameraX, cameraY);
								break;
							case left:
							case up_left:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 24, 4, TRUE, cameraX, cameraY);
								break;
							case right:
							case up_right:
								DrawAnimation(swordSailor[x].x, swordSailor[x].y, scimitarsailorimg, swordSailor[x].index, swordSailor[x].framecount, swordSailor[x].framerate, 28, 4, TRUE, cameraX, cameraY);
								break;
							}
						}


						if (enemySlash[x].aliveflag) {
							switch (enemySlash[x].dir) {
							case up:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, -1.57);
								break;
							case down:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 1.57);
								break;
							case down_right:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 0.8);
								break;
							case down_left:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 - 0.8);
								break;
							case left:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 3.14);
								break;
							case up_left:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 + 0.8);
								break;
							case right:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, 0);
								break;
							case up_right:
								DrawRotaAnimation(enemySlash[x].x, enemySlash[x].y, redslash, enemySlash[x].index, enemySlash[x].framecount, enemySlash[x].framerate, 0, 4, FALSE, cameraX, cameraY, -0.8);
								break;
							}
						}
					}
					else if (!swordSailor[x].aliveflag && swordSailor[x].x != 1630 && swordSailor[x].y != 1960) DrawRotaGraph(swordSailor[x].x - cameraX, swordSailor[x].y - cameraY, 1, -3.14/2, deadscimitar, true);
				}


				if (slashEntity.aliveflag) {
					switch (slashEntity.dir) {
					case up:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, -1.57);
						break;
					case down:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 1.57);
						break;
					case down_right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 0.8);
						break;
					case down_left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 - 0.8);
						break;
					case left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14);
						break;
					case up_left:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 3.14 + 0.8);
						break;
					case right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, 0);
						break;
					case up_right:
						DrawRotaAnimation(slashEntity.x, slashEntity.y, blueslash, slashEntity.index, slashEntity.framecount, slashEntity.framerate, 0, 4, FALSE, cameraX, cameraY, -0.8);
						break;
					}
				}
				DrawBG(shipdetail, 0, 0, cameraX, cameraY); // Draw BG
				//DrawFormatString(10, 10, GetColor(255, 255, 255), "%d", bossState);

				SideLoadBar(17 * 4, 16, 335 * 4, 44, boss.HP, 600, 255, 60, 60);
				SideLoadBar(17 * 4, 177*4, 79 * 4, 56, pirate.HP, 40, 60, 255, 60);
				DrawBG(HUD, 0, 0, 0, 0);

				if (shotflag) DrawStatic(mouseX, mouseY, reticle);
				else DrawStatic(mouseX, mouseY, yellreticle);
				
				if (pirate.HP <= 0) gamestate = fail;
				if (boss.HP <= 0) gamestate = finish;
				
				break;
			
			case finish:
				if (boss.x - cameraX <= WIN_WIDTH / 2) DrawFormatString(boss.x - cameraX + 100, boss.y - cameraY, GetColor(255, 255, 255), "[SPACE] メインメニュー");
				if (boss.x - cameraX > WIN_WIDTH / 2) DrawFormatString(boss.x - cameraX - 400, boss.y - cameraY, GetColor(255, 255, 255), "[SPACE] メインメニュー");
				DrawAnimation(boss.x, boss.y, bossimg, boss.index, boss.framecount, boss.framerate, 48, 7, FALSE, cameraX, cameraY);
				DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 72, 6, TRUE, cameraX, cameraY);
				if (spaceflag) scenestate = mainmenu;
				break;
			case fail:
				if (pirate.x - cameraX <= WIN_WIDTH / 2) DrawFormatString(pirate.x - cameraX + 100, pirate.y - cameraY, GetColor(255, 255, 255), "[SPACE] リセット");
				if (pirate.x - cameraX > WIN_WIDTH / 2) DrawFormatString(pirate.x - cameraX - 400, pirate.y - cameraY, GetColor(255, 255, 255), "[SPACE] リセット");
				DrawAnimation(pirate.x, pirate.y, pirateimg, pirate.index, pirate.framecount, pirate.framerate, 64, 7, FALSE, cameraX, cameraY);
				if (spaceflag) gamestate = load;
				break;
			}
			break;
		}

		
		
		//---------  ここまでにプログラムを記述  ---------//
		frame++;
		GetHitKeyStateAll(prevkeys);
		clickstateLeft = GetMouseInput() & MOUSE_INPUT_LEFT;
		clickstateRight = GetMouseInput() & MOUSE_INPUT_RIGHT;
		ScreenFlip();
		WaitTimer(20);
		if (ProcessMessage() == -1) {
			break;
		}
	}

	DxLib_End();
	return 0;
}
