#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor(start, end);
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