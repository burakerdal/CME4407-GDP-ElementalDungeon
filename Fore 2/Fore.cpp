//-----------------------------------------------------------------
// Fore 2 Application
// C++ Source - Fore.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Fore.h"

//-----------------------------------------------------------------
// Game Engine Functions
//-----------------------------------------------------------------
void ResetLevel() {
	for (int i = 0; i < 100; i++)
	{

		afx[i] = NULL;
		aax[i] = NULL;
		aay[i] = NULL;
		aadir[i] = NULL;
		afy[i] = NULL;
		afdir[i] = NULL;
		aCaught[i] = NULL;
		aCaughtX[i] = NULL;
		aCaughtY[i] = NULL;
		afreeway[i] = NULL;
		afireDie[i] = NULL;
		enemyType[i] = NULL;

	}
	dynamite_counter = 50;
	dynamiteActive = false;

}

void CreateEnemies(int num)
{
	// 0 fire 1 air 2 water
	for (int i = 0; i < num; i++) {
		if (i % 10 == 9)
		{
			enemyType[i] = 1;
		}
		else {
			enemyType[i] = 0;
		}

	}
}

int distance(coord *c1, coord *c2) {
	int dx, dy;
	dx = abs(c1->x - c2->x);
	dy = abs(c1->y - c2->y);

	return dx + dy;
}

int findpathBFS(coord *c1, coord *c2, int **map, int maxrow, int maxcol, int empty, coord **path) {// c1:start c2: end 

	queue < coord * > alt;
	coord *c;

	for (int y = 0; y < maxrow; y++) {
		for (int x = 0; x < maxcol; x++) {
			map2[y][x] = map[y][x];

		}
	}

	// beginning

	alt.push(c1);
	while (!alt.empty()) {
		c = (coord *)alt.front();
		alt.pop();
		if (map2[c->y][c->x] != empty) continue;
		map2[c->y][c->x] = 1;

		if (distance(c, c2) <= 1) {// target found
			int i = 0;
			while (c->prev != NULL) {
				path[i] = c;

				c = c->prev;
				i++;
			}
			return i;
		}

		if (c->x + 1 < maxcol && map2[c->y][c->x + 1] == empty) alt.push(new coord(c->x + 1, c->y, c->dist + 1, c));
		if (c->x - 1 >= 0 && map2[c->y][c->x - 1] == empty) alt.push(new coord(c->x - 1, c->y, c->dist + 1, c));
		if (c->y + 1 < maxrow && map2[c->y + 1][c->x] == empty) alt.push(new coord(c->x, c->y + 1, c->dist + 1, c));
		if (c->y - 1 >= 0 && map2[c->y - 1][c->x] == empty) alt.push(new coord(c->x, c->y - 1, c->dist + 1, c));
	}
	return -1;
}

BOOL GameInitialize(HINSTANCE hInstance)
{
	// Create the game engine
	_pGame = new GameEngine(hInstance, TEXT("Archie"), TEXT("Archie"), IDI_FORE, IDI_FORE_SM, 23 * 50, 15 * 50);
	if (_pGame == NULL)
		return FALSE;

	// Set the frame rate
	_pGame->SetFrameRate(30);

	// Store the instance handle
	_hInstance = hInstance;

	return TRUE;
}

