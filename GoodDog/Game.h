#pragma once

#include <iostream>
#include "raylib.h"
#include "WobblyRender.h"

#define DOG_WOBBLE_RATE 0.2f
#define WALL_WOBBLE_RATE 0.75f
#define HOP_TIMER 0.833333333333

enum Button { N, E, S, W };
enum CurveType { NE, SE, SW, NW };

struct Floor
{
	WobblyLine line1, line2;
	PaintLine solid;

	Floor() {}
	Floor(Texture2D& lineTex, Texture2D& paintTex, Vector2 start, Vector2 end);
	void Update(float dt, float wobbleRate);
	void Draw();
};

struct DangerBlock
{
	Rectangle dimensions;
	Vector2 pos1;
	Vector2 pos2;
	Button button;
};

struct Elevator
{
	float startX;
	float endX;
	float startY;
	float endY;
	float lerpTime;
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
	Texture2D texLine;
	Texture2D texPaint;

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

	Game() {}
	Game(Texture2D& _texLine, Texture2D& _texPaint) { texLine = _texLine; texPaint = _texPaint; }
	void AddFloor(Vector2 start, Vector2 end);
};