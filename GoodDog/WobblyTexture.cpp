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

void WobblyTexture::Draw(Vector2 pos, Vector2 scale, float angle, bool hFlipped)
{
	Vector2 frameSize = { texture.width * wobbleScale.x * scale.x, texture.height * wobbleScale.y * scale.y };
	Rectangle srcRect = { 0.f, 0.f, (hFlipped ? -1 : 1) * texture.width, texture.height };
	Rectangle dstRect = { pos.x + wobbleOffset.x * scale.x, pos.y + wobbleOffset.y * scale.y, frameSize.x, frameSize.y };
	Vector2 origin = { frameSize.x / 2.f, frameSize.y / 2.f };
	DrawTexturePro(texture, srcRect, dstRect, origin, angle + wobbleAngle, WHITE);
}

void WobblyTexture::Unload()
{
	UnloadTexture(texture);
}

WobblyLine::WobblyLine(Texture2D& _lineTex, Vector2 _start, Vector2 _end)
{
	lineTex = _lineTex;
	wobbleTime = 0.f;

	start = _start;
	end = _end;
	numSegments = (int)(Vector2Distance(_start, _end) / 128) + 1;
	assert(numSegments <= 32);
	for (int i = 0; i < numSegments; i++)
	{
		srcRects[i] = { (float)GetRandomValue(0, 384), 0.f, 128.f, 32.f };
	}
}

void WobblyLine::Update(float dt, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		wobbleTime = wobbleRate;
		for (int i = 0; i < numSegments; i++)
		{
			wobbleAngles[i] = (float)GetRandomValue(-1, 1);
		}
	}
}

void WobblyLine::Draw()
{
	float totalLength = Vector2Distance(start, end);
	Vector2 s = start;
	for (int i = 0; i < numSegments; i++)
	{
		Vector2 e = (i == numSegments - 1) ? end : Vector2Lerp(start, end, 128 * (i + 1) / totalLength);
		float segmentSize = Vector2Distance(s, e);
		Vector2 frameSize = { segmentSize, 32.f };
		Rectangle srcRect = srcRects[i];
		Rectangle dstRect = { (s.x + e.x) / 2.f, (s.y + e.y) / 2.f, frameSize.x, frameSize.y};
		Vector2 origin = { frameSize.x / 2.f, frameSize.y / 2.f };
		float angle = atan2f(e.y - s.y, e.x - s.x) * RAD2DEG;
		DrawTexturePro(lineTex, srcRect, dstRect, origin, angle + wobbleAngles[i], WHITE);
		s = e;
	}
}
