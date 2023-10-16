#pragma once

#include <iostream>
#include "raylib.h"
#include "WobblyRender.h"

#define DOG_WOBBLE_RATE 0.2f
#define WALL_WOBBLE_RATE 0.75f
#define HOP_TIMER 0.833333333333

enum Button { 
	A = KEY_A,
	B = KEY_B, 
	C = KEY_C, 
	D = KEY_D, 
	E = KEY_E, 
	F = KEY_F, 
	G = KEY_G, 
	H = KEY_H, 
	I = KEY_I, 
	J = KEY_J, 
	K = KEY_K, 
	L = KEY_L, 
	M = KEY_M, 
	N = KEY_N, 
	O = KEY_O, 
	P = KEY_P, 
	Q = KEY_Q, 
	R = KEY_R, 
	S = KEY_S, 
	T = KEY_T, 
	U = KEY_U, 
	V = KEY_V, 
	W = KEY_W, 
	X = KEY_X, 
	Y = KEY_Y, 
	Z = KEY_Z 
};

enum CurveType { NE, SE, SW, NW };

struct Floor
{
	Vector2 start = {}, end = {};
	WobblyLine line1, line2;

	Floor() {}
	Floor(Vector2 _start, Vector2 _end);
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Texture2D& paintTex);
};

struct Elevator
{
	Vector2 start = {};
	Vector2 end = {};
	Vector2 newStart = {};
	Vector2 newEnd = {};
	WobblyLine line1, line2;
	float travelTime = 0.f;
	float currentTravelTime = 0.f;
	Button button;

	Elevator() {}
	Elevator(Vector2 _start, Vector2 _end, Vector2 _newStart, Vector2 _newEnd, float _travelTime, Button _button);
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Texture2D& paintTex);

	inline Vector2 GetCurrentStart() { float t = currentTravelTime / travelTime; return Vector2Lerp(start, newStart, t); }
	inline Vector2 GetCurrentEnd()   { float t = currentTravelTime / travelTime; return Vector2Lerp(  end,   newEnd, t); }
};

struct DangerBlock
{
	Rectangle dimensions;
	Vector2 pos1;
	Vector2 pos2;
	Button button;
};

struct Reverser
{
	Rectangle dimensions;
	Vector2 pos;
	Vector2 pos1;
	Vector2 pos2;
	Button button;
	float enabled;
};

struct Curve
{
	Vector2 pos;
	CurveType type;
};

enum GameState
{
	CUTSCENE,
	GOING,
	WIN,
	LOSE
};

struct Game
{
	Floor floors[256] = {};
	Curve curves[256] = {};
	DangerBlock dangerBlocks[256] = {};
	Elevator elevators[256] = {};
	Reverser reversers[256] = {};

	int floorsCount = 0;
	int reversersCount = 0;
	int elevatorsCount = 0;
	int dangerBlocksCount = 0;
	int curvesCount = 0;

	Camera2D camera;

	Game() {}
	void AddFloor(Vector2 start, Vector2 end);
	void AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button);
};