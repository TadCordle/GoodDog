#include "WobblyRender.h"

int pcg_hash(int input)
{
	int state = input * 747796405u + 2891336453u;
	int  word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float hash_float(int seed)
{
	return (float)pcg_hash(seed) / (float)0xffffffffu * 2.f;
}

void WobblyTexture::Update(float dt, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		wobbleTime = wobbleRate;
		wobbleState = GetRandomValue(0, 9999);
	}
}

void WobblyTexture::Draw(Texture2D& texture, Vector2 pos, Vector2 scale, float angle, bool hFlipped, bool stableWobble, float alpha, bool lightning)
{
	float randOffX = hash_float(wobbleState) * 10.f - 5.f;
	float randOffY = hash_float(wobbleState + 1) * 10.f - 5.f;
	float randScaleX = stableWobble ? 1.f : (hash_float(wobbleState + 2) * 0.1f + 0.95f);
	float randScaleY = stableWobble ? 1.f : (hash_float(wobbleState + 3) * 0.1f + 0.95f);
	float randAngle = stableWobble ? 0.f : (hash_float(wobbleState + 4) * 2.f - 1.f);

	Vector2 frameSize = { texture.width * randScaleX * scale.x, texture.height * randScaleY * scale.y };
	Rectangle srcRect = { 0.f, 0.f, (hFlipped ? -1.f : 1.f) * (float)texture.width, (float)texture.height };
	Rectangle dstRect = { pos.x + randOffX * scale.x, pos.y + randOffY * scale.y, frameSize.x, frameSize.y };
	Vector2 origin = { frameSize.x / 2.f, frameSize.y / 2.f };
	unsigned char colVal = lightning ? 0 : 255;
	Color color = { colVal, colVal, colVal, (unsigned char)(alpha * 255) };
	DrawTexturePro(texture, srcRect, dstRect, origin, angle + randAngle, color);
}

WobblyLine::WobblyLine()
{
	wobbleTime = 0.f;
	wobbleState = GetRandomValue(0, 9999);
}

void WobblyLine::Update(float dt, float wobbleRate)
{
	wobbleTime -= dt;
	if (wobbleTime <= 0.f)
	{
		wobbleState = GetRandomValue(0, 9999);
		wobbleTime = wobbleRate;
	}
}

