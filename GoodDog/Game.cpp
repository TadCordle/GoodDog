#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor(start, end);
}

void Game::AddCurve(Vector2 pos, CurveType type)
{
	curves[curvesCount++] = Curve(pos, type);
}

void Game::AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button)
{
	elevators[elevatorsCount++] = Elevator(start, end, newStart, newEnd, travelTime, button);
}

void Game::AddDangerBlock(Vector2 pos1, Vector2 pos2, Vector2 size, Button button)
{
	dangerBlocks[dangerBlocksCount++] = DangerBlock(pos1, pos2, size, button);
}

void Game::AddReverser(Vector2 pos1, Vector2 pos2, Direction dir, Button button)
{
	reversers[reversersCount++] = Reverser(pos1, pos2, dir, button);
}

Floor::Floor(Vector2 _start, Vector2 _end)
{
	start = _start;
	end = _end;
	line1 = WobblyLine();
	line2 = WobblyLine();
}

void Floor::Update(float dt, float wobbleRate)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);
}

void Floor::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	DrawPaintLine(paintTex, { start.x, start.y }, { end.x, end.y });
	Vector2 offsetDir = Vector2Normalize({ end.y - start.y, end.x - start.x });
	Vector2 offsetPos = Vector2Scale(offsetDir, 12.f);
	Vector2 offsetNeg = Vector2Scale(offsetDir, -12.f);
	line1.Draw(lineTex, { start.x + offsetNeg.x, start.y + offsetNeg.y }, { end.x + offsetNeg.x, end.y + offsetNeg.y });
	line2.Draw(lineTex, { start.x + offsetPos.x, start.y + offsetPos.y }, { end.x + offsetPos.x, end.y + offsetPos.y });
}

Elevator::Elevator(Vector2 _start, Vector2 _end, Vector2 _newStart, Vector2 _newEnd, float _travelTime, Button _button)
{
	start = _start;
	end = _end;
	newStart = _newStart;
	newEnd = _newEnd;
	travelTime = _travelTime;
	button = _button;
	line1 = WobblyLine();
	line2 = WobblyLine();
}

void Elevator::Update(float dt, float wobbleRate)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f) currentTravelTime = 0.f;
	if (currentTravelTime > travelTime) currentTravelTime = travelTime;
}

void Elevator::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Vector2 s = GetCurrentStart();
	Vector2 e = GetCurrentEnd();
	DrawPaintLine(paintTex, { s.x, s.y }, { e.x, e.y });
	line1.Draw(lineTex, { s.x, s.y - 12.f }, { e.x, e.y - 12.f });
	line2.Draw(lineTex, { s.x, s.y + 12.f }, { e.x, e.y + 12.f });
}

DangerBlock::DangerBlock(Vector2 _pos1, Vector2 _pos2, Vector2 _dimensions, Button _button)
{
	pos1 = _pos1;
	pos2 = _pos2;
	dimensions = _dimensions;
	button = _button;
	wobblyRectangle = WobblyRectangle();
}

void DangerBlock::Update(float dt, float wobbleRate)
{
	wobblyRectangle.Update(dt, wobbleRate);

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.4f) currentTravelTime = 0.4f;
}

void DangerBlock::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Vector2 p = GetCurrentPos();
	Vector2 topLeft  = { p.x - dimensions.x / 2.f, p.y - dimensions.y / 2.f };
	Vector2 botRight = { p.x + dimensions.x / 2.f, p.y + dimensions.y / 2.f };
	wobblyRectangle.Draw(lineTex, paintTex, topLeft, botRight);
	// TODO: Draw prompt
}

Reverser::Reverser(Vector2 _pos1, Vector2 _pos2, Direction _dir, Button _button)
{
	pos1 = _pos1;
	pos2 = _pos2;
	dir = _dir;
	button = _button;
	texFront = WobblyTexture();
	texBack = WobblyTexture();
}

void Reverser::Update(float dt, float wobbleRate)
{
	texFront.Update(dt, wobbleRate);
	texBack.Update(dt, wobbleRate);

	if (enabled < 1.f)
	{
		enabled -= dt * 3.f;
		if (enabled < 0.f)
			enabled = 0.f;
	}

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.4f) currentTravelTime = 0.4f;
}

void Reverser::Draw(Texture2D& texBackEnabled, Texture2D& texBackDisabled, Texture2D& texOutline, Texture2D& texArrows)
{
	Vector2 p = GetCurrentPos();
	Vector2 scale = { 0.75f, 0.75f };
	float angle = dir == Up || dir == Down ? 90.f : 0.f;
	bool flipped = dir == Left || dir == Up;
	if (enabled < 1.f)
	{
		texBack.Draw(texBackDisabled, p, scale, angle, flipped, false, 1.f);
	}
	if (enabled > 0.f)
	{
		texBack.Draw(texBackEnabled, p, scale, angle, flipped, false, enabled);
		texFront.Draw(texArrows, p, scale, angle, flipped, false, enabled);
	}
	texFront.Draw(texOutline, p, scale, angle, flipped, true, 1.f);
}

Curve::Curve(Vector2 _pos, CurveType _type)
{
	pos = _pos;
	type = _type;
}

void Curve::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Rectangle srcRect = {};
	switch (type)
	{
		case NE: srcRect = { 128.f,   0.f, 128.f, 128.f }; break;
		case SE: srcRect = { 128.f, 128.f, 128.f, 128.f }; break;
		case SW: srcRect = {   0.f, 128.f, 128.f, 128.f }; break;
		case NW: srcRect = {   0.f,   0.f, 128.f, 128.f }; break;
	}

	Rectangle dstRect = { pos.x, pos.y, 128.f, 128.f };
	Vector2 origin = { 64.f, 64.f };
	DrawTexturePro(paintTex, srcRect, dstRect, origin, 0.f, WHITE);
	DrawTexturePro(lineTex, srcRect, dstRect, origin, 0.f, WHITE);
}
