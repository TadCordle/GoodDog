#include "WobblyRender.h"

int pcg_hash(int input)
{
	int state = input * 747796405u + 2891336453u;
	int  word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float hash_float(int seed)
{
	return (float)pcg_hash(seed) / (float)UINT32_MAX * 2.f;
}

WobblyTexture::WobblyTexture(const char* _path)
{
	texture = LoadTexture(_path);
	wobbleTime = 0.f;
	state = 0;
}

void WobblyTexture::Update(float dt, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		wobbleTime = wobbleRate;
		state = GetRandomValue(0, 9999);
	}
}

void WobblyTexture::Draw(Vector2 pos, Vector2 scale, float angle, bool hFlipped, bool stableWobble)
{
	float randOffX = hash_float(state) * 10.f - 5.f;
	float randOffY = hash_float(state) * 10.f - 5.f;
	float randScaleX = stableWobble ? 1.f : (hash_float(state) * 0.1f + 0.95f);
	float randScaleY = stableWobble ? 1.f : (hash_float(state) * 0.1f + 0.95f);
	float randAngle = stableWobble ? 0.f : (hash_float(state) * 2.f - 1.f);

	Vector2 frameSize = { texture.width * randScaleX * scale.x, texture.height * randScaleY * scale.y };
	Rectangle srcRect = { 0.f, 0.f, (hFlipped ? -1.f : 1.f) * (float)texture.width, (float)texture.height };
	Rectangle dstRect = { pos.x + randOffX * scale.x, pos.y + randOffY * scale.y, frameSize.x, frameSize.y };
	Vector2 origin = { frameSize.x / 2.f, frameSize.y / 2.f };
	DrawTexturePro(texture, srcRect, dstRect, origin, angle + randAngle, WHITE);
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
	state = GetRandomValue(0, 9999);
}

void WobblyLine::Update(float dt, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		state = GetRandomValue(0, 9999);
		wobbleTime = wobbleRate;
	}
}

void WobblyLine::Draw()
{
	float totalLength = Vector2Distance(start, end);
	int numSegments = (int)(totalLength / 128) + 1;
	Vector2 s = start;
	for (int i = 0; i < numSegments; i++)
	{
		float wobble = hash_float(state + i);
		Vector2 e = (i == numSegments - 1) ? end : Vector2Lerp(start, end, 128 * (i + 1) / totalLength);
		float segmentSize = Vector2Distance(s, e);
		Rectangle srcRect = { wobble * 384.f, 0.f, 128.f, 32.f};
		Rectangle dstRect = { (s.x + e.x) / 2.f, (s.y + e.y) / 2.f, segmentSize, 32.f };
		Vector2 origin = { segmentSize / 2.f, 16.f };
		float angle = atan2f(e.y - s.y, e.x - s.x) * RAD2DEG + wobble;
		DrawTexturePro(lineTex, srcRect, dstRect, origin, angle, WHITE);
		s = e;
	}
}

PaintLine::PaintLine(Texture2D& _paintTex, Vector2 _start, Vector2 _end)
{
	paintTex = _paintTex;
	start = _start;
	end = _end;
}

void PaintLine::Draw()
{
	float totalLength = Vector2Distance(start, end);
	if (totalLength < 256.f)
	{
		Rectangle srcRect = { 0.f, hash_float((int)start.x) * 232.f, totalLength, 24.f };
		Rectangle dstRect = { (start.x + end.x) / 2.f, (start.y + end.y) / 2.f, 256.f, 24.f };
		Vector2 origin = { 128.f, 12.f };
		float angle = atan2f(end.y - start.y, end.x - start.x) * RAD2DEG;
		DrawTexturePro(paintTex, srcRect, dstRect, origin, angle, WHITE);
	}
	else
	{
		Vector2 s = start;
		int numSegments = (int)(totalLength / 96) + 1;
		for (int i = 0; i < numSegments; i++)
		{
			Vector2 e = (i == numSegments - 1) ? end : Vector2Lerp(start, end, 256 * (i + 1) / totalLength);
			if (i == numSegments - 1)
				s = Vector2Lerp(start, end, 1.f - (256.f / totalLength));
			Rectangle srcRect = { 0.f, hash_float((int)s.x + i) * 232.f, 256.f, 24.f };
			Rectangle dstRect = { (s.x + e.x) / 2.f, (s.y + e.y) / 2.f, 256.f, 24.f};
			Vector2 origin = { 128.f, 12.f };
			float angle = atan2f(e.y - s.y, e.x - s.x) * RAD2DEG;
			DrawTexturePro(paintTex, srcRect, dstRect, origin, angle, WHITE);
			s = e;
		}
	}
}

WobblyRectangle::WobblyRectangle(Texture2D& _lineTex, Texture2D& _paintTex, Vector2 _pos, Vector2 _size)
{
	Vector2 topLeft  = { _pos.x - _size.x / 2.f, _pos.y - _size.y / 2.f };
	Vector2 topRight = { _pos.x + _size.x / 2.f, _pos.y - _size.y / 2.f };
	Vector2 botLeft  = { _pos.x - _size.x / 2.f, _pos.y + _size.y / 2.f };
	Vector2 botRight = { _pos.x + _size.x / 2.f, _pos.y + _size.y / 2.f };
	pos = topLeft;
	top = WobblyLine(_lineTex, topLeft, topRight);
	bottom = WobblyLine(_lineTex, botLeft, botRight);
	left = WobblyLine(_lineTex, topLeft, botLeft);
	right = WobblyLine(_lineTex, topRight, botRight);
	paintTex = _paintTex;
}

void WobblyRectangle::Update(float dt, float wobbleRate)
{
	top.Update(dt, wobbleRate);
	bottom.Update(dt, wobbleRate);
	left.Update(dt, wobbleRate);
	right.Update(dt, wobbleRate);
}

void WobblyRectangle::Draw()
{
	float sizeX = right.start.x - left.start.x;
	float sizeY = bottom.start.y - top.start.y;
	int numFillsX = (int)(sizeX / 256.f) + 1;
	int numFillsY = (int)(sizeY / 256.f) + 1;
	Vector2 srcRectSize = { numFillsX == 1 ? sizeX : 256.f, numFillsY == 1 ? sizeY : 256.f };
	Rectangle srcRect = { (256.f - srcRectSize.x) / 2.f, (256.f - srcRectSize.y) / 2.f, srcRectSize.x, srcRectSize.y };
	for (int x = 0; x < numFillsX; x++)
	{
		float dstX = x == numFillsX - 1 ? (right.start.x - srcRectSize.x) : (x * 256.f + pos.x);
		for (int y = 0; y < numFillsY; y++)
		{
			float dstY = y == numFillsY - 1 ? (bottom.start.y - srcRectSize.y) : (y * 256.f + pos.y);
			Rectangle dstRect = { dstX, dstY, srcRectSize.x, srcRectSize.y };
			DrawTexturePro(paintTex, srcRect, dstRect, { 0.f, 0.f }, 0.f, WHITE);
		}
	}

	top.Draw();
	bottom.Draw();
	left.Draw();
	right.Draw();
}