void NextLevel() {
	level++;
	ResetLevel();
	// Clear the sprites
	_pGame->CleanupSprites();

	totalbomb = max(5, initialBomb - level + 1);
	totalGrenade = max(0, initialGrenade - (level - 1) / 5);
	afnum = level*2 + level/5;
	CreateEnemies(afnum);

	bombPerKill = max(1, 5 - ((level - 1) / 5));
	secForBomb = min(80, secForBomb++);
	gemsToWin = min(4, level / 3 + 1);
	startTime = 5068;
	RECT    rcBounds2 = { 0, 0, 23 * 50, 15 * 50 };
	small_air_gem_spr = new Sprite(small_air_gem, rcBounds2, BA_DIE);
	small_air_gem_spr->SetPosition(1070, 210);
	small_air_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_air_gem_spr);

	small_earth_gem_spr = new Sprite(small_earth_gem, rcBounds2, BA_DIE);
	small_earth_gem_spr->SetPosition(1070, 260);
	small_earth_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_earth_gem_spr);

	small_fire_gem_spr = new Sprite(small_fire_gem, rcBounds2, BA_DIE);
	small_fire_gem_spr->SetPosition(1070, 310);
	small_fire_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_fire_gem_spr);

	small_water_gem_spr = new Sprite(small_water_gem, rcBounds2, BA_DIE);
	small_water_gem_spr->SetPosition(1070, 360);
	small_water_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_water_gem_spr);


	playerspr = new Sprite(playerbmp_right);
	playerspr->SetPosition(50, 50);
	playerspr->SetVelocity(0, 0);
	_pGame->AddSprite(playerspr);

	path = new coord*[1000];

	path2 = new coord*[1000];

	map = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map[i] = new int[maxcol];

	map2 = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map2[i] = new int[maxcol];

	// base labyrinth
	for (int y = 0; y < maxrow; y++) {
		for (int x = 0; x < maxcol; x++) {
			map[y][x] = 0;    // 0:empty   100:base wall   101:additional wall
			if (y == 0 || y == maxrow - 1 || x == 0 || x == maxcol - 1) map[y][x] = 100;
			if (y % 2 == 0 && x % 2 == 0) map[y][x] = 100;
		}
	}
	// additional walls
	for (wall = 0; wall < maxwall; wall++) {
		int x, y, x1, y1, x2, y2;
		bool wallok;

		wallok = false;
		do {
			x = rand() % maxcol;
			y = rand() % maxrow;

			if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
				if ((x % 2 == 1 && y % 2 == 0)) {
					x1 = x2 = x;
					y1 = y - 1;
					y2 = y + 1;
				}
				if ((x % 2 == 0 && y % 2 == 1)) {
					y1 = y2 = y;
					x1 = x - 1;
					x2 = x + 1;
				}



				if (map[y][x] == 0) { // found

					map[y][x] = 101;
					wallok = true;

					c1 = new coord(x1, y1);
					c2 = new coord(x2, y2);

					if (findpathBFS(c1, c2, map, maxrow, maxcol, 0, path) < 0) {
						map[y][x] = 0;
						wallok = false;
					}

				}
			}
		} while (wallok == false);
	}




	RECT    rcBounds3 = { 50, 50, 20 * 50, 14 * 50 };

	for (int a = 0; a < afnum; a++) {
		while (true) {
			int bol = rand() % 3;
			if (bol == 0) {
				fy = rand() % maxrow;
				fx = rand() % maxcol + 4;
			}
			if (bol == 1) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol;
			}
			if (bol == 2) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol + 4;
			}
			afx[a] = fx;
			afy[a] = fy;
			if (fy < maxrow && fx < maxcol && map[fy][fx] == 0) break;
		};
		if (enemyType[a] == 0) {
			afire_left_spr[a] = new Sprite(fire_left_bmp1, rcBounds3, BA_BOUNCE);
		}
		else if (enemyType[a] == 1) {
			afire_left_spr[a] = new Sprite(air_right_bmp, rcBounds3, BA_BOUNCE);
		}
		else
		{
			afire_left_spr[a] = new Sprite(water_left_bmp, rcBounds3, BA_BOUNCE);
		}
		afire_left_spr[a]->SetPosition(fx * 50, fy * 50);
		afdir[a] = rand() % 4;
		_pGame->AddSprite(afire_left_spr[a]);
	}


	for (int i = 0; i < 4; i++) {
		gemx[i] = max(1, rand() % 20);
		gemy[i] = max(1, rand() % 14);


		while (map[gemy[i]][gemx[i]] != 101) {
			gemx[i] = max(1, rand() % 20);
			gemy[i] = max(1, rand() % 14);
		}
		map[gemy[i]][gemx[i]] = 102;
	}

	fire_gem_spr = new Sprite(fire_gem);
	fire_gem_spr->SetPosition(gemx[0] * 50, gemy[0] * 50);
	_pGame->AddSprite(fire_gem_spr);

	water_gem_spr = new Sprite(water_gem);
	water_gem_spr->SetPosition(gemx[1] * 50, gemy[1] * 50);
	_pGame->AddSprite(water_gem_spr);

	earth_gem_spr = new Sprite(earth_gem);
	earth_gem_spr->SetPosition(gemx[2] * 50, gemy[2] * 50);
	_pGame->AddSprite(earth_gem_spr);

	air_gem_spr = new Sprite(air_gem);
	air_gem_spr->SetPosition(gemx[3] * 50, gemy[3] * 50);
	_pGame->AddSprite(air_gem_spr);


	gempoint = 0;

	gameOver = false;

	win = false;
	lose = false;

	_pGame->PlayMIDISong(TEXT("HIP_HOP.mid"), TRUE);

	// bizim karakter ölmüþtü onu tekrar oluþtur
	// initial deðerleri sýfýrlar
	// arka plan müziði


}

void NewGame() {


	ResetLevel();
	gemsToWin = 1;
	CreateEnemies(afnum);
	level = 1;
	// Clear the sprites
	_pGame->CleanupSprites();

	RECT    rcBounds2 = { 0, 0, 23 * 50, 15 * 50 };
	small_air_gem_spr = new Sprite(small_air_gem, rcBounds2, BA_DIE);
	small_air_gem_spr->SetPosition(1070, 210);
	small_air_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_air_gem_spr);

	small_earth_gem_spr = new Sprite(small_earth_gem, rcBounds2, BA_DIE);
	small_earth_gem_spr->SetPosition(1070, 260);
	small_earth_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_earth_gem_spr);

	small_fire_gem_spr = new Sprite(small_fire_gem, rcBounds2, BA_DIE);
	small_fire_gem_spr->SetPosition(1070, 310);
	small_fire_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_fire_gem_spr);

	small_water_gem_spr = new Sprite(small_water_gem, rcBounds2, BA_DIE);
	small_water_gem_spr->SetPosition(1070, 360);
	small_water_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_water_gem_spr);

	playerspr = new Sprite(playerbmp_right);
	playerspr->SetPosition(50, 50);
	playerspr->SetVelocity(0, 0);
	_pGame->AddSprite(playerspr);

	afnum = 2;
	totalbomb = initialBomb;
	totalGrenade = 5;
	bombPerKill = 5;
	secForBomb = 20;
	bombAdded = false;

	startTime = 5068;

	path = new coord*[1000];

	path2 = new coord*[1000];

	map = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map[i] = new int[maxcol];

	map2 = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map2[i] = new int[maxcol];

	// base labyrinth
	for (int y = 0; y < maxrow; y++) {
		for (int x = 0; x < maxcol; x++) {
			map[y][x] = 0;    // 0:empty   100:base wall   101:additional wall
			if (y == 0 || y == maxrow - 1 || x == 0 || x == maxcol - 1) map[y][x] = 100;
			if (y % 2 == 0 && x % 2 == 0) map[y][x] = 100;
		}
	}
	// additional walls
	for (wall = 0; wall < maxwall; wall++) {
		int x, y, x1, y1, x2, y2;
		bool wallok;

		wallok = false;
		do {
			x = rand() % maxcol;
			y = rand() % maxrow;

			if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
				if ((x % 2 == 1 && y % 2 == 0)) {
					x1 = x2 = x;
					y1 = y - 1;
					y2 = y + 1;
				}
				if ((x % 2 == 0 && y % 2 == 1)) {
					y1 = y2 = y;
					x1 = x - 1;
					x2 = x + 1;
				}



				if (map[y][x] == 0) { // found

					map[y][x] = 101;
					wallok = true;

					c1 = new coord(x1, y1);
					c2 = new coord(x2, y2);

					if (findpathBFS(c1, c2, map, maxrow, maxcol, 0, path) < 0) {
						map[y][x] = 0;
						wallok = false;
					}

				}
			}
		} while (wallok == false);
	}




	RECT    rcBounds3 = { 50, 50, 20 * 50, 14 * 50 };

	for (int a = 0; a < afnum; a++) {
		while (true) {
			int bol = rand() % 3;
			if (bol == 0) {
				fy = rand() % maxrow;
				fx = rand() % maxcol + 4;
			}
			if (bol == 1) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol;
			}
			if (bol == 2) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol + 4;
			}
			afx[a] = fx;
			afy[a] = fy;
			if (fy < maxrow && fx < maxcol && map[fy][fx] == 0) break;
		};
		if (enemyType[a] == 0) {
			afire_left_spr[a] = new Sprite(fire_left_bmp1, rcBounds3, BA_BOUNCE);
		}
		else if (enemyType[a] == 1) {
			afire_left_spr[a] = new Sprite(air_right_bmp, rcBounds3, BA_BOUNCE);
		}
		else
		{
			afire_left_spr[a] = new Sprite(water_left_bmp, rcBounds3, BA_BOUNCE);
		}
		afire_left_spr[a]->SetPosition(fx * 50, fy * 50);
		afdir[a] = rand() % 4;
		_pGame->AddSprite(afire_left_spr[a]);
	}


	for (int i = 0; i < 4; i++) {
		gemx[i] = max(1, rand() % 20);
		gemy[i] = max(1, rand() % 14);


		while (map[gemy[i]][gemx[i]] != 101) {
			gemx[i] = max(1, rand() % 20);
			gemy[i] = max(1, rand() % 14);
		}
		map[gemy[i]][gemx[i]] = 102;
	}

	fire_gem_spr = new Sprite(fire_gem);
	fire_gem_spr->SetPosition(gemx[0] * 50, gemy[0] * 50);
	_pGame->AddSprite(fire_gem_spr);

	water_gem_spr = new Sprite(water_gem);
	water_gem_spr->SetPosition(gemx[1] * 50, gemy[1] * 50);
	_pGame->AddSprite(water_gem_spr);

	earth_gem_spr = new Sprite(earth_gem);
	earth_gem_spr->SetPosition(gemx[2] * 50, gemy[2] * 50);
	_pGame->AddSprite(earth_gem_spr);

	air_gem_spr = new Sprite(air_gem);
	air_gem_spr->SetPosition(gemx[3] * 50, gemy[3] * 50);
	_pGame->AddSprite(air_gem_spr);


	gempoint = 0;

	gameOver = false;

	win = false;
	lose = false;

	_pGame->PlayMIDISong(TEXT("HIP_HOP.mid"), TRUE);

	// bizim karakter ölmüþtü onu tekrar oluþtur
	// initial deðerleri sýfýrlar
	// arka plan müziði
}

