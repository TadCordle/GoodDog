#include "Game.h"

int main()
{
	InitWindow(1280, 720, "Good Dog");
	SetTargetFPS(120);
	//ToggleFullscreen();

	InitAudioDevice();

	Music music = LoadMusicStream("resources/music.wav");

	Vector2 pos = { 300.f, 450.f };
	float dogSpriteScale = 0.75f;
	float dogSpriteAngle = 0.f;
	bool dogFlipped = true;

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
	Texture2D texLine = LoadTexture("resources/line.png");
	Texture2D texPaintBlue = LoadTexture("resources/paint_blue.png");
	Texture2D texPaintGray = LoadTexture("resources/paint_gray.png");
	Texture2D texBG = LoadTexture("resources/bg.png");

	GameState state = CUTSCENE;

	Game* game = new Game(texLine, texPaintBlue);
	game->AddFloor({ 0.f, 552.f }, { 1280.f, 552.f });

	WobblyRectangle rectTest(texLine, texPaintGray, { 800.f, 400.f }, { 100.f, 280.f });

	// Cutscene state
	float cutsceneTimer = 0.f;

	// Gameplay state
	double hopTimer = -HOP_TIMER / 2.f;
	int frame = 0;
	float hopOffset = 0.f;

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		if (dt > 0.03333333f) dt = 0.03333333f;

		switch (state)
		{
		case CUTSCENE:
		{
			cutsceneTimer += dt;
			if (cutsceneTimer < 1.f)
			{
				//printf("Waiting\n");
			}
			else if (cutsceneTimer < 1.5f)
			{
				//printf("Pre-throw\n");
			}
			else if (cutsceneTimer < 1.6f)
			{
				//printf("Throw\n");
				dogFlipped = false;
			}
			else if (cutsceneTimer < 2.0f)
			{
				//printf("Fetch!\n");
			}
			else
			{
				PlayMusicStream(music);
				frame = 2;
				state = GOING;
			}
			break;
		}
		case GOING:
		{
			UpdateMusicStream(music);

			if (pos.x > 1280.f) dogFlipped = true;
			if (pos.x < 0.f) dogFlipped = false;

			pos.x += (dogFlipped ? -1.f : 1.f) * 280.f * dt;

			hopOffset += (frame == 2 ? -100.f : 100.f) * dt;
			if (hopOffset > 0.f) hopOffset = 0.f;
			if (hopOffset < -20.f) hopOffset = -20.f;

			hopTimer += dt;
			double timeWithinBeat = remainder(hopTimer, HOP_TIMER);
			frame = timeWithinBeat >= 0.0 ? 1 : 2;
			// TODO: Use GetMusicTimePlayed(music) to ensure we're synced up, in case someone drags the window and pauses the game or something

			break;
		}
		case WIN:
		{
			
			// TODO
			break;
		}
		case LOSE:
		{
			StopMusicStream(music);

			// TODO
			break;
		}
		}

		texDogBack[frame].Update(dt, false, DOG_WOBBLE_RATE);
		texDogOutline[frame].Update(dt, true, DOG_WOBBLE_RATE);

		rectTest.Update(dt, WALL_WOBBLE_RATE);

		for (int i = 0; i < game->floorRendersCount; i++)
		{
			game->floorRenders[i].Update(dt, WALL_WOBBLE_RATE);
		}
		
		BeginDrawing();

		ClearBackground(DARKPURPLE);
		DrawTexture(texBG, 0, 0, WHITE);

		for (int i = 0; i < game->floorRendersCount; i++)
		{
			game->floorRenders[i].Draw();
		}
		rectTest.Draw();
		
		Vector2 drawPos = { pos.x, pos.y + hopOffset };
		texDogBack[frame].Draw(drawPos, { dogSpriteScale, dogSpriteScale }, dogSpriteAngle, dogFlipped);
		texDogOutline[frame].Draw(drawPos, { dogSpriteScale, dogSpriteScale }, dogSpriteAngle, dogFlipped);

		//DrawFPS(10, 10);

		EndDrawing();
	}

	for (int i = 0; i < 3; i++)
	{
		texDogOutline[i].Unload();
		texDogBack[i].Unload();
	}
	UnloadTexture(texLine);
	UnloadTexture(texPaintBlue);
	CloseWindow();

	delete game;
	return 0;
}
