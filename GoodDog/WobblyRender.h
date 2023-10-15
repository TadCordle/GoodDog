#pragma once

#include "raylib.h"
#include "raymath.h"
#include <assert.h>

struct WobblyTexture
{
	Texture2D texture;
	float wobbleAngle;
	float wobbleTime;
	Vector2 wobbleOffset;
	Vector2 wobbleScale;

	WobblyTexture(const char* _path);
	void Update(float dt, bool stableWobble, float wobbleRate);
	void Draw(Vector2 pos, Vector2 scale, float angle, bool hFlipped = false);
	void Unload();
};

struct WobblyLine
{
	Texture2D lineTex;
	float wobbleTime;
	int numSegments;
	Vector2 start;
	Vector2 end;
	Rectangle srcRects[32] = {};
	float wobbleAngles[32] = {};

	WobblyLine() {}
	WobblyLine(Texture2D& lineTex, Vector2 _start, Vector2 _end);
	void Update(float dt, float wobbleRate);
	void Draw();
};

struct PaintLine
{
	Texture2D paintTex;
	int numSegments;
	Vector2 start;
	Vector2 end;
	Rectangle srcRects[32] = {};

	PaintLine() {}
	PaintLine(Texture2D& _paintTex, Vector2 _start, Vector2 _end);
	void Draw();
};

struct WobblyRectangle
{
	WobblyLine top, bottom, left, right;
	Rectangle fillRects[16] = {};
	int numFillsX = 0;
	int numFillsY = 0;

	WobblyRectangle() {}
	WobblyRectangle(Texture2D& _lineTex, Texture2D& _paintTex, Vector2 _pos, Vector2 _size);
	void Update(float dt, float wobbleRate);
	void Draw();
};