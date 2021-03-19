//-----------------------------------------------------------------
// Fore 2 Application
// C++ Header - Fore.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "Resource.h"
#include "GameEngine.h"
#include "Bitmap.h"
#include "Sprite.h"
#include "Player.h"





#include <stack>
#include <queue>

class coord {
public:
	int x;
	int y;
	int dist;
	coord* prev;

public:
	coord::coord(int x, int y) {
		this->x = x; this->y = y;
		dist = 0;    prev = NULL;
	}
	coord::coord(int x, int y, int dist, coord* prev) {
		this->x = x; this->y = y;
		this->dist = dist; this->prev = prev;
	}
};

//-----------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------
HINSTANCE   _hInstance;
GameEngine* _pGame;
HDC         _hOffscreenDC;
HBITMAP     _hOffscreenBitmap;
Bitmap*     ground_bmp;
Bitmap*     wall_bmp;
Bitmap*     playerbmp_right;
Bitmap*     playerbmp_left;
Bitmap*     playerbmp_up;
Bitmap*     playerbmp_down;

Bitmap* fire_gem;
Bitmap* earth_gem;
Bitmap* air_gem;
Bitmap* water_gem;
Bitmap* base_wall;
Bitmap* panel;
Bitmap* dynamite;
Sprite* dynamite_spr;
Bitmap* dynamite2;
Bitmap* dynamite3;
Sprite* playerspr;

int **map;
int **map2;
coord **path;
coord *c1, *c2;
int maxrow = 15, maxcol = 21;  // 6*9
int wall, maxwall = ((maxrow - 3) / 2)*((maxcol - 3) / 2);





HDC hDC;

int bx = 1, by = 1;

int fx = rand() % 11 + 10 , fy = rand() % 8 + 7;
int fdir = rand() % 4;
int adir = rand() % 4;
int afx[100], afy[100], afdir[100];
int aax[100], aay[100], aadir[100];
int level = 1;
int afnum = level*2+level/5;
int enemyType[100]; //0 = fire, 1 = air


RECT rc;
HBRUSH brush;
//---------------------------------------------------------------
int direction = -1;
int counter = 0;
int dynamite_counter = 50;
bool dynamiteActive = false;

int _iFireInputDelay;

int xdy, ydy;

int gemx[4];
int gemy[4];

bool aCaught[100];
int aCaughtX[100];
int aCaughtY[100];
bool afreeway[100];

Bitmap* fire_left_bmp1;
Sprite* fire_left_spr;
Sprite* afire_left_spr[100];
Bitmap* air_right_bmp;
Bitmap* water_left_bmp;
Sprite* air_spr;
Sprite* aair_spr[100];

Sprite* air_gem_spr;
Sprite* fire_gem_spr;
Sprite* water_gem_spr;
Sprite* earth_gem_spr;

int gempoint;
int initialBomb = 20;
int initialGrenade = 5;
int totalbomb = 20;
int totalGrenade = 5;
int bombPerKill = 5;
int secForBomb = 20;
bool bombAdded = false;
int gemsToWin = 1;


Bitmap* small_dynamite;
Bitmap* small_clock;
Bitmap* small_grenade;

Bitmap* small_fire_gem;
Bitmap* small_earth_gem;
Bitmap* small_air_gem;
Bitmap* small_water_gem;

Sprite* small_air_gem_spr;
Sprite* small_fire_gem_spr;
Sprite* small_water_gem_spr;
Sprite* small_earth_gem_spr;

int startTime = 5068;

BOOL gameOver;

BOOL fireDie;
BOOL airDie;
BOOL afireDie[100];
BOOL aairDie[100];

Bitmap* _pLgExplosionBitmap;
Bitmap* _pLgExplosionBitmap2;

coord *c3,*c4;
coord **path2;
int bombermanspeed = 10;

Bitmap* win_bmp;
Bitmap* lose_bmp;

BOOL win;
BOOL lose;



