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
	float dogAngle = 0.f;
	bool dogFlipped = true;
	Vector2 dogUp = { 0.f, -1.f };
	Vector2 dogRight = { 1.f, 0.f };

	Texture2D texDogOutline[] = {
		LoadTexture("resources/dog_outline.png"),
		LoadTexture("resources/dog_hop1_outline.png"),
		LoadTexture("resources/dog_hop2_outline.png"),
	};
	Texture2D texDogBack[] = { 
		LoadTexture("resources/dog_back.png"),
		LoadTexture("resources/dog_hop1_back.png"),
		LoadTexture("resources/dog_hop2_back.png"),
	};
	Texture2D texLine = LoadTexture("resources/line.png");
	Texture2D texPaintBlue = LoadTexture("resources/paint_blue.png");
	Texture2D texPaintGray = LoadTexture("resources/paint_gray.png");
	Texture2D texBG = LoadTexture("resources/bg.png");

	GameState state = CUTSCENE;

	Game* game = new Game();
	game->AddFloor({ 0.f, 300.f }, { 1280.f, 552.f });
	game->AddFloor({ 0.f, 552.f }, { 1280.f, 300.f });

	game->camera.offset = { 0.f, 0.f };
	game->camera.rotation = 0.f;
	game->camera.zoom = 1.f;
	game->camera.target = { 0.f, 0.f };

	WobblyTexture dogOutline, dogBack;
	WobblyRectangle rectTest;

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

		Rectangle dogHitBox = { pos.x - 64.f, pos.y - 32.f, 128.f, 96.f };

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

			// Dog hopping
			{
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
			}

			// TODO: Check if walking over curve; initiate rotation if so

			float minFloorDist = FLT_MAX;
			Vector2 minPos = {};

			// TODO: Check if "above" elevator, if so match height

			for (int i = 0; i < game->floorsCount; i++)
			{
				// TODO take rotation into account oh god
				Floor& floor = game->floors[i];
				Vector2 floorCenter = Vector2Lerp(floor.start, floor.end, 0.5f);
				float dogXToFloor = Vector2DotProduct(Vector2Subtract(pos, floorCenter), dogRight);
				float floorLength = Vector2DotProduct(Vector2Subtract(floor.end, floor.start), dogRight);
				if (fabs(dogXToFloor) < floorLength / 2.f + dogHitBox.width / 2.f)
				{
					Vector2 closestFloorPos = Vector2Lerp(floor.start, floor.end, dogXToFloor / floorLength + 0.5f);
					float dist = Vector2DotProduct(Vector2Subtract(pos, closestFloorPos), dogUp);
					if (dist > 80.f && dist < 120.f && dist < minFloorDist)
					{
						printf("%f\n", dist);
						minFloorDist = dist;
						minPos = Vector2Add(closestFloorPos, Vector2Scale(dogUp, 100.f));
					}
				}
			}

			// TODO: Check if obstacle

			if (minFloorDist == FLT_MAX)
			{
				// TODO: Fall
			}
			else
			{
				pos = minPos;
			}

			// TODO: Check collision with obstacle

			// TODO: Check collision with reverser

			// TODO: (?) Check collision with camera zone to change camera parameters?

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

		dogBack.Update(dt, DOG_WOBBLE_RATE);
		dogOutline.Update(dt, DOG_WOBBLE_RATE);

		rectTest.Update(dt, WALL_WOBBLE_RATE);

		for (int i = 0; i < game->floorsCount; i++)
		{
			game->floors[i].Update(dt, WALL_WOBBLE_RATE);
		}
		
		BeginDrawing();
		{
			ClearBackground(DARKPURPLE);
			DrawTexture(texBG, 0, 0, WHITE);

			BeginMode2D(game->camera);

			for (int i = 0; i < game->floorsCount; i++)
			{
				Floor& floor = game->floors[i];
				floor.Draw(texLine, texPaintBlue);
				//DrawRectangleLines((int)floor.start.x, (int)floor.start.y - 48.f, floor.end.x - floor.start.x, 64.f, BLUE);
			}
			rectTest.Draw(texLine, texPaintGray, { 750.f, 260.f }, { 850.f, 540.f });

			Vector2 drawPos = { pos.x, pos.y + hopOffset };
			dogBack.Draw(texDogBack[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, false);
			dogOutline.Draw(texDogOutline[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, true);

			//DrawRectangleLines((int)dogHitBox.x, (int)dogHitBox.y, (int)dogHitBox.width, (int)dogHitBox.height, RED);

			EndMode2D();

			//DrawFPS(10, 10);
		}
		EndDrawing();
	}

	for (int i = 0; i < 3; i++)
	{
		UnloadTexture(texDogBack[i]);
		UnloadTexture(texDogOutline[i]);
	}
	UnloadTexture(texLine);
	UnloadTexture(texPaintBlue);
	CloseWindow();

	delete game;
	return 0;
}
