#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor(texLine, texPaint, start, end);
}

Floor::Floor(Texture2D & lineTex, Texture2D & paintTex, Vector2 start, Vector2 end)
{
	line1 = WobblyLine(lineTex, { start.x, start.y - 12.f }, { end.x, end.y - 12.f });
	line2 = WobblyLine(lineTex, { start.x, start.y + 12.f }, { end.x, end.y + 12.f });
	solid = PaintLine(paintTex, { start.x, start.y }, { end.x, end.y });
}

void Floor::Update(float dt, float wobbleRate)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);
}

void Floor::Draw()
{
	solid.Draw();
	line1.Draw();
	line2.Draw();
}