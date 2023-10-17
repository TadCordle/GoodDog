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
	Texture2D texPaintLightGreen = LoadTexture("resources/paint_lightgreen.png");
	Texture2D texPaintLightBlue = LoadTexture("resources/paint_lightblue.png");
	Texture2D texBG = LoadTexture("resources/bg.png");

	GameState state = CUTSCENE;

	Game* game = new Game();
	game->AddFloor({ 0.f, 552.f }, { 1280.f, 552.f });
	game->AddFloor({ 1000.f, 300.f }, { 1280.f, 300.f });
	game->AddElevator({ 500.f, 300.f }, { 1000.f, 300.f }, { 500.f, 552.f }, { 1000.f, 552.f }, 0.3f, Button::W);
	game->AddDangerBlock({ 1030.f, 426.f }, { 1030.f, 426.f }, { 60.f, 220.f }, Button::A);

	game->camera.offset = { 0.f, 0.f };
	game->camera.rotation = 0.f;
	game->camera.zoom = 1.f;
	game->camera.target = { 0.f, 0.f };

	WobblyTexture dogOutline, dogBack;
	
	// Cutscene state
	float cutsceneTimer = 0.f;

	// Gameplay state
	double hopTimer = -HOP_TIMER / 2.f;
	int frame = 0;
	float hopOffset = 0.f;
	float fallingSpeed = 0.f;

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		if (dt > 0.03333333f) dt = 0.03333333f;

		Rectangle dogHitBox = { pos.x - 48.f, pos.y - 48.f, 96.f, 96.f };

		for (int i = 0; i < game->floorsCount; i++)
		{
			game->floors[i].Update(dt, WALL_WOBBLE_RATE);
		}
		for (int i = 0; i < game->elevatorsCount; i++)
		{
			game->elevators[i].Update(dt, WALL_WOBBLE_RATE);
		}
		for (int i = 0; i < game->dangerBlocksCount; i++)
		{
			game->dangerBlocks[i].Update(dt, WALL_WOBBLE_RATE);
		}

		switch (state)
		{
		case CUTSCENE:
		{
			cutsceneTimer += dt;
			if (cutsceneTimer < 0.5f)
			{
				//printf("Waiting\n");
			}
			else if (cutsceneTimer < 1.f)
			{
				//printf("Pre-throw\n");
			}
			else if (cutsceneTimer < 1.5f)
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

				pos = Vector2Add(pos, Vector2Scale(dogRight, (dogFlipped ? -1.f : 1.f) * 280.f * dt));

				hopOffset += (frame == 2 ? -100.f : 100.f) * dt;
				if (hopOffset > 0.f) hopOffset = 0.f;
				if (hopOffset < -20.f) hopOffset = -20.f;

				hopTimer += dt;
				double timeWithinBeat = remainder(hopTimer, HOP_TIMER);
				frame = timeWithinBeat >= 0.0 ? 1 : 2;
				// TODO: Use GetMusicTimePlayed(music) to ensure we're synced up, in case someone drags the window and pauses the game or something
			}

			// TODO: (?) Check collision with camera zone to change camera parameters?

			// TODO: Check if walking over curve; initiate rotation if so

			// Check if we're standing on anything
			float minFloorDist = FLT_MAX;
			Vector2 minPos = {};
			auto CheckOnFloor = [&](Vector2 start, Vector2 end)
			{
				Vector2 floorCenter = Vector2Lerp(start, end, 0.5f);
				float dogXToFloor = Vector2DotProduct(Vector2Subtract(pos, floorCenter), dogRight);
				float floorLength = Vector2DotProduct(Vector2Subtract(end, start), dogRight);
				if (fabs(dogXToFloor) < floorLength / 2.f + 64.f)
				{
					Vector2 closestFloorPos = Vector2Lerp(start, end, dogXToFloor / floorLength + 0.5f);
					float dist = Vector2DotProduct(Vector2Subtract(pos, closestFloorPos), dogUp);
					if (dist > 60.f && dist < 140.f && dist < minFloorDist)
					{
						minFloorDist = dist;
						minPos = Vector2Add(closestFloorPos, Vector2Scale(dogUp, 100.f));
					}
				}
			};

			// Elevators
			for (int i = 0; i < game->elevatorsCount; i++)
			{
				Elevator& elevator = game->elevators[i];
				CheckOnFloor(elevator.GetCurrentStart(), elevator.GetCurrentEnd());
			}

			// Normal floors
			for (int i = 0; i < game->floorsCount; i++)
			{
				Floor& floor = game->floors[i];
				CheckOnFloor(floor.start, floor.end);
			}

			// Are we falling?
			if (minFloorDist == FLT_MAX || minFloorDist > 100.f)
			{
				fallingSpeed += 1000.f * dt;
				pos = Vector2Add(pos, Vector2Scale(dogUp, -fallingSpeed * dt));
			}
			else
			{
				pos = minPos;
				fallingSpeed = 0.f;
			}

			// TODO: Check collision with reverser

			// Check collision with obstacle
			for (int i = 0; i < game->dangerBlocksCount; i++)
			{
				float dogTop = dogHitBox.y - dogHitBox.height / 2.f;
				float dogBot = dogHitBox.y + dogHitBox.height / 2.f;
				float dogLeft = dogHitBox.x - dogHitBox.width / 2.f;
				float dogRight = dogHitBox.x + dogHitBox.width / 2.f;

				DangerBlock& block = game->dangerBlocks[i];
				Vector2 blockPos = block.GetCurrentPos();
				float blockTop = blockPos.y - block.dimensions.y;
				float blockBot = blockPos.y + block.dimensions.y;
				float blockLeft = blockPos.x - block.dimensions.x;
				float blockRight = blockPos.x + block.dimensions.x;

				if (dogTop < blockBot && dogBot > blockTop && dogLeft < blockRight && dogRight > blockLeft)
				{
					state = LOSE;
					// TODO: play sad sound
					break;
				}
			}

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
			frame = 2;
			// TODO: Make dog fall off screen
			break;
		}
		}

		dogBack.Update(dt, DOG_WOBBLE_RATE);
		dogOutline.Update(dt, DOG_WOBBLE_RATE);
		
		BeginDrawing();
		{
			ClearBackground(DARKPURPLE);
			DrawTexture(texBG, 0, 0, WHITE);

			BeginMode2D(game->camera);

			for (int i = 0; i < game->floorsCount; i++)
			{
				Floor& floor = game->floors[i];
				floor.Draw(texLine, texPaintBlue);
			}
			for (int i = 0; i < game->elevatorsCount; i++)
			{
				Elevator& elevator = game->elevators[i];
				elevator.Draw(texLine, texPaintLightGreen);
			}
			for (int i = 0; i < game->dangerBlocksCount; i++)
			{
				DangerBlock& dangerBlock = game->dangerBlocks[i];
				dangerBlock.Draw(texLine, texPaintGray);
			}

			Vector2 drawPos = Vector2Add({ pos.x, pos.y + hopOffset}, Vector2Scale(dogRight, dogFlipped ? -12.f : 12.f));
			dogBack.Draw(texDogBack[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, false);
			dogOutline.Draw(texDogOutline[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, true);
			DrawRectangleLines((int)dogHitBox.x, (int)dogHitBox.y, (int)dogHitBox.width, (int)dogHitBox.height, RED);

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
