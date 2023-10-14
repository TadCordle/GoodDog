#include "WobblyTexture.h"

WobblyTexture::WobblyTexture(const char* _path)
{
	texture = LoadTexture(_path);
	wobbleTime = 0.f;
	wobbleOffset = { 0.f, 0.f };
	wobbleScale = { 1.f, 1.f };
	wobbleAngle = 0.f;
}

void WobblyTexture::Update(float dt, bool stableWobble, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		wobbleTime = wobbleRate;
		wobbleOffset = { (float)GetRandomValue(-5, 5), (float)GetRandomValue(-5, 5) };
		if (!stableWobble) wobbleScale = { (float)GetRandomValue(95, 105) / 100.f, (float)GetRandomValue(95, 105) / 100.f };
		if (!stableWobble) wobbleAngle = (float)GetRandomValue(-1, 1);
	}
}

void WobblyTexture::Draw(Vector2 pos, Vector2 scale, float angle)
{
	Rectangle srcRect = { 0.f, 0.f, (float)texture.width, (float)texture.height };
	Vector2 frameSize = { srcRect.width * wobbleScale.x * scale.x, srcRect.height * wobbleScale.y * scale.y };
	Rectangle dstRect = { pos.x + wobbleOffset.x * scale.x, pos.y + wobbleOffset.y * scale.y, frameSize.x, frameSize.y };
	Vector2 origin = { frameSize.x / 2.f, frameSize.y / 2.f };
	DrawTexturePro(texture, srcRect, dstRect, origin, angle + wobbleAngle, WHITE);
}

void WobblyTexture::Unload()
{
	UnloadTexture(texture);
}
