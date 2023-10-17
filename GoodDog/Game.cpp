#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor(start, end);
}

void Game::AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button)
{
	elevators[elevatorsCount++] = Elevator(start, end, newStart, newEnd, travelTime, button);
}

void Game::AddDangerBlock(Vector2 pos1, Vector2 pos2, Vector2 size, Button button)
{
	dangerBlocks[dangerBlocksCount++] = DangerBlock(pos1, pos2, size, button);
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
	line1.Draw(lineTex, { start.x, start.y - 12.f }, { end.x, end.y - 12.f });
	line2.Draw(lineTex, { start.x, start.y + 12.f }, { end.x, end.y + 12.f });
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
	if (currentTravelTime < 0.f) currentTravelTime = 0.f;
	if (currentTravelTime > 0.4f) currentTravelTime = 0.4f;
}

void DangerBlock::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Vector2 p = GetCurrentPos();
	Vector2 topLeft = { p.x - dimensions.x / 2.f, p.y - dimensions.y / 2.f };
	Vector2 botRight = { p.x + dimensions.x / 2.f, p.y + dimensions.y / 2.f };
	wobblyRectangle.Draw(lineTex, paintTex, topLeft, botRight);
	// TODO: Draw prompt
}