void GameStart(HWND hWindow)
{
	// Seed the random number generator
	srand(GetTickCount());

	CreateEnemies(afnum);
	// Create the offscreen device context and bitmap
	_hOffscreenDC = CreateCompatibleDC(GetDC(hWindow));
	_hOffscreenBitmap = CreateCompatibleBitmap(GetDC(hWindow),
		_pGame->GetWidth(), _pGame->GetHeight());
	SelectObject(_hOffscreenDC, _hOffscreenBitmap);

	// Create and load the bitmaps
	HDC hDC = GetDC(hWindow);

	panel = new Bitmap(hDC, IDB_panel, _hInstance);

	win_bmp = new Bitmap(hDC, IDB_win, _hInstance);
	lose_bmp = new Bitmap(hDC, IDB_Lose, _hInstance);

	wall_bmp = new Bitmap(hDC, IDB_add_wall, _hInstance);
	ground_bmp = new Bitmap(hDC, IDB_ground, _hInstance);
	base_wall = new Bitmap(hDC, IDB_base_wall, _hInstance);

	playerbmp_right = new Bitmap(hDC, IDB_PLAYER_RIGHT, _hInstance);
	playerbmp_left = new Bitmap(hDC, IDB_PLAYER_LEFT, _hInstance);
	playerbmp_up = new Bitmap(hDC, IDB_PLAYER_UP, _hInstance);
	playerbmp_down = new Bitmap(hDC, IDB_PLAYER_DOWN, _hInstance);


	fire_left_bmp1 = new Bitmap(hDC, fire_left_bmp, _hInstance);
	air_right_bmp = new Bitmap(hDC, IDB_air_right, _hInstance);
	water_left_bmp = new Bitmap(hDC, IDB_water_left, _hInstance);

	//enemyKill = false;
	//fireDie = false;

	fire_gem = new Bitmap(hDC, IDB_fire_stone, _hInstance);
	earth_gem = new Bitmap(hDC, IDB_earth_stone, _hInstance);
	air_gem = new Bitmap(hDC, IDB_air_stone, _hInstance);
	water_gem = new Bitmap(hDC, IDB_water_stone, _hInstance);

	dynamite = new Bitmap(hDC, IDB_dynamite, _hInstance);

	_pLgExplosionBitmap = new Bitmap(hDC, IDB_LgExplosion, _hInstance);
	_pLgExplosionBitmap2 = new Bitmap(hDC, IDB_LgExplosion, _hInstance);
	dynamite2 = new Bitmap(hDC, IDB_dynamite2, _hInstance);
	dynamite3 = new Bitmap(hDC, IDB_dynamite3, _hInstance);

	small_dynamite = new Bitmap(hDC, IDB_small_dynamite, _hInstance);
	small_clock = new Bitmap(hDC, IDB_smallclock, _hInstance);
	small_grenade = new Bitmap(hDC, IDB_smallgrenade, _hInstance);

	small_air_gem = new Bitmap(hDC, IDB_air_stone, _hInstance);
	small_earth_gem = new Bitmap(hDC, IDB_earth_stone, _hInstance);
	small_fire_gem = new Bitmap(hDC, IDB_fire_stone, _hInstance);
	small_water_gem = new Bitmap(hDC, IDB_water_stone, _hInstance);

	RECT    rcBounds2 = { 0, 0, 23 * 50, 15 * 50 };
	small_air_gem_spr = new Sprite(small_air_gem, rcBounds2, BA_DIE);
	small_air_gem_spr->SetPosition(1070, 210);
	small_air_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_air_gem_spr);

	small_earth_gem_spr = new Sprite(small_earth_gem, rcBounds2, BA_DIE);
	small_earth_gem_spr->SetPosition(1070, 260);
	small_earth_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_earth_gem_spr);

	small_fire_gem_spr = new Sprite(small_fire_gem, rcBounds2, BA_DIE);
	small_fire_gem_spr->SetPosition(1070, 310);
	small_fire_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_fire_gem_spr);

	small_water_gem_spr = new Sprite(small_water_gem, rcBounds2, BA_DIE);
	small_water_gem_spr->SetPosition(1070, 360);
	small_water_gem_spr->SetHidden(true);

	_pGame->AddSprite(small_water_gem_spr);


	RECT    rcBounds = { 0, 0, 21 * 50, 15 * 50 };

	rc = { 0, 0, 21 * 50, 15 * 50 };


	playerspr = new Sprite(playerbmp_right, rcBounds, BA_DIE);
	playerspr->SetPosition(50, 50);
	playerspr->SetVelocity(0, 0);
	_pGame->AddSprite(playerspr);





	HPEN wpen = (HPEN)CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	SelectObject(hDC, wpen);
	HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));




	// allocating array

	path = new coord*[1000];

	path2 = new coord*[1000];

	map = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map[i] = new int[maxcol];

	map2 = new int*[maxrow];
	for (int i = 0; i < maxrow; i++) map2[i] = new int[maxcol];

	// base labyrinth
	for (int y = 0; y < maxrow; y++) {
		for (int x = 0; x < maxcol; x++) {
			map[y][x] = 0;    // 0:empty   100:base wall   101:additional wall
			if (y == 0 || y == maxrow - 1 || x == 0 || x == maxcol - 1) map[y][x] = 100;
			if (y % 2 == 0 && x % 2 == 0) map[y][x] = 100;
		}
	}
	// additional walls
	for (wall = 0; wall < maxwall; wall++) {
		int x, y, x1, y1, x2, y2;
		bool wallok;

		wallok = false;
		do {
			x = rand() % maxcol;
			y = rand() % maxrow;

			if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
				if ((x % 2 == 1 && y % 2 == 0)) {
					x1 = x2 = x;
					y1 = y - 1;
					y2 = y + 1;
				}
				if ((x % 2 == 0 && y % 2 == 1)) {
					y1 = y2 = y;
					x1 = x - 1;
					x2 = x + 1;
				}



				if (map[y][x] == 0) { // found

					map[y][x] = 101;
					wallok = true;

					c1 = new coord(x1, y1);
					c2 = new coord(x2, y2);

					if (findpathBFS(c1, c2, map, maxrow, maxcol, 0, path) < 0) {
						map[y][x] = 0;
						wallok = false;
					}

				}
			}
		} while (wallok == false);
	}




	RECT    rcBounds3 = { 50, 50, 20 * 50, 14 * 50 };

	for (int a = 0; a < afnum; a++) {
		while (true) {
			int bol = rand() % 3;
			if (bol == 0) {
				fy = rand() % maxrow;
				fx = rand() % maxcol + 4;
			}
			if (bol == 1) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol;
			}
			if (bol == 2) {
				fy = rand() % maxrow + 4;
				fx = rand() % maxcol + 4;
			}
			afx[a] = fx;
			afy[a] = fy;
			if (fy < maxrow && fx < maxcol && map[fy][fx] == 0) break;
		};
		if (enemyType[a] == 0) {
			afire_left_spr[a] = new Sprite(fire_left_bmp1, rcBounds3, BA_BOUNCE);
		}
		else if (enemyType[a] == 1) {
			afire_left_spr[a] = new Sprite(air_right_bmp, rcBounds3, BA_BOUNCE);
		}
		else
		{
			afire_left_spr[a] = new Sprite(water_left_bmp, rcBounds3, BA_BOUNCE);
		}
		afire_left_spr[a]->SetPosition(fx * 50, fy * 50);
		afdir[a] = rand() % 4;
		_pGame->AddSprite(afire_left_spr[a]);
	}


	for (int i = 0; i < 4; i++) {
		gemx[i] = max(1, rand() % 20);
		gemy[i] = max(1, rand() % 14);


		while (map[gemy[i]][gemx[i]] != 101) {
			gemx[i] = max(1, rand() % 20);
			gemy[i] = max(1, rand() % 14);
		}
		map[gemy[i]][gemx[i]] = 102;
	}

	fire_gem_spr = new Sprite(fire_gem, rcBounds, BA_DIE);
	fire_gem_spr->SetPosition(gemx[0] * 50, gemy[0] * 50);
	_pGame->AddSprite(fire_gem_spr);

	water_gem_spr = new Sprite(water_gem, rcBounds, BA_DIE);
	water_gem_spr->SetPosition(gemx[1] * 50, gemy[1] * 50);
	_pGame->AddSprite(water_gem_spr);

	earth_gem_spr = new Sprite(earth_gem, rcBounds, BA_DIE);
	earth_gem_spr->SetPosition(gemx[2] * 50, gemy[2] * 50);
	_pGame->AddSprite(earth_gem_spr);

	air_gem_spr = new Sprite(air_gem, rcBounds, BA_DIE);
	air_gem_spr->SetPosition(gemx[3] * 50, gemy[3] * 50);
	_pGame->AddSprite(air_gem_spr);


	gempoint = 0;

	gameOver = false;

	win = false;
	lose = false;

	_pGame->PlayMIDISong(TEXT("HIP_HOP.mid"), FALSE);
	
}