void WobblyLine::Draw(Texture2D& lineTex, Vector2 start, Vector2 end)
{
	float totalLength = Vector2Distance(start, end);
	int numSegments = (int)(totalLength / 128) + 1;
	Vector2 s = start;
	for (int i = 0; i < numSegments; i++)
	{
		float wobble = hash_float(wobbleState + i);
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

void DrawPaintLine(Texture2D& paintTex, Vector2 start, Vector2 end, bool lightning)
{
	float totalLength = Vector2Distance(start, end);
	if (totalLength < 256.f)
	{
		Rectangle srcRect = { 0.f, hash_float((int)start.x) * 232.f, totalLength, 24.f };
		Rectangle dstRect = { (start.x + end.x) / 2.f, (start.y + end.y) / 2.f, totalLength, 24.f };
		Vector2 origin = { totalLength / 2.f, 12.f };
		float angle = atan2f(end.y - start.y, end.x - start.x) * RAD2DEG;
		DrawTexturePro(paintTex, srcRect, dstRect, origin, angle, lightning ? BLACK : WHITE);
	}
	else
	{
		Vector2 s = start;
		int numSegments = (int)(totalLength / 256) + 1;
		for (int i = 0; i < numSegments; i++)
		{
			Vector2 e = (i == numSegments - 1) ? end : Vector2Lerp(start, end, 256 * (i + 1) / totalLength);
			if (i == numSegments - 1)
				s = Vector2Lerp(start, end, 1.f - (256.f / totalLength));
			Rectangle srcRect = { 0.f, hash_float(i) * 232.f, 256.f, 24.f };
			Rectangle dstRect = { (s.x + e.x) / 2.f, (s.y + e.y) / 2.f, 256.f, 24.f};
			Vector2 origin = { 128.f, 12.f };
			float angle = atan2f(e.y - s.y, e.x - s.x) * RAD2DEG;
			DrawTexturePro(paintTex, srcRect, dstRect, origin, angle, lightning ? BLACK : WHITE);
			s = e;
		}
	}
}

void WobblyRectangle::Update(float dt, float wobbleRate)
{
	top.Update(dt, wobbleRate);
	bottom.Update(dt, wobbleRate);
	left.Update(dt, wobbleRate);
	right.Update(dt, wobbleRate);
}

void WobblyRectangle::Draw(Texture2D& lineTex, Texture2D& paintTex, Vector2 topLeft, Vector2 botRight, bool lightning)
{
	float sizeX = botRight.x - topLeft.x;
	float sizeY = botRight.y - topLeft.y;
	int numFillsX = (int)(sizeX / 256.f) + 1;
	int numFillsY = (int)(sizeY / 256.f) + 1;
	Vector2 srcRectSize = { numFillsX == 1 ? sizeX : 256.f, numFillsY == 1 ? sizeY : 256.f };
	Rectangle srcRect = { (256.f - srcRectSize.x) / 2.f, (256.f - srcRectSize.y) / 2.f, srcRectSize.x, srcRectSize.y };
	for (int x = 0; x < numFillsX; x++)
	{
		float dstX = x == numFillsX - 1 ? (botRight.x - srcRectSize.x) : (x * 256.f + topLeft.x);
		for (int y = 0; y < numFillsY; y++)
		{
			float dstY = y == numFillsY - 1 ? (botRight.y - srcRectSize.y) : (y * 256.f + topLeft.y);
			Rectangle dstRect = { dstX, dstY, srcRectSize.x, srcRectSize.y };
			DrawTexturePro(paintTex, srcRect, dstRect, { 0.f, 0.f }, 0.f, lightning ? BLACK : WHITE);
		}
	}

	top.Draw(lineTex, topLeft, { botRight.x, topLeft.y });
	bottom.Draw(lineTex, { topLeft.x, botRight.y }, botRight);
	left.Draw(lineTex, topLeft, { topLeft.x, botRight.y });
	right.Draw(lineTex, { botRight.x, topLeft.y }, botRight);
}

void DrawQuestionMark(Font& font, Vector2 position)
{
	const char* str = "?";
	Vector2 fontSize = MeasureTextEx(font, str, 80, 0);
	DrawTextEx(font, str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { 3.f, 3.f }), 80, 0, BLACK);
	DrawTextEx(font, str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { -3.f, 3.f }), 80, 0, BLACK);
	DrawTextEx(font, str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { 3.f, -3.f }), 80, 0, BLACK);
	DrawTextEx(font, str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { -3.f, -3.f }), 80, 0, BLACK);
	DrawTextEx(font, str, Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), 80, 0, MAGENTA);
}

void DrawButtonText(Font& font, Texture& cursorTex, Vector2 position, int button, bool lightning)
{
	if (button == 3)
	{
		Vector2 pos = Vector2Subtract(position, { 40.f, 40.f });
		DrawTexture(cursorTex, (int)pos.x, (int)pos.y, lightning ? BLACK : WHITE);
	}
	else if (button < 65)
		DrawQuestionMark(font, position);
	else
	{
		StringAndColor sc = stringsAndColors[button - 65];
		Color brightened = ColorBrightness(sc.color, 0.5f);
		Vector2 fontSize = MeasureTextEx(font, sc.str, 80, 0);
		DrawTextEx(font, sc.str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { 3.f, 3.f }), 80, 0, BLACK);
		DrawTextEx(font, sc.str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { -3.f, 3.f }), 80, 0, BLACK);
		DrawTextEx(font, sc.str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { 3.f, -3.f }), 80, 0, BLACK);
		DrawTextEx(font, sc.str, Vector2Add(Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), { -3.f, -3.f }), 80, 0, BLACK);
		DrawTextEx(font, sc.str, Vector2Subtract(position, Vector2Scale(fontSize, 0.5f)), 80, 0, lightning ? BLACK : brightened);
	}
}