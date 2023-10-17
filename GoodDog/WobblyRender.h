#pragma once

#include "raylib.h"
#include "raymath.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

struct WobblyTexture
{
	float wobbleTime = 0.f;
	int wobbleState = 0;

	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& texture, Vector2 pos, Vector2 scale, float angle, bool hFlipped = false, bool stableWobble = false, float alpha = 1.f);
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