void GameEnd()
{
	// Cleanup the offscreen device context and bitmap
	DeleteObject(_hOffscreenBitmap);
	DeleteDC(_hOffscreenDC);

	// Cleanup the bitmaps
	delete ground_bmp;
	delete wall_bmp;
	

	// Cleanup the sprites
	_pGame->CleanupSprites();

	// Cleanup the game engine
	delete _pGame;
}

void GameActivate(HWND hWindow)
{
}

void GameDeactivate(HWND hWindow)
{
}

void GamePaint(HDC hDC)
{

	FillRect(hDC, &rc, brush);

	ground_bmp->Draw(hDC, 0, 0);

	panel->Draw(hDC, 1050, 0);
	small_dynamite->Draw(hDC, 1060, 110);
	small_clock->Draw(hDC, 1060, 60);
	small_grenade->Draw(hDC, 1060, 160);


	_pGame->DrawSprites(hDC);

	for (int y = 0; y < maxrow; y++) {
		for (int x = 0; x < maxcol; x++) {
			if (map[y][x] == 100) base_wall->Draw(hDC, x * 50, y * 50);
			if (map[y][x] == 101) wall_bmp->Draw(hDC, x * 50, y * 50);
			if (map[y][x] == 102) wall_bmp->Draw(hDC, x * 50, y * 50);

		}
	}

	TCHAR szText[64];
	TCHAR szText2[64];
	TCHAR szText3[64];
	TCHAR szText4[64];
	RECT  rect = { 460, 0, 1120, 50 };
	RECT  rect2 = { 460, 0, 1125, 150 };
	RECT  rect3 = { 460, 0, 1120, 250 };
	RECT  rect4 = { 460, 0, 1110, 350 };
	wsprintf(szText, " %d", totalbomb);
	wsprintf(szText2, " %d", startTime / 28);
	wsprintf(szText3, " %d", totalGrenade);
	wsprintf(szText4, "Level %d", level);
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(255, 255, 255));
	DrawText(hDC, szText, -1, &rect3, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	DrawText(hDC, szText3, -1, &rect4, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	DrawText(hDC, szText4, -1, &rect, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);
	if (startTime / 28 < 10) {
		SetTextColor(hDC, RGB(255, 255, 0));
	}
	DrawText(hDC, szText2, -1, &rect2, DT_SINGLELINE | DT_RIGHT | DT_VCENTER);

	if (gameOver) {
		_pGame->CloseMIDIPlayer();
		if (win) {
			
			PlaySound((LPCSTR)IDR_WAVE2, _hInstance, SND_ASYNC |
				SND_RESOURCE);
			win_bmp->Draw(hDC, 400, 250, TRUE);
		}
		if (lose) {
			PlaySound((LPCSTR)IDR_WAVE3, _hInstance, SND_ASYNC |
				SND_RESOURCE);
			lose_bmp->Draw(hDC, 400, 250, TRUE);
		}

	}



}

void GameCycle()
{
	if (!gameOver) {
		counter++;
		if (startTime > 0) {
			startTime--;
		}

		if ((startTime / 28) % secForBomb == 1 && !bombAdded)
		{
			totalbomb++;
			bombAdded = true;
		}
		if ((startTime / 28) % secForBomb == 0)
		{
			bombAdded = false;
		}

		// Update the sprites
		_pGame->UpdateSprites();

		for (int a = 0; a < afnum; a++) {
			fireDie = afireDie[a];
			fire_left_spr = afire_left_spr[a];
			fdir = afdir[a];
			bool caught = aCaught[a];
			int caughtX = aCaughtX[a];
			int caughtY = aCaughtY[a];
			bool freeway = afreeway[a];
			int velocity;

			if (enemyType[a] == 0)
			{
				velocity = 3;
			}
			else {
				velocity = 6;
			}

			//-----------------------------------------------------------------
			// Enemy random movement
			//-------------------------------------------------------------------
			if (!fireDie) {
				int fxx, fyy, fhar = 0;
				RECT frect;
				frect = fire_left_spr->GetPosition();
				fxx = frect.left;
				fyy = frect.top;
				if (fxx % 50 <= 4) fxx -= (fxx % 50);
				if (fxx % 50 >= 46) fxx += (fxx % 50);
				if (fyy % 50 <= 4) fyy -= (fyy % 50);
				if (fyy % 50 >= 46) fyy += (fyy % 50);

				if (fxx % 50 == 0 && fyy % 50 == 0) {
					fire_left_spr->SetVelocity(0, 0);
					fx = fxx / 50;
					fy = fyy / 50;
					while (true) {
						if (rand() % 100 < 20) fdir = rand() % 4;

						if (fdir == 0 && map[fy][fx + 1] == 0) {
							if (enemyType[a] == 0) {
								fire_left_spr->SetVelocity(velocity, 0);
							}
							else if (enemyType[a] == 1)
							{
								fire_left_spr->SetVelocity(velocity, 0);
							}
							
							fhar = 1;
						}

						else if (fdir == 1 && map[fy][fx - 1] == 0) {
							if (enemyType[a] == 0) {
								fire_left_spr->SetVelocity(-velocity, 0);
							}
							else if (enemyType[a] == 1)
							{
								fire_left_spr->SetVelocity(-velocity, 0);
							}
							
							fhar = 1;
						}

						else if (fdir == 2 && map[fy + 1][fx] == 0) {
							if (enemyType[a] == 0) {
								fire_left_spr->SetVelocity(0, velocity);
							}
							else if (enemyType[a] == 1)
							{
								fire_left_spr->SetVelocity(0, velocity);
							}
							
							fhar = 1;
						}

						else if (fdir == 3 && map[fy - 1][fx] == 0) {
							if (enemyType[a] == 0) {
								fire_left_spr->SetVelocity(0, -velocity);
							}
							else if (enemyType[a] == 1)
							{
								fire_left_spr->SetVelocity(0, -velocity);
							}
							
							fhar = 1;
						}

						if (fhar == 0) {
							fdir = rand() % 4;
						}




						if (fhar == 1) {
							c3 = new coord(bx, by);   // adam
							c4 = new coord(fx, fy);   // dusman
							freeway = true;
							
							if ((((bx == fx && abs(by - fy) <= 8) || (by == fy && abs(bx - fx) <= 8)) && enemyType[a] == 0) ||
								(((bx == fx && abs(by - fy) <= 15) || (by == fy && abs(bx - fx) <= 21)) && enemyType[a] == 2)) {
								caught = true;
								caughtX = bx;
								caughtY = by;
								if (c3->x == c4->x) {
									if (c3->y > c4->y) {

										for (int i = 1; i < c3->y - c4->y; i++) {
											if (map[c4->y + i][c4->x] > 99) {
												freeway = false;
											}
										}

										fdir = 2;
										if (map[fy + 1][fx] == 0 && freeway == true) {
											if (enemyType[a] == 0) {
												fire_left_spr->SetVelocity(0, 6);
											}
											else if (enemyType[a] == 2) {
												fire_left_spr->SetVelocity(0, 10);
											}
											fhar = 1;
										}
										if (fhar == 0) {
											
											fdir = rand() % 4;
										}
									}



									else {
										for (int i = 1; i < c4->y - c3->y; i++) {
											if (map[c4->y - i][c4->x] > 99) {
												freeway = false;
											}
										}
										fdir = 3;
										if (fdir == 3 && map[fy - 1][fx] == 0 && freeway == true) {
											if (enemyType[a] == 0) {
												fire_left_spr->SetVelocity(0, -6);
											}
											else if (enemyType[a] == 2) {
												fire_left_spr->SetVelocity(0, -10);
											}

											fhar = 1;
										}
										if (fhar == 0) {
											fdir = rand() % 4;
										}
									}

								}
								if (c3->y == c4->y) {
									if (c3->x > c4->x) {
										for (int i = 1; i < c3->x - c4->x; i++) {
											if (map[c4->y][c3->x - i] > 99) {
												freeway = false;
											}
										}
										fdir = 0;
										if (fdir == 0 && map[fy][fx + 1] == 0 && freeway == true) {
											if (enemyType[a] == 0) {
												fire_left_spr->SetVelocity(6, 0);
											}
											else if (enemyType[a] == 2) {
												fire_left_spr->SetVelocity(10, 0);
											}

											fhar = 1;
										}
										if (fhar == 0) {
											fdir = rand() % 4;
										}
									}
									else {
										for (int i = 1; i < c4->x - c3->x; i++) {
											if (map[c4->y][c3->x + i] > 99) {
												freeway = false;
											}
										}
										fdir = 1;
										if (fdir == 1 && map[fy][fx - 1] == 0 && freeway == true) {
											if (enemyType[a] == 0) {
												fire_left_spr->SetVelocity(-6, 0);
											}
											else if (enemyType[a] == 2) {
												fire_left_spr->SetVelocity(-10, 0);
											}

											fhar = 1;
										}
										if (fhar == 0) {
											fdir = rand() % 4;
										}
									}
								}

							}
						

							

							break;

						}
					}

				}
			}
			afdir[a] = fdir;
			aCaught[a] = caught;
			aCaughtX[a] = caughtX;
			aCaughtY[a] = caughtY;
			afreeway[a] = freeway;
			afireDie[a] = fireDie;
		}
		//-------------------- dynamite action --------------------------------
		if (dynamiteActive) {

			dynamite_counter--;
			if (dynamite_counter == 33) {
				dynamite_spr->SetBitmap(dynamite2);
			}
			if (dynamite_counter == 16) {
				dynamite_spr->SetBitmap(dynamite3);
			}
			if (dynamite_counter == 0)
			{
				xdy = dynamite_spr->GetPosition().left;
				ydy = dynamite_spr->GetPosition().top;

				PlaySound((LPCSTR)Explosion, _hInstance, SND_ASYNC |
					SND_RESOURCE);
				RECT rcPos = dynamite_spr->GetPosition();
				Sprite* pSprite = new Sprite(_pLgExplosionBitmap);
				pSprite->SetNumFrames(8, TRUE);
				pSprite->SetPosition(rcPos.left, rcPos.top);
				_pGame->AddSprite(pSprite);
				if (map[(ydy / 50) + 1][(xdy / 50)] > 100 ||
					map[(ydy / 50) + 1][(xdy / 50)] == 0)
				{
					map[(ydy / 50) + 1][(xdy / 50)] = 0;



					RECT rcPos2 = dynamite_spr->GetPosition();
					Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap);
					pSprite2->SetNumFrames(8, TRUE);
					pSprite2->SetPosition(rcPos2.left, rcPos2.top + 50);
					_pGame->AddSprite(pSprite2);

				}
				if (map[(ydy / 50) - 1][(dynamite_spr->GetPosition().left / 50)] > 100 ||
					map[(ydy / 50) - 1][(dynamite_spr->GetPosition().left / 50)] == 0)
				{
					map[(dynamite_spr->GetPosition().top / 50) - 1][(dynamite_spr->GetPosition().left / 50)] = 0;

					RECT rcPos2 = dynamite_spr->GetPosition();
					Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap);
					pSprite2->SetNumFrames(8, TRUE);
					pSprite2->SetPosition(rcPos2.left, rcPos2.top - 50);
					_pGame->AddSprite(pSprite2);

				}
				if (map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) + 1] > 100 ||
					map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) + 1] == 0)
				{
					map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) + 1] = 0;

					RECT rcPos2 = dynamite_spr->GetPosition();
					Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap);
					pSprite2->SetNumFrames(8, TRUE);
					pSprite2->SetPosition(rcPos2.left + 50, rcPos2.top);
					_pGame->AddSprite(pSprite2);

				}
				if (map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) - 1] > 100 ||
					map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) - 1] == 0)
				{
					map[(dynamite_spr->GetPosition().top / 50)][(dynamite_spr->GetPosition().left / 50) - 1] = 0;

					RECT rcPos2 = dynamite_spr->GetPosition();
					Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap);
					pSprite2->SetNumFrames(8, TRUE);
					pSprite2->SetPosition(rcPos2.left - 50, rcPos2.top);
					_pGame->AddSprite(pSprite2);

				}
				dynamite_spr->Kill();
				dynamite_counter = 50;
				dynamiteActive = false;
			}
		}

		//-------------------------------------------------------------------

		if (gempoint == 1) { //----- gemler toplaninca oyun biter ve kazaniriz
			gameOver = true;
			win = true;
		}
		else if (startTime == 0) { // ------ gemler toplanmamis ve sure bitmisse oyun biter ve kaybederiz

			gameOver = true;
			lose = true;
		}

		// Obtain a device context for repainting the game
		HWND  hWindow = _pGame->GetWindow();
		HDC   hDC = GetDC(hWindow);

		// Paint the game to the offscreen device context
		GamePaint(_hOffscreenDC);

		// Blit the offscreen bitmap to the game screen
		BitBlt(hDC, 0, 0, _pGame->GetWidth(), _pGame->GetHeight(),
			_hOffscreenDC, 0, 0, SRCCOPY);

		// Cleanup
		ReleaseDC(hWindow, hDC);



	}

}

