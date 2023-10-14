#pragma once

#include "raylib.h"

struct WobblyTexture
{
	Texture2D texture;
	float wobbleAngle;
	float wobbleTime;
	Vector2 wobbleOffset;
	Vector2 wobbleScale;

	WobblyTexture(const char* _path);
	void Update(float dt, bool stableWobble, float wobbleRate);
	void Draw(Vector2 pos, Vector2 scale, float angle);
	void Unload();
};
