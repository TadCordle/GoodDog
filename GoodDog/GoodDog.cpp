#include "Game.h"

int main()
{
	InitWindow(1280, 720, "Good Dog");
	SetTargetFPS(120);

	InitAudioDevice();

	Music music = LoadMusicStream("resources/music.wav");

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

	WobblyTexture dogOutline, dogBack;

	Font font = LoadFontEx("resources/GamjaFlower-Regular.ttf", 80, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
	const char* loseText = "Bad dog :(";
	const char* winText = "Good dog :)";
	int loseTextWidth = MeasureTextEx(font, loseText, 80, 0).x;
	int winTextWidth = MeasureTextEx(font, winText, 80, 0).x;

	GameState state = CUTSCENE;
	GameState prevState = CUTSCENE;
	EditorState editor;

	Game* game = new Game();
	game->dogStartingPos = { 600.f, 550.f };
	game->Deserialize("resources/level.txt");
	
	game->camera.offset = { 0.f, 0.f };
	game->camera.rotation = 0.f;
	game->camera.zoom = 1.f;
	game->camera.target = { 0.f, 0.f };

	// Gameplay state
	Vector2 pos = game->dogStartingPos;
	float dogAngle = 0.f;
	bool dogFlipped = true;
	Vector2 dogUp = { 0.f, -1.f };
	Vector2 dogRight = { 1.f, 0.f };

	double hopTimer = -HOP_TIMER / 2.f;
	int frame = 0;
	float hopOffset = 0.f;
	float fallingSpeed = 0.f;
	DogRotationTarget currentRotTarget;

	// Cutscene state
	float cutsceneTimer = 0.f;

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

		// Toggle between editor and play mode
		if (IsKeyPressed(KeyboardKey::KEY_F5))
		{
			if (state == EDITOR)
			{
				state = prevState;
				if (state == GOING)
					PlayMusicStream(music);
			}
			else
			{
				prevState = state;
				state = EDITOR;
				PauseMusicStream(music);
			}
		}

		// Reset
		if (IsKeyPressed(KeyboardKey::KEY_F6))
		{
			state = CUTSCENE;
			cutsceneTimer = 0.f;
			pos = game->dogStartingPos;
			dogAngle = 0.f;
			dogFlipped = true;
			hopTimer = -HOP_TIMER / 2.f;
			frame = 0;
			hopOffset = 0.f;
			fallingSpeed = 0.f;
			currentRotTarget = DogRotationTarget();
			StopMusicStream(music);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].enabled = 1.f;
		}

		// Toggle fullscreen
		if (IsKeyPressed(KeyboardKey::KEY_F11))
			ToggleFullscreen();

		// Update entities
		if (state != EDITOR)
		{
			for (int i = 0; i < game->dangerBlocksCount; i++)
				game->dangerBlocks[i].Update(dt, WALL_WOBBLE_RATE);
			for (int i = 0; i < game->floorsCount; i++)
				game->floors[i].Update(dt, WALL_WOBBLE_RATE);
			for (int i = 0; i < game->elevatorsCount; i++)
				game->elevators[i].Update(dt, WALL_WOBBLE_RATE);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].Update(dt, WALL_WOBBLE_RATE);
		}

		Rectangle dogHitBox = { pos.x - 48.f, pos.y - 48.f, 96.f, 96.f };

		// Update camera when you hit a camera zone
		if (state != EDITOR)
		{
			for (int i = 0; i < game->cameraZonesCount; i++)
			{
				CameraZone& cameraZone = game->cameraZones[i];
				if (cameraZone.ContainsPoint(pos))
					game->camera = cameraZone.params;
			}
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
				
				pos = Vector2Add(pos, Vector2Scale(dogRight, 290.f * dt));

				// Dog hopping
				hopOffset += (frame == 2 ? -120.f : 120.f) * dt;
				if (hopOffset > 0.f) hopOffset = 0.f;
				if (hopOffset < -30.f) hopOffset = -30.f;

				hopTimer += dt;
				double timeWithinBeat = remainder(hopTimer, HOP_TIMER);
				frame = timeWithinBeat >= 0.0 ? 1 : 2;
				// TODO: Use GetMusicTimePlayed(music) to ensure we're synced up, in case someone drags the window and pauses the game or something
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
		case EDITOR:
		{
			if (IsKeyDown(KeyboardKey::KEY_LEFT_CONTROL))
			{
				if (IsKeyPressed(KeyboardKey::KEY_S))
				{
					game->Serialize("resources/level.txt");
				}
			}

			// Zoom
			float mouseWheel = GetMouseWheelMove();
			if (mouseWheel != 0.f)
			{
				Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), game->camera);
				game->camera.offset = GetMousePosition();
				game->camera.target = mouseWorldPos;

				// zoom
				game->camera.zoom += mouseWheel * 0.125f;
				if (game->camera.zoom < 0.125f) game->camera.zoom = 0.125f;
				if (game->camera.zoom > 3.000f) game->camera.zoom = 3.000f;
			}

			// Pan
			if (IsMouseButtonDown(MouseButton::MOUSE_BUTTON_MIDDLE))
			{
				Vector2 mouseOffset = GetMouseDelta();
				mouseOffset = Vector2Scale(mouseOffset, -1.0f / game->camera.zoom);
				game->camera.target = Vector2Add(game->camera.target, mouseOffset);
			}

			editor.placingPos = GetScreenToWorld2D(GetMousePosition(), game->camera);
			editor.placingPos.x = ((int)(editor.placingPos.x / 4.f)) * 4.f;
			editor.placingPos.y = ((int)(editor.placingPos.y / 4.f)) * 4.f;
			auto SnapPlacingPos = [&](Vector2 other)
			{
				Vector2 diff = Vector2Subtract(editor.placingPos, other);
				if (fabs(diff.x) > fabs(diff.y))
					editor.placingPos.y = other.y;
				else
					editor.placingPos.x = other.x;
			};

			// Select item
			if (IsKeyPressed(KeyboardKey::KEY_KP_0)) editor.UpdatePlacing(ATNone);
			if (IsKeyPressed(KeyboardKey::KEY_KP_1)) editor.UpdatePlacing(ATFloor);
			if (IsKeyPressed(KeyboardKey::KEY_KP_2)) editor.UpdatePlacing(ATCurve);
			if (IsKeyPressed(KeyboardKey::KEY_KP_3)) editor.UpdatePlacing(ATElevator);
			if (IsKeyPressed(KeyboardKey::KEY_KP_4)) editor.UpdatePlacing(ATDangerBlock);
			if (IsKeyPressed(KeyboardKey::KEY_KP_5)) editor.UpdatePlacing(ATReverser);
			if (IsKeyPressed(KeyboardKey::KEY_KP_6)) editor.UpdatePlacing(ATCameraZone);

			if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT))
			{
				// If curve or reverser is selected, choose direction
				if (editor.placingAsset == ATCurve || (editor.placingAsset == ATReverser && editor.placingStep == 0))
				{
					editor.v5++;
					if (editor.v5 > 3.f)
						editor.v5 = 0.f;
				}
				else
				{
					// Right click removal

					bool removedSomething = false;

					auto IsMouseOverLine = [&](Vector2 start, Vector2 end)
					{
						Vector2 floorCenter = Vector2Lerp(start, end, 0.5f);
						Vector2 floorRight = Vector2Normalize(Vector2Subtract(end, start));
						Vector2 floorUp = { floorRight.y, -floorRight.x };
						float mouseXToFloor = Vector2DotProduct(Vector2Subtract(editor.placingPos, floorCenter), floorRight);
						float floorLength = Vector2DotProduct(Vector2Subtract(end, start), floorRight);
						if (fabs(mouseXToFloor) < fabs(floorLength) / 2.f + 64.f)
						{
							Vector2 closestFloorPos = Vector2Lerp(start, end, mouseXToFloor / floorLength + 0.5f);
							float dist = Vector2DotProduct(Vector2Subtract(editor.placingPos, closestFloorPos), floorUp);
							return abs(dist) < 24.f;
						}
						return false;
					};

					auto IsMouseOverRectangle = [&](Vector2 pos, Vector2 size)
					{
						return editor.placingPos.x < pos.x + size.x / 2.f &&
							   editor.placingPos.x > pos.x - size.x / 2.f &&
							   editor.placingPos.y < pos.y + size.y / 2.f &&
							   editor.placingPos.y > pos.y - size.y / 2.f;
					};

					for (int i = 0; i < game->floorsCount; i++)
					{
						if (IsMouseOverLine(game->floors[i].start, game->floors[i].end))
						{
							game->floors[i] = game->floors[game->floorsCount - 1];
							game->floorsCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->curvesCount; i++)
					{
						if (IsMouseOverRectangle(game->curves[i].pos, { 128.f, 128.f }))
						{
							game->curves[i] = game->curves[game->curvesCount - 1];
							game->curvesCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->elevatorsCount; i++)
					{
						if (IsMouseOverLine(game->elevators[i].start, game->elevators[i].end))
						{
							game->elevators[i] = game->elevators[game->elevatorsCount - 1];
							game->elevatorsCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->dangerBlocksCount; i++)
					{
						if (IsMouseOverRectangle(game->dangerBlocks[i].pos1, game->dangerBlocks[i].dimensions))
						{
							game->dangerBlocks[i] = game->dangerBlocks[game->dangerBlocksCount - 1];
							game->dangerBlocksCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->reversersCount; i++)
					{
						Vector2 dimensions = (game->reversers[i].dir == Left || game->reversers[i].dir == Right) ? Vector2{ 60.f, 220.f } : Vector2{ 220.f, 60.f };
						if (IsMouseOverRectangle(game->reversers[i].pos1, dimensions))
						{
							game->reversers[i] = game->reversers[game->reversersCount - 1];
							game->reversersCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->cameraZonesCount; i++)
					{
						if (game->cameraZones[i].ContainsPoint(editor.placingPos))
						{
							game->cameraZones[i] = game->cameraZones[game->cameraZonesCount - 1];
							game->cameraZonesCount--;
							removedSomething = true;
							break;
						}
					}
				}
			}

			if (editor.placingAsset != ATNone)
			{
				bool clicked = IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT);

				switch (editor.placingAsset)
				{
				case ATFloor:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Foor start
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1)
					{
						// Floor end
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))  SnapPlacingPos(editor.v1);
						if (clicked)
						{
							game->AddFloor(editor.v1, editor.placingPos);
							editor.placingStep = 0;
						}
					}
					break;
				}
				case ATCurve:
				{
					if (clicked)
					{
						CurveType ctype;
						if (editor.v5 == 0.f) ctype = SE;
						else if (editor.v5 == 1.f) ctype = NE;
						else if (editor.v5 == 2.f) ctype = NW;
						else if (editor.v5 == 3.f) ctype = SW;
						game->AddCurve(editor.placingPos, ctype);
						editor.placingStep = 0;
					}
					break;
				}
				case ATElevator:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Elevator start
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1)
					{
						// Elevator end
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))  SnapPlacingPos(editor.v1);
						if (clicked)
						{
							editor.v2 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 2)
					{
						// Elevator start move position
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))  SnapPlacingPos(editor.v1);
						if (clicked)
						{
							editor.v3 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 3)
					{
						// Elevator end move position
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))
						{
							SnapPlacingPos(editor.v2);
							if (IsKeyDown(KeyboardKey::KEY_LEFT_CONTROL)) // Align with first movement point
								SnapPlacingPos(editor.v3);
						}
						if (clicked)
						{
							editor.v4 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 4)
					{
						// Button
						Button button = GetButtonFromKeyPressed();
						if (button != Button::None)
						{
							printf("Button assigned!\n");
							game->AddElevator(editor.v1, editor.v2, editor.v3, editor.v4, 0.4f, button);
							editor.placingStep = 0;
						}
					}
					break;
				}
				case ATDangerBlock:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Top left corner
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1)
					{
						if (clicked)
						{
							// Bottom right corner
							editor.v2 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 2)
					{
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))  SnapPlacingPos(Vector2Scale(Vector2Add(editor.v2, editor.v1), 0.5f));
						if (clicked)
						{
							// Move position
							editor.v3 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 3)
					{
						// Button
						Button button = GetButtonFromKeyPressed();
						if (button != Button::None)
						{
							printf("Button assigned!\n");
							Vector2 pos = Vector2Scale(Vector2Add(editor.v2, editor.v1), 0.5f);
							Vector2 size = Vector2Subtract(editor.v2, editor.v1);
							game->AddDangerBlock(pos, editor.v3, size, button);
							editor.placingStep = 0;
						}
					}
					break;
				}
				case ATReverser:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Position 1
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1)
					{
						if (IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT))  SnapPlacingPos(editor.v1);
						if (clicked)
						{
							// Position 2
							editor.v2 = editor.placingPos;
							editor.placingStep++;
						}
					}
					else if (editor.placingStep == 2)
					{
						// Button
						Direction dir;
						if (editor.v5 == 0.f) dir = Left;
						else if (editor.v5 == 1.f) dir = Up;
						else if (editor.v5 == 2.f) dir = Right;
						else if (editor.v5 == 3.f) dir = Down;
						Button button = GetButtonFromKeyPressed();
						if (button != Button::None)
						{
							printf("Button assigned!\n");
							game->AddReverser(editor.v1, editor.v2, dir, button);
							editor.placingStep = 0;
						}
					}
					break;
				}
				case ATCameraZone:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Top left corner of zone
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1 && clicked)
					{
						// Bottom right corner of zone
						editor.v2 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 2 && clicked)
					{
						// Camera settings
						Vector2 pos = Vector2Scale(Vector2Add(editor.v1, editor.v2), 0.5f);
						Vector2 size = Vector2Subtract(editor.v2, editor.v1);
						game->AddCameraZone(pos, size, game->camera);
						printf("Camera zone assigned! %f %f %f\n", game->camera.offset.x, game->camera.offset.y, game->camera.zoom);
						editor.placingStep = 0;
					}
					break;
				}
				}
			}

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
			dogBack.Draw(texDogBack[frame], drawPos, { 0.75f, 0.75f }, dogAngle, dogFlipped, false);
			dogOutline.Draw(state == LOSE ? texDogLose : texDogOutline[frame], drawPos, { 0.75f, 0.75f }, dogAngle, dogFlipped, true);
			//DrawRectangleLines((int)pos.x - 8.f, (int)pos.y - 8.f, 16, 16, RED);

			// Draw edits in progress
			if (state == EDITOR)
			{
				for (int i = 0; i < game->cameraZonesCount; i++)
				{
					CameraZone& zone = game->cameraZones[i];
					Vector2 pos = { zone.pos.x - zone.size.x / 2.f, zone.pos.y - zone.size.y / 2.f };
					Rectangle zoneRect = { pos.x, pos.y, zone.size.x, zone.size.y };
					DrawRectangleLinesEx(zoneRect, 2.f / game->camera.zoom, SKYBLUE);
				}

				switch (editor.placingAsset)
				{
				case ATFloor:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 12, (int)editor.placingPos.y - 12, 24, 24, BLUE);
					else
					{
						Floor temp(editor.v1, editor.placingPos);
						temp.Draw(texLine, texPaintBlue);
					}
					
					break;
				}
				case ATCurve:
				{
					CurveType ctype;
					if (editor.v5 == 0.f) ctype = SE;
					else if (editor.v5 == 1.f) ctype = NE;
					else if (editor.v5 == 2.f) ctype = NW;
					else if (editor.v5 == 3.f) ctype = SW;

					Curve temp(editor.placingPos, ctype);
					temp.Draw(texCurveOutline, texCurveSolid);
					
					break;
				}
				case ATElevator:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 12, (int)editor.placingPos.y - 12, 24, 24, GREEN);
					else
					{
						Elevator temp(editor.v1, editor.placingStep == 1 ? editor.placingPos : editor.v2, {}, {}, 0.5f, Button::None);
						temp.Draw(texLine, texPaintLightGreen);

						if (editor.placingStep == 2)
							DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.placingPos.x, (int)editor.placingPos.y, GREEN);
						else if (editor.placingStep == 3)
						{
							DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.v3.x, (int)editor.v3.y, GREEN);
							DrawLine((int)editor.v2.x, (int)editor.v2.y, (int)editor.placingPos.x, (int)editor.placingPos.y, GREEN);
						}
						else if (editor.placingStep > 3)
						{
							DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.v3.x, (int)editor.v3.y, GREEN);
							DrawLine((int)editor.v2.x, (int)editor.v2.y, (int)editor.v4.x, (int)editor.v4.y, GREEN);
						}
					}
					break;
				}
				case ATDangerBlock:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 12, (int)editor.placingPos.y - 12, 24, 24, WHITE);
					else
					{
						Vector2 botRight = editor.placingStep == 1 ? editor.placingPos : editor.v2;
						Vector2 pos = Vector2Scale(Vector2Add(editor.v1, botRight), 0.5f);
						Vector2 size = Vector2Subtract(botRight, editor.v1);
						DangerBlock temp(pos, {}, size, Button::None);
						temp.Draw(texLine, texPaintGray);

						if (editor.placingStep > 1)
						{
							Vector2 pos2 = editor.placingStep == 2 ? editor.placingPos : editor.v3;
							DrawLine((int)pos.x, (int)pos.y, (int)pos2.x, (int)pos2.y, WHITE);
						}
					}
					break;
				}
				case ATReverser:
				{
					Direction dir;
					if (editor.v5 == 0.f) dir = Left;
					else if (editor.v5 == 1.f) dir = Up;
					else if (editor.v5 == 2.f) dir = Right;
					else if (editor.v5 == 3.f) dir = Down;

					Reverser temp(editor.placingStep == 0 ? editor.placingPos : editor.v1, {}, dir, Button::None);
					temp.Draw(texReverserBackEnabled, texReverserBackDisabled, texReverserOutline, texReverserArrows);
					if (editor.placingStep == 1)
						DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.placingPos.x, (int)editor.placingPos.y, GREEN);
					break;
				}
				case ATCameraZone:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 12, (int)editor.placingPos.y - 12, 24, 24, SKYBLUE);
					else
					{
						Vector2 botRight = editor.placingStep == 1 ? editor.placingPos : editor.v2;
						Vector2 size = Vector2Subtract(botRight, editor.v1);
						Rectangle zoneRect = { editor.v1.x, editor.v1.y, size.x, size.y };
						DrawRectangleLinesEx(zoneRect, 2.f / game->camera.zoom, SKYBLUE);
					}
					break;
				}
				}
			}

			EndMode2D();

			if (state == LOSE)
			{
				// TODO: Wobble!
				DrawTextEx(font, loseText, { 640.f - loseTextWidth / 2, 320.f }, 80, 0, BLACK);
				DrawTextEx(font, loseText, { 643.f - loseTextWidth / 2, 323.f }, 80, 0, WHITE);
			}
			else if (state == WIN)
			{
				// TODO: Wobble
				DrawText(winText, 640 - winTextWidth / 2, 320, 80, BLACK);
				DrawText(winText, 643 - winTextWidth / 2, 323, 80, WHITE);
			}

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
	UnloadMusicStream(music);
	CloseWindow();

	delete game;
	return 0;
}