void CollectGems() {
	if (by == gemy[0] && bx == gemx[0] && small_fire_gem_spr->IsHidden())
	{
		fire_gem_spr->Kill();
		small_fire_gem_spr->SetHidden(false);
		gempoint++;

	}
	else if (by == gemy[1] && bx == gemx[1] && small_water_gem_spr->IsHidden())
	{
		water_gem_spr->Kill();
		small_water_gem_spr->SetHidden(false);
		gempoint++;

	}
	else if (by == gemy[2] && bx == gemx[2] && small_earth_gem_spr->IsHidden())
	{
		earth_gem_spr->Kill();
		small_earth_gem_spr->SetHidden(false);
		gempoint++;

	}
	else if (by == gemy[3] && bx == gemx[3] && small_air_gem_spr->IsHidden())
	{
		air_gem_spr->Kill();
		small_air_gem_spr->SetHidden(false);
		gempoint++;

	}


}

void Mine() {


	//-- el bombasý olabilir 4 tane verilsin baþta


	if (direction == 1 && map[by][bx - 1] > 100) {
		map[by][bx - 1] = 0;
		RECT rcPos2 = playerspr->GetPosition();
		Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap2);
		pSprite2->SetNumFrames(8, TRUE);
		pSprite2->SetPosition(rcPos2.left - 50, rcPos2.top);
		_pGame->AddSprite(pSprite2);
		totalGrenade--;
	}
	if (direction == 2 && map[by][bx + 1] > 100) {
		map[by][bx + 1] = 0;
		RECT rcPos2 = playerspr->GetPosition();
		Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap2);
		pSprite2->SetNumFrames(8, TRUE);
		pSprite2->SetPosition(rcPos2.left + 50, rcPos2.top);
		_pGame->AddSprite(pSprite2);
		totalGrenade--;
	}
	if (direction == 3 && map[by - 1][bx] > 100) {
		map[by - 1][bx] = 0;
		RECT rcPos2 = playerspr->GetPosition();
		Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap2);
		pSprite2->SetNumFrames(8, TRUE);
		pSprite2->SetPosition(rcPos2.left, rcPos2.top - 50);
		_pGame->AddSprite(pSprite2);
		totalGrenade--;
	}
	if (direction == 4 && map[by + 1][bx] > 100) {
		map[by + 1][bx] = 0;
		RECT rcPos2 = playerspr->GetPosition();
		Sprite* pSprite2 = new Sprite(_pLgExplosionBitmap2);
		pSprite2->SetNumFrames(8, TRUE);
		pSprite2->SetPosition(rcPos2.left, rcPos2.top + 50);
		_pGame->AddSprite(pSprite2);
		totalGrenade--;
	}


}

