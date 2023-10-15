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
	int state;

	WobblyTexture(const char* _path);
	void Update(float dt, float wobbleRate);
	void Draw(Vector2 pos, Vector2 scale, float angle, bool hFlipped = false, bool stableWobble = false);
	void Unload();
};

struct WobblyLine
{
	Vector2 start = {};
	Vector2 end = {};
	float wobbleTime = 0.f;
	int state = 0;
	Texture2D lineTex;

	WobblyLine() {}
	WobblyLine(Texture2D& lineTex, Vector2 _start, Vector2 _end);
	void Update(float dt, float wobbleRate);
	void Draw();
};

struct PaintLine
{
	Texture2D paintTex;
	Vector2 start = {};
	Vector2 end = {};

	PaintLine() {}
	PaintLine(Texture2D& _paintTex, Vector2 _start, Vector2 _end);
	void Draw();
};

struct WobblyRectangle
{
	WobblyLine top, bottom, left, right;
	Texture2D paintTex;
	Vector2 srcRectSize = {};
	Vector2 pos = {};
	int numFillsX = 0;
	int numFillsY = 0;

	WobblyRectangle() {}
	WobblyRectangle(Texture2D& _lineTex, Texture2D& _paintTex, Vector2 _pos, Vector2 _size);
	void Update(float dt, float wobbleRate);
	void Draw();
};