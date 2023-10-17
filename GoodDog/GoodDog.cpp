#include "Game.h"

int main()
{
	InitWindow(1280, 720, "Good Dog");
	SetTargetFPS(120);
	//ToggleFullscreen();

	InitAudioDevice();

	Music music = LoadMusicStream("resources/music.wav");

	Vector2 pos = { 600.f, 550.f };
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
	Texture2D texDogLose = LoadTexture("resources/dog_lose_outline.png");
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
	Texture2D texReverserBackEnabled = LoadTexture("resources/reverser_enabled.png");
	Texture2D texReverserBackDisabled = LoadTexture("resources/reverser_disabled.png");
	Texture2D texReverserOutline = LoadTexture("resources/reverser_outline.png");
	Texture2D texReverserArrows = LoadTexture("resources/reverser_arrows.png");
	Texture2D texCurveSolid = LoadTexture("resources/curve_solid.png");
	Texture2D texCurveOutline = LoadTexture("resources/curve_outline.png");
	Texture2D texBG = LoadTexture("resources/bg.png");

	const char* loseText = "Bad dog :(";
	const char* winText = "Good dog :)";
	int loseTextWidth = MeasureText(loseText, 80);
	int winTextWidth = MeasureText(winText, 80);

	GameState state = CUTSCENE;

	Game* game = new Game();
	//game->AddFloor({ 1000.f, 300.f }, { 1280.f, 300.f });
	//game->AddElevator({ 500.f, 300.f }, { 1000.f, 300.f }, { 500.f, 552.f }, { 1000.f, 552.f }, 0.3f, Button::W);
	//game->AddReverser({ 30.f, 426.f }, { 30.f, 426.f }, Right, Button::A);
	//game->AddReverser({ 1030.f, 426.f }, { 1030.f, 426.f }, Left, Button::A);
	//game->AddDangerBlock({ 1030.f, 426.f }, { 1030.f, 426.f }, { 60.f, 220.f }, Button::A);

	game->AddFloor({ 128.f, 652.f }, { 1152.f, 652.f });
	game->AddCurve({ 1216.f, 604.f }, SE);
	game->AddFloor({ 1260.f, 540.f }, { 1260.f, 252.f });
	game->AddCurve({ 1216.f, 192.f }, NE);
	game->AddFloor({ 128.f, 144.f }, { 1152.f, 144.f });
	game->AddCurve({ 64.f, 192.f }, NW);
	game->AddFloor({ 18.f, 540.f }, { 18.f, 252.f });
	game->AddCurve({ 64.f, 604.f }, SW);
	game->AddReverser({ 350.f, 526.f }, { 30.f, 426.f }, Left, Button::A);
	game->AddDangerBlock({ 850.f, 526.f }, { 650.f, 526.f }, { 60.f, 220.f }, Button::A);

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
	DogRotationTarget currentRotTarget;

	auto Lose = [&]()
	{
		fallingSpeed = -900.f;
		state = LOSE;
		// TODO: play sad sound
	};

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		if (dt > 0.03333333f) dt = 0.03333333f;

		for (int i = 0; i < game->dangerBlocksCount; i++)
			game->dangerBlocks[i].Update(dt, WALL_WOBBLE_RATE);
		for (int i = 0; i < game->floorsCount; i++)
			game->floors[i].Update(dt, WALL_WOBBLE_RATE);
		for (int i = 0; i < game->elevatorsCount; i++)
			game->elevators[i].Update(dt, WALL_WOBBLE_RATE);
		for (int i = 0; i < game->reversersCount; i++)
			game->reversers[i].Update(dt, WALL_WOBBLE_RATE);

		Rectangle dogHitBox = { pos.x - 48.f, pos.y - 48.f, 96.f, 96.f };

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

			// Update dog position
			{
				// Rotate when walking on curves
				if (currentRotTarget.angularSpeed != 0.f)
				{
					dogAngle += currentRotTarget.angularSpeed * dt;
					if (currentRotTarget.angularSpeed > 0 && dogAngle >= currentRotTarget.targetAngle ||
						currentRotTarget.angularSpeed < 0 && dogAngle <= currentRotTarget.targetAngle)
					{
						dogAngle = currentRotTarget.targetAngle;
						while (dogAngle < 0.f) dogAngle += 360.f;
						while (dogAngle >= 360.f) dogAngle -= 360.f;
						currentRotTarget = DogRotationTarget();
					}
				}
				dogUp = { sinf(dogAngle * DEG2RAD), -cosf(dogAngle * DEG2RAD) };
				dogRight = Vector2Scale({ cosf(dogAngle * DEG2RAD), sinf(dogAngle * DEG2RAD) }, dogFlipped ? -1.f : 1.f);
				
				pos = Vector2Add(pos, Vector2Scale(dogRight, 280.f * dt));

				// Dog hopping
				hopOffset += (frame == 2 ? -100.f : 100.f) * dt;
				if (hopOffset > 0.f) hopOffset = 0.f;
				if (hopOffset < -20.f) hopOffset = -20.f;

				hopTimer += dt;
				double timeWithinBeat = remainder(hopTimer, HOP_TIMER);
				frame = timeWithinBeat >= 0.0 ? 1 : 2;
				// TODO: Use GetMusicTimePlayed(music) to ensure we're synced up, in case someone drags the window and pauses the game or something
			}

			// Update camera when you hit a camera zone
			for (int i = 0; i < game->cameraZonesCount; i++)
			{
				CameraZone& cameraZone = game->cameraZones[i];
				if (cameraZone.ContainsPoint(pos))
					game->camera = cameraZone.params;
			}
			
			// Check if walking over curve; initiate rotation if so
			for (int i = 0; i < game->curvesCount; i++)
			{
				Curve& curve = game->curves[i];
				DogRotationTarget rotTarget = curve.GetRotationTarget(pos, dogUp, dogRight);
				if (rotTarget.angularSpeed != 0.f)
					currentRotTarget = rotTarget;
			}

			// Check if we're standing on anything
			float minFloorDist = FLT_MAX;
			Vector2 minPos = {};
			auto CheckOnFloor = [&](Vector2 start, Vector2 end)
			{
				Vector2 floorCenter = Vector2Lerp(start, end, 0.5f);
				float dogXToFloor = Vector2DotProduct(Vector2Subtract(pos, floorCenter), dogRight);
				float floorLength = Vector2DotProduct(Vector2Subtract(end, start), dogRight);
				if (fabs(dogXToFloor) < fabs(floorLength) / 2.f + 64.f)
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
			if (currentRotTarget.angularSpeed == 0.f)
			{
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
			}

			// AABB check
			float dogHBoxTop = dogHitBox.y;
			float dogHBoxBot = dogHitBox.y + dogHitBox.height;
			float dogHBoxLeft = dogHitBox.x;
			float dogHBoxRight = dogHitBox.x + dogHitBox.width;
			auto CheckDogHit = [&](Vector2 _pos, float _width, float _height)
			{
				float blockTop = _pos.y - _height / 2.f;
				float blockBot = _pos.y + _height / 2.f;
				float blockLeft = _pos.x - _width / 2.f;
				float blockRight = _pos.x + _width / 2.f;
				return dogHBoxTop < blockBot && dogHBoxBot > blockTop && dogHBoxLeft < blockRight && dogHBoxRight > blockLeft;
			};

			// Check collision with reverser
			for (int i = 0; i < game->reversersCount; i++)
			{
				Reverser& block = game->reversers[i];
				Vector2 blockPos = block.GetCurrentPos();
				if (CheckDogHit(blockPos, 60, 220))
				{
					if (block.enabled == 1.f)
					{
						dogFlipped = !dogFlipped;
						block.enabled -= dt;
						// TODO: play reverse sound
					}
					else if (block.enabled == 0.f)
					{
						Lose();
					}
					break;
				}
			}

			// Check collision with obstacle
			for (int i = 0; i < game->dangerBlocksCount; i++)
			{
				DangerBlock& block = game->dangerBlocks[i];
				Vector2 blockPos = block.GetCurrentPos();
				if (CheckDogHit(blockPos, block.dimensions.x, block.dimensions.y))
				{
					Lose();
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
			StopMusicStream(music); // TODO: Fade music out to avoid pop
			frame = 2;
			fallingSpeed += 3000.f * dt;
			pos = Vector2Add(pos, Vector2Scale(dogUp, -fallingSpeed * dt));
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

			for (int i = 0; i < game->dangerBlocksCount; i++)
				game->dangerBlocks[i].Draw(texLine, texPaintGray);
			for (int i = 0; i < game->curvesCount; i++)
				game->curves[i].Draw(texCurveOutline, texCurveSolid);
			for (int i = 0; i < game->floorsCount; i++)
				game->floors[i].Draw(texLine, texPaintBlue);
			for (int i = 0; i < game->elevatorsCount; i++)
				game->elevators[i].Draw(texLine, texPaintLightGreen);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].Draw(texReverserBackEnabled, texReverserBackDisabled, texReverserOutline, texReverserArrows);

			Vector2 offsetPos = Vector2Add(pos, Vector2Scale(dogUp, -hopOffset));
			Vector2 drawPos = Vector2Add(offsetPos, Vector2Scale(dogRight, dogFlipped ? -12.f : 12.f));
			dogBack.Draw(texDogBack[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, false);
			dogOutline.Draw(state == LOSE ? texDogLose : texDogOutline[frame], drawPos, { dogSpriteScale, dogSpriteScale }, dogAngle, dogFlipped, true);
			//DrawRectangleLines((int)pos.x - 8.f, (int)pos.y - 8.f, 16, 16, RED);

			if (state == LOSE)
			{
				DrawText(loseText, 640 - loseTextWidth / 2, 320, 80, BLACK);
				DrawText(loseText, 643 - loseTextWidth / 2, 323, 80, WHITE);
			}
			else if (state == WIN)
			{
				DrawText(winText, 640 - winTextWidth / 2, 320, 80, BLACK);
				DrawText(winText, 643 - winTextWidth / 2, 323, 80, WHITE);
			}

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
