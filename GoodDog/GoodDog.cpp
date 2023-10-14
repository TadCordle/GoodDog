#include <iostream>
#include "raylib.h"
#include "WobblyTexture.h"

#define DOG_WOBBLE_RATE 0.2f
#define HOP_TIMER 0.4166666667;

int main()
{
	InitWindow(1280, 720, "Good Dog");
	SetTargetFPS(120);

	Vector2 pos = { 300.f, 450.f };
	float dogSpriteScale = 0.75f;
	float dogSpriteAngle = 0.f;

	float cutsceneTimer = 0.f;
	float hopTimer = 0.4166666667;

	WobblyTexture texDogOutline[] = {
		WobblyTexture("resources/dog_outline.png"),
		WobblyTexture("resources/dog_hop1_outline.png"),
		WobblyTexture("resources/dog_hop2_outline.png"),
	};

	WobblyTexture texDogBack[] = { 
		WobblyTexture("resources/dog_back.png"),
		WobblyTexture("resources/dog_hop1_back.png"),
		WobblyTexture("resources/dog_hop2_back.png"),
	};

	int frame = 0;
	float hopOffset = 0.f;

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

		if (cutsceneTimer < 0)
		{
			hopTimer -= dt;

			hopOffset += (frame == 2 ? -120.f : 70.f) * dt;
			if (hopOffset > 0.f) hopOffset = 0.f;
			if (hopOffset < -20.f) hopOffset = -20.f;

			if (hopTimer <= 0.f)
			{
				hopTimer = 0.4166666667f;
				frame = (frame == 2 ? 1 : 2);
			}
		}
		else if (cutsceneTimer > 2.f)
		{
			cutsceneTimer = -1.f;
			frame = 2;
		}
		else
		{
			cutsceneTimer += dt;
		}

		texDogBack[frame].Update(dt, false, DOG_WOBBLE_RATE);
		texDogOutline[frame].Update(dt, true, DOG_WOBBLE_RATE);

		BeginDrawing();

		ClearBackground(DARKPURPLE);
		Vector2 drawPos = { pos.x, pos.y + hopOffset };
		texDogBack[frame].Draw(drawPos, {dogSpriteScale, dogSpriteScale}, dogSpriteAngle);
		texDogOutline[frame].Draw(drawPos, {dogSpriteScale, dogSpriteScale}, dogSpriteAngle);

		EndDrawing();
	}

	for (int i = 0; i < 3; i++)
	{
		texDogOutline[i].Unload();
		texDogBack[i].Unload();
	}
	CloseWindow();
	return 0;
}
