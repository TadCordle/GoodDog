#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor{ start, end };
	floorRenders[floorRendersCount++] = FloorRender(texLine, texPaint, start, end);
}