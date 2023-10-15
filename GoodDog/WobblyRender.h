#pragma once

#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

struct WobblyTexture
{
	Texture2D texture;
	float wobbleTime;
	int wobbleState;

	WobblyTexture(const char* _path);
	void Update(float dt, float wobbleRate);
	void Draw(Vector2 pos, Vector2 scale, float angle, bool hFlipped = false, bool stableWobble = false);
	void Unload();
};

struct WobblyLine
{
	float wobbleTime;
	int wobbleState;

	WobblyLine();
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Vector2 start, Vector2 end);
};

struct WobblyRectangle
{
	WobblyLine top, bottom, left, right;

	WobblyRectangle() {}
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, Vector2 topLeft, Vector2 botRight);
};

void DrawPaintLine(Texture2D& paintTex, Vector2 start, Vector2 end);