void DropDynamite()
{

	RECT  rcPos = playerspr->GetPosition();

	rcPos.left = rcPos.left - (rcPos.left % 50);
	rcPos.top = rcPos.top - (rcPos.top % 50);

	dynamite_spr = new Sprite(dynamite);
	dynamite_spr->SetBoundsAction(BA_BOUNCE);
	dynamite_spr->SetPosition(rcPos.left, rcPos.top);
	dynamite_spr->SetZOrder(playerspr->GetZOrder() - 1);
	_pGame->AddSprite(dynamite_spr);
	dynamiteActive = true;
}

void HandleKeys()
{
	if (!gameOver) {
		int fxx, fyy, fhar = 0;
		RECT frect;
		frect = playerspr->GetPosition();
		fxx = frect.left;
		fyy = frect.top;
		bx = fxx / 50;
		by = fyy / 50;

		if (fxx % 50 == 0 && fyy % 50 == 0) playerspr->SetVelocity(0, 0);


		if (fyy % 50 == 0 && (GetAsyncKeyState('A') < 0 || GetAsyncKeyState(VK_LEFT) < 0)) {
			CollectGems();
			if (bx > 0 && map[by][bx - 1] == 0)
			{
				playerspr->SetVelocity(-bombermanspeed, 0);
			}
			direction = 1;
			playerspr->SetBitmap(playerbmp_left);
		}
		else if (fyy % 50 == 0 && (GetAsyncKeyState('D') < 0 || GetAsyncKeyState(VK_RIGHT) < 0)) {
			CollectGems();
			if (bx < maxcol - 1 && map[by][bx + 1] == 0)
			{
				playerspr->SetVelocity(bombermanspeed, 0);

			}
			playerspr->SetBitmap(playerbmp_right);
			direction = 2;
		}
		if (fxx % 50 == 0 && (GetAsyncKeyState('W') < 0 || GetAsyncKeyState(VK_UP) < 0)) {
			CollectGems();
			if (by > 0 && map[by - 1][bx] == 0)
			{
				playerspr->SetVelocity(0, -bombermanspeed);
			}
			playerspr->SetBitmap(playerbmp_up);
			direction = 3;
		}
		else if (fxx % 50 == 0 && (GetAsyncKeyState('S') < 0 || GetAsyncKeyState(VK_DOWN) < 0)) {
			CollectGems();
			if (by < maxrow - 1 && map[by + 1][bx] == 0)
			{
				playerspr->SetVelocity(0, bombermanspeed);
			}
			playerspr->SetBitmap(playerbmp_down);
			direction = 4;
		}



		if (dynamite_counter == 50 && GetAsyncKeyState(VK_SPACE) < 0 && totalbomb > 0) {
			/*if (enemyKill) {
				enemyKill = false;
			}*/
			DropDynamite();
			totalbomb--;

		}

		if (GetAsyncKeyState('M') < 0 && totalGrenade > 0) { // ----- el bombasý atma 

			Mine();
			//clickControl = false;


		}
	}
	if (gameOver && GetAsyncKeyState(VK_RETURN) < 0) {
		if (win) {
			NextLevel();

		}
		if (lose) {
			NewGame();
		}

	}


	// Fire missiles based upon spacebar presses
	/*if ((++_iFireInputDelay > 6) && GetAsyncKeyState(VK_SPACE) < 0)
	{
	// Create a new missile sprite
	RECT  rcBounds = { 50, 50, 20*50, 14*50 };
	RECT  rcPos = playerspr->GetPosition();
	Sprite* pSprite = NULL;
	if (direction == 1) {

	pSprite = new Sprite(arrow_left, rcBounds, BA_DIE);
	pSprite->SetPosition(rcPos.left - 50, rcPos.top);
	pSprite->SetVelocity(-5, 0);
	}
	if (direction == 2) {
	pSprite = new Sprite(arrow_right, rcBounds, BA_DIE);
	pSprite->SetPosition(rcPos.left + 50, rcPos.top);
	pSprite->SetVelocity(5, 0);
	}
	if (direction == 3) {
	pSprite = new Sprite(arrow_up, rcBounds, BA_DIE);
	pSprite->SetPosition(rcPos.left , rcPos.top - 50);
	pSprite->SetVelocity(0, -5);
	}
	if (direction == 4) {
	pSprite = new Sprite(arrow_down, rcBounds, BA_DIE);
	pSprite->SetPosition(rcPos.left, rcPos.top + 50);
	pSprite->SetVelocity(0, 5);
	}

	_pGame->AddSprite(pSprite);



	// Reset the input delay
	_iFireInputDelay = 0;
	}
	*/


}

