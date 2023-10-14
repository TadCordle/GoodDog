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

	WobblyLine(Texture2D& lineTex, Vector2 _start, Vector2 _end);
	void Update(float dt, float wobbleRate);
	void Draw();
};