void MouseButtonDown(int x, int y, BOOL bLeft)
{
	// See if a player was clicked with the left mouse button

}

void MouseButtonUp(int x, int y, BOOL bLeft)
{
	// Release the mouse
	ReleaseCapture();

	// Stop dragging
	/*
	_pDragSprite = NULL;
	drag = false;
	c2->x = x / 50;
	c2->y = y / 50;
	showpath = 1;
	findpathBFS(c1, c2, map, maxrow, maxcol, 0, path);
	Sleep(5000);
	showpath = 0;
	*/
}

void MouseMove(int x, int y)
{


}

void HandleJoystick(JOYSTATE jsJoystickState)
{
}

BOOL SpriteCollision(Sprite* pSpriteHitter, Sprite* pSpriteHittee)
{
	//------ dusman bizi oldururse oyun biter ve kaybederiz --------

	Bitmap* pHitter = pSpriteHitter->GetBitmap();
	Bitmap* pHittee = pSpriteHittee->GetBitmap();

	/*	if (dynamiteActive == true && dynamite_spr->GetPosition().top == playerspr->GetPosition().top &&dynamite_spr->GetPosition().left == playerspr->GetPosition().left) {

		   fire_left_spr->SetZOrder(dynamite_spr->GetZOrder());
	   } */

	if ((pHitter == fire_left_bmp1 || pHitter == air_right_bmp || pHitter == water_left_bmp) && (pHittee == playerbmp_down || pHittee == playerbmp_up || pHittee == playerbmp_right || pHittee == playerbmp_left))
	{
		//    Play the small explosion sound
		/* PlaySound((LPCSTR)IDW_LGEXPLODE, _hInstance, SND_ASYNC |
		SND_RESOURCE);*/

		pSpriteHittee->Kill();

		RECT rcPos;
		if (pHittee == playerbmp_down || pHittee == playerbmp_up || pHittee == playerbmp_right || pHittee == playerbmp_left)
			rcPos = pSpriteHittee->GetPosition();
		else
			rcPos = pSpriteHitter->GetPosition();

		Sprite* pSprite = new Sprite(_pLgExplosionBitmap);
		pSprite->SetNumFrames(8, TRUE);
		pSprite->SetPosition(rcPos.left, rcPos.top);
		_pGame->AddSprite(pSprite);

		gameOver = true;
		lose = true;

	}
	if (pHitter == _pLgExplosionBitmap && (pHittee == fire_left_bmp1 || pHittee == air_right_bmp || pHittee == water_left_bmp) && xdy != NULL)
	{

		for (int f = 0; f < afnum; f++) {

			int sprleft = afire_left_spr[f]->GetPosition().left;
			int sprtop = afire_left_spr[f]->GetPosition().top;

			if ((xdy - 75) < sprleft && (xdy + 75) > sprleft && (ydy - 75) < sprtop && (ydy + 75) > sprtop)
			{
				if (!afireDie[f])
				{
					totalbomb += bombPerKill;
					afire_left_spr[f]->Kill();
					afireDie[f] = true;
					afdir[f] = -1;
					afx[f], afy[f] = 0;
				}


			}


		}

	}

	if (pHitter == _pLgExplosionBitmap && (pHittee == playerbmp_down || pHittee == playerbmp_up || pHittee == playerbmp_right || pHittee == playerbmp_left))
	{
		//    Play the small explosion sound
		/* PlaySound((LPCSTR)IDW_LGEXPLODE, _hInstance, SND_ASYNC |
		SND_RESOURCE);*/

		pSpriteHittee->Kill();

		gameOver = true;
		lose = true;

	}

	return FALSE;
}

void SpriteDying(Sprite* pSprite)
{

}

