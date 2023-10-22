#include "Game.h"

#define PLAY_MUSIC 1

int main()
{
	InitWindow(1280, 720, "Good Dog");
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	Image icon = LoadImage("resources/dog_icon.png");
	SetWindowIcon(icon);

	InitAudioDevice();

	Music music = LoadMusicStream("resources/music.wav");
	SetMusicVolume(music, PLAY_MUSIC ? 0.8f : 0.f);
	Music hihatMusic = LoadMusicStream("resources/music_hihat.wav");
	SetMusicVolume(hihatMusic, PLAY_MUSIC ? 0.8f : 0.f);

	Sound sfxThrow = LoadSound("resources/throw.wav");
	Sound sfxWoof = LoadSound("resources/woof.wav");
	Sound sfxButton[7] = {
		LoadSound("resources/button1.wav"),
		LoadSound("resources/button2.wav"),
		LoadSound("resources/button3.wav"),
		LoadSound("resources/button4.wav"),
		LoadSound("resources/button5.wav"),
		LoadSound("resources/button6.wav"),
		LoadSound("resources/button7.wav")
	};
	Sound sfxReverse = LoadSound("resources/reverse.wav");
	Sound sfxLose = LoadSound("resources/lose.wav");
	Sound sfxWin = LoadSound("resources/cheer.wav");
	Sound sfxThunder = LoadSound("resources/thunder.wav");

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
	Texture2D texItems[] = {
		LoadTexture("resources/sunglasses.png"),
		LoadTexture("resources/hat.png"),
		LoadTexture("resources/ball.png")
	};
	Texture2D texGuyOutline[] = {
		LoadTexture("resources/guy_neutral_outline.png"),
		LoadTexture("resources/guy_prethrow_outline.png"),
		LoadTexture("resources/guy_throw_outline.png")
	};
	Texture2D texGuyBack[] = {
		LoadTexture("resources/guy_neutral_fill.png"),
		LoadTexture("resources/guy_prethrow_fill.png"),
		LoadTexture("resources/guy_throw_fill.png")
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
	Texture2D texLightning = LoadTexture("resources/lightning.png");
	Texture2D texCursor = LoadTexture("resources/cursor.png");
	Texture2D texBG = LoadTexture("resources/bg.png");

	WobblyTexture dogOutline, dogBack;
	WobblyTexture guyOutline, guyBack;

	Font font = LoadFontEx("resources/GamjaFlower-Regular.ttf", 80, 0, 0);
	SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
	const char* fetchText = "Fetch!";
	const char* loseText = "Bad dog :(";
	const char* loseDetailsText = "'R' to restart from beginning\n'C' to restart from checkpoint";
	const char* winText = "Good dog :)";
	const char* winDetailsText = "'F5' to use level editor\nCheck editor.txt for instructions";
	float fetchTextWidth = MeasureTextEx(font, fetchText, 80, 0).x;
	float loseTextWidth = MeasureTextEx(font, loseText, 80, 0).x;
	Vector2 loseDetailsSize = MeasureTextEx(font, loseDetailsText, 40, 0);
	float winTextWidth = MeasureTextEx(font, winText, 80, 0).x;
	Vector2 winDetailsSize = MeasureTextEx(font, winDetailsText, 40, 0);

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
	Camera2D prevCamera = game->camera;

	// Gameplay state
	Vector2 pos = game->dogStartingPos;
	float dogAngle = 0.f;
	bool dogFlipped = true;
	Vector2 dogUp = { 0.f, -1.f };
	Vector2 dogRight = { 1.f, 0.f };
	bool hasSunglasses = false;
	bool hasHat = false;
	bool hasBall = false;
	bool teleported = false;
	float ballTimer = 0.f;

	double hopTimer = -HOP_TIMER / 2.f;
	int frame = 0;
	float hopOffset = 0.f;
	float fallingSpeed = 0.f;
	DogRotationTarget currentRotTarget;
	Checkpoint lastCheckpoint(game->dogStartingPos, 0.f, false);

	// Cutscene state
	float cutsceneTimer = 0.f;
	int guyFrame = 0;
	bool drawFetchText = false;
	Vector2 ballPos = { -1.f, 0.f };
	bool playedThrowSound = false;
	bool playedWoofSound = false;
	float ballYSpeed = 0.f;

	float lightningFlashTime = 0.f;
	auto LightningFlash = [&]()
	{
		bool result = (int)(lightningFlashTime * 30) % 2 != 0;
		return result;
	};

	auto Lose = [&]()
	{
		fallingSpeed = -900.f;
		state = LOSE;
		PlaySound(sfxLose);
	};

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		if (dt > 0.03333333f) dt = 0.03333333f;

		if (lightningFlashTime > 0.f) lightningFlashTime -= dt;

		// Toggle between editor and play mode
		if (IsKeyPressed(KeyboardKey::KEY_F5))
		{
			if (state == EDITOR)
			{
				state = prevState;
				game->camera = prevCamera;
				if (state == GOING)
					PlayMusicStream(music);
			}
			else
			{
				prevCamera = game->camera;
				prevState = state;
				state = EDITOR;
				PauseMusicStream(music);
			}
		}

		// Reset
		if (IsKeyPressed(KeyboardKey::KEY_F6))
		{
			lastCheckpoint = Checkpoint(game->dogStartingPos, 0.f, false);
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
			hasSunglasses = false;
			hasHat = false;
			hasBall = false;
			StopMusicStream(music);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].enabled = 1.f;
			for (int i = 0; i < game->itemsCount; i++)
				game->items[i].enabled = true;
		}

		// Toggle fullscreen
		if (IsKeyPressed(KeyboardKey::KEY_F11))
			ToggleFullscreen();

		// Update entities
		if (state != EDITOR)
		{
			for (int i = 0; i < game->dangerBlocksCount; i++)
				game->dangerBlocks[i].Update(dt, WALL_WOBBLE_RATE, game->camera);
			for (int i = 0; i < game->floorsCount; i++)
				game->floors[i].Update(dt, WALL_WOBBLE_RATE);
			for (int i = 0; i < game->elevatorsCount; i++)
				game->elevators[i].Update(dt, WALL_WOBBLE_RATE, game->camera);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].Update(dt, WALL_WOBBLE_RATE, game->camera);
		}

		Rectangle dogHitBox = { pos.x - 48.f, pos.y - 48.f, 96.f, 96.f };

		// Update camera when you hit a camera zone
		if (state != EDITOR)
		{
			if (state != LOSE)
			{
				for (int i = 0; i < game->cameraZonesCount; i++)
				{
					CameraZone& cameraZone = game->cameraZones[i];
					if (cameraZone.ContainsPoint(pos))
						game->camera = cameraZone.params;
				}
			}

			// Play sound when you hit a key
			Button btnPressed = GetButtonFromKeyPressed();
			if (btnPressed != Button::Cancel && btnPressed != Button::None)
			{
				int rand = GetRandomValue(0, 6);
				SetSoundPitch(sfxButton[rand], 1.2f);
				PlaySound(sfxButton[rand]);
			}
			Button btnReleased = GetButtonFromKeyReleased();
			if (btnReleased != Button::Cancel && btnReleased != Button::None)
			{
				int rand = GetRandomValue(0, 6);
				SetSoundPitch(sfxButton[rand], 0.8f);
				PlaySound(sfxButton[rand]);
			}
		}

		switch (state)
		{
		case CUTSCENE:
		{
			cutsceneTimer += dt;
			if (cutsceneTimer < 1.f)
			{
				// Waiting
			}
			else if (cutsceneTimer < 2.f)
			{
				// Pre-throw
				guyFrame = 1;
			}
			else if (cutsceneTimer < 3.7f)
			{
				// Throw;
				if (ballPos.x < 0.f)
					ballPos.x = 0.f;
				ballPos.x += dt;
				guyFrame = 2;
				if (!playedThrowSound)
				{
					playedThrowSound = true;
					PlaySound(sfxThrow);
				}
				dogFlipped = false;
			}
			else if (cutsceneTimer < 5.7f)
			{
				// Fetch!
				ballPos.x = -1.f;
				drawFetchText = true;
				guyFrame = 0;
				if (!playedWoofSound)
				{
					playedWoofSound = true;
					PlaySound(sfxWoof);
				}
			}
			else
			{
				playedThrowSound = false;
				playedWoofSound = false;
				drawFetchText = false;
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
						PlaySound(sfxReverse);
					}
					else if (block.enabled == 0.f)
					{
						Lose();
					}
					break;
				}
			}
			
			// Check collision with items
			for (int i = 0; i < game->itemsCount; i++)
			{
				Item& item = game->items[i];
				if (item.enabled && CheckDogHit(item.pos, 32.f, 512.f))
				{
					item.enabled = false;
					PlaySound(sfxThunder);
					lightningFlashTime = 0.233333333333f;
					if (item.itemType == ITBall)
					{
						dogFlipped = !dogFlipped;
						PlaySound(sfxReverse);
					}

					switch (item.itemType)
					{
					case ITSunglasses: hasSunglasses = true; break;
					case ITHat:        hasHat = true; break;
					case ITBall:       hasBall = true; break;
					}
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

			// Check collision with checkpoints
			for (int i = 0; i < game->checkpointsCount; i++)
			{
				Checkpoint& checkpoint = game->checkpoints[i];
				if (CheckDogHit(checkpoint.pos, 192.f, 192.f))
				{
					if (checkpoint.musicStartTime == 0.f)
						checkpoint.musicStartTime = GetMusicTimePlayed(music);
					lastCheckpoint = checkpoint;
				}
			}

			// Winning
			if (hasBall)
			{
				ballTimer += dt;
				if (ballTimer > 2.78f && !teleported)
				{
					dogFlipped = true;
					pos = { 1400.f, 540.f };
					teleported = true;
				}

				if (teleported && pos.x <= 690.f)
				{
					ballYSpeed = 0.f;
					frame = 0;
					state = WIN;
					ballPos = Vector2Subtract(pos, { 120.f, 32.f });
					PlaySound(sfxWin);
					PlayMusicStream(hihatMusic);
				}
			}

			break;
		}
		case WIN:
		{
			hopOffset = 0.f;
			if (ballPos.y < 532.f)
			{
				ballYSpeed += 600.f * dt;
				ballPos.y += ballYSpeed * dt;
				ballPos.x -= 400.f * dt;
			}
			else
			{
				ballPos.y = 532.f;
			}

			UpdateMusicStream(hihatMusic);
			break;
		}
		case LOSE:
		{
			StopMusicStream(music); // TODO: Fade music out to avoid pop
			frame = 2;
			fallingSpeed += 3000.f * dt;
			pos = Vector2Add(pos, Vector2Scale(dogUp, -fallingSpeed * dt));

			if (IsKeyPressed(KEY_C))
			{
				state = GOING;
				pos = lastCheckpoint.pos;
				dogFlipped = lastCheckpoint.dogFlipped;
				SeekMusicStream(music, lastCheckpoint.musicStartTime);
				PlayMusicStream(music);
				dogAngle = 0.f;
				hopTimer = -HOP_TIMER / 2.f;
				frame = 0;
				hopOffset = 0.f;
				fallingSpeed = 0.f;
				currentRotTarget = DogRotationTarget();
				hasSunglasses = false;
				hasHat = false;
				hasBall = false;
				for (int i = 0; i < game->reversersCount; i++)
					game->reversers[i].enabled = 1.f;
				for (int i = 0; i < game->itemsCount; i++)
					game->items[i].enabled = true;
			}

			if (IsKeyPressed(KEY_R))
			{
				lastCheckpoint = Checkpoint(game->dogStartingPos, 0.f, false);
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
				hasSunglasses = false;
				hasHat = false;
				hasBall = false;
				StopMusicStream(music);
				for (int i = 0; i < game->reversersCount; i++)
					game->reversers[i].enabled = 1.f;
				for (int i = 0; i < game->itemsCount; i++)
					game->items[i].enabled = true;
			}
			break;
		}
		case EDITOR:
		{
			// Save
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

			// Mouse position
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
			if (IsKeyPressed(KeyboardKey::KEY_KP_0) || IsKeyPressed(KeyboardKey::KEY_ZERO))   editor.UpdatePlacing(ATNone);
			if (IsKeyPressed(KeyboardKey::KEY_KP_1) || IsKeyPressed(KeyboardKey::KEY_ONE))    editor.UpdatePlacing(ATFloor);
			if (IsKeyPressed(KeyboardKey::KEY_KP_2) || IsKeyPressed(KeyboardKey::KEY_TWO))    editor.UpdatePlacing(ATCurve);
			if (IsKeyPressed(KeyboardKey::KEY_KP_3) || IsKeyPressed(KeyboardKey::KEY_THREE))  editor.UpdatePlacing(ATElevator);
			if (IsKeyPressed(KeyboardKey::KEY_KP_4) || IsKeyPressed(KeyboardKey::KEY_FOUR))   editor.UpdatePlacing(ATDangerBlock);
			if (IsKeyPressed(KeyboardKey::KEY_KP_5) || IsKeyPressed(KeyboardKey::KEY_FIVE))   editor.UpdatePlacing(ATReverser);
			if (IsKeyPressed(KeyboardKey::KEY_KP_6) || IsKeyPressed(KeyboardKey::KEY_SIX))    editor.UpdatePlacing(ATCameraZone);
			if (IsKeyPressed(KeyboardKey::KEY_KP_7) || IsKeyPressed(KeyboardKey::KEY_SEVEN))  editor.UpdatePlacing(ATPrompt);
			if (IsKeyPressed(KeyboardKey::KEY_KP_8) || IsKeyPressed(KeyboardKey::KEY_EIGHT))  editor.UpdatePlacing(ATItem);
			if (IsKeyPressed(KeyboardKey::KEY_KP_9) || IsKeyPressed(KeyboardKey::KEY_NINE)) { editor.UpdatePlacing(ATCheckpoint); editor.v5 = 0.f; }

			if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT))
			{
				// If curve or reverser is selected, choose direction
				if (editor.placingAsset == ATCurve || (editor.placingAsset == ATReverser && editor.placingStep == 0) || editor.placingAsset == ATItem)
				{
					editor.v5++;
					if ((editor.placingAsset == ATItem && editor.v5 > 2.f) || editor.v5 > 3.f)
						editor.v5 = 0.f;
				}
				else
				{
					// Right click removal

					bool removedSomething = false;

					for (int i = 0; i < game->floorsCount; i++)
					{
						if (IsMouseOverLine(editor.placingPos, game->floors[i].start, game->floors[i].end))
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
						if (IsMouseOverRectangle(editor.placingPos, game->curves[i].pos, { 128.f, 128.f }))
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
						if (IsMouseOverLine(editor.placingPos, game->elevators[i].start, game->elevators[i].end))
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
						if (IsMouseOverRectangle(editor.placingPos, game->dangerBlocks[i].pos1, game->dangerBlocks[i].dimensions))
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
						if (IsMouseOverRectangle(editor.placingPos, game->reversers[i].pos1, dimensions))
						{
							game->reversers[i] = game->reversers[game->reversersCount - 1];
							game->reversersCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->promptsCount; i++)
					{
						if (IsMouseOverRectangle(editor.placingPos, game->prompts[i].pos, { 64.f, 64.f }))
						{
							game->prompts[i] = game->prompts[game->promptsCount - 1];
							game->promptsCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->itemsCount; i++)
					{
						if (IsMouseOverRectangle(editor.placingPos, game->items[i].pos, { 128.f, 128.f }))
						{
							game->items[i] = game->items[game->itemsCount - 1];
							game->itemsCount--;
							removedSomething = true;
							break;
						}
					}

					if (!removedSomething)
					for (int i = 0; i < game->checkpointsCount; i++)
					{
						if (IsMouseOverRectangle(editor.placingPos, game->checkpoints[i].pos, { 192.f, 192.f }))
						{
							game->checkpoints[i] = game->checkpoints[game->checkpointsCount - 1];
							game->checkpointsCount--;
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
				case ATPrompt:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Position
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1)
					{
						// Button
						Button button = GetButtonFromKeyPressed();
						if (button != Button::None && button != Button::Cancel)
						{
							printf("Button assigned!\n");
							game->AddPrompt(editor.v1, button);
							editor.placingStep = 0;
						}
					}
					break;
				}
				case ATItem:
				{
					if (clicked)
					{
						ItemType itype;
						if (editor.v5 == 0.f) itype = ITSunglasses;
						else if (editor.v5 == 1.f) itype = ITHat;
						else if (editor.v5 == 2.f) itype = ITBall;
						game->AddItem(editor.placingPos, itype);
						editor.placingStep = 0;
					}
					break;
				}
				case ATCheckpoint:
				{
					if (editor.placingStep == 0 && clicked)
					{
						// Position
						editor.v1 = editor.placingPos;
						editor.placingStep++;
					}
					else if (editor.placingStep == 1 && clicked)
					{
						// Dog flipped
						bool flipped = editor.placingPos.x < editor.v1.x;
						game->AddCheckpoint(editor.v1, 0.f, flipped);
						editor.placingStep = 0;
					}
				}
				}
			}

			break;
		}
		}

		dogBack.Update(dt, DOG_WOBBLE_RATE);
		dogOutline.Update(dt, DOG_WOBBLE_RATE);
		guyBack.Update(dt, WALL_WOBBLE_RATE);
		guyOutline.Update(dt, WALL_WOBBLE_RATE);
		
		BeginDrawing();
		{
			bool lightning = LightningFlash();
			if (lightning)
				ClearBackground({130, 90, 255, 255});
			else
			{
				ClearBackground(DARKPURPLE);
				DrawTexture(texBG, 0, 0, WHITE);
			}

			BeginMode2D(game->camera);

			for (int i = 0; i < game->promptsCount; i++)
				game->prompts[i].Draw(font, texCursor, lightning);
			for (int i = 0; i < game->dangerBlocksCount; i++)
				game->dangerBlocks[i].Draw(texLine, texPaintGray, font, texCursor, lightning);
			for (int i = 0; i < game->curvesCount; i++)
				game->curves[i].Draw(texCurveOutline, texCurveSolid, lightning);
			for (int i = 0; i < game->floorsCount; i++)
				game->floors[i].Draw(texLine, texPaintBlue, lightning);
			for (int i = 0; i < game->elevatorsCount; i++)
				game->elevators[i].Draw(texLine, texPaintLightGreen, lightning);
			for (int i = 0; i < game->reversersCount; i++)
				game->reversers[i].Draw(texReverserBackEnabled, texReverserBackDisabled, texReverserOutline, texReverserArrows, lightning);

			Vector2 offsetPos = Vector2Add(pos, Vector2Scale(dogUp, -hopOffset));
			Vector2 drawPos = Vector2Add(offsetPos, Vector2Scale(dogRight, dogFlipped ? -12.f : 12.f));
			dogBack.Draw(texDogBack[frame], drawPos, { 0.75f, 0.75f }, dogAngle, dogFlipped, false);
			dogOutline.Draw(state == LOSE ? texDogLose : texDogOutline[frame], drawPos, { 0.75f, 0.75f }, dogAngle, dogFlipped, true);
			//DrawRectangleLines((int)pos.x - 8.f, (int)pos.y - 8.f, 16, 16, RED);
			
			Vector2 guyDrawPos = { 332.f, 488.f };
			guyBack.Draw(texGuyBack[guyFrame], guyDrawPos, { 1.f, 1.f }, 0.f, false, false);
			guyOutline.Draw(texGuyOutline[guyFrame], guyDrawPos, { 1.f, 1.f }, 0.f, false, true);

			if (ballPos.x >= 0.f && state == CUTSCENE)
				DrawTexture(texItems[2], 450 + (int)(ballPos.x * 900), 340, WHITE);
			else if (state == WIN)
				DrawTexture(texItems[2], (int)ballPos.x, (int)ballPos.y, WHITE);

			// Items
			if (hasSunglasses)
			{
				Vector2 eyesPos = Vector2Add(Vector2Scale(dogRight, dogFlipped ? 26.f : 48.f), Vector2Scale(dogUp, 28.f - hopOffset + (frame == 0 ? 32 : (frame == 2 ? 29.f : 0.f))));
				dogOutline.Draw(texItems[0], Vector2Add(pos, eyesPos), { 0.75f, 0.75f }, dogAngle, dogFlipped, true);
			}
			if (hasHat)
			{
				Vector2 headPos = Vector2Add(Vector2Scale(dogRight, dogFlipped ? 24.f : 46.f), Vector2Scale(dogUp, 90.f - hopOffset + (frame == 0 ? 32 : (frame == 2 ? 29.f : 0.f))));
				dogOutline.Draw(texItems[1], Vector2Add(pos, headPos), { 0.75f, 0.75f }, dogAngle, dogFlipped, true);
			}

			for (int i = 0; i < game->itemsCount; i++)
				game->items[i].Draw(texItems);

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

				for (int i = 0; i < game->checkpointsCount; i++)
				{
					Checkpoint& checkpoint = game->checkpoints[i];
					Vector2 pos = { checkpoint.pos.x - 96.f, checkpoint.pos.y - 96.f };
					Rectangle cpRect = { pos.x, pos.y, 192.f, 192.f };
					DrawRectangleLinesEx(cpRect, 2.f / game->camera.zoom, GREEN);
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
						temp.Draw(texLine, texPaintBlue, false);
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
					temp.Draw(texCurveOutline, texCurveSolid, false);
					
					break;
				}
				case ATElevator:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 12, (int)editor.placingPos.y - 12, 24, 24, GREEN);
					else
					{
						Elevator temp(editor.v1, editor.placingStep == 1 ? editor.placingPos : editor.v2, {}, {}, 0.5f, Button::None);
						temp.Draw(texLine, texPaintLightGreen, false);

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
							DrawButtonText(font, texCursor, Vector2Scale(Vector2Add(editor.v1, editor.v2), 0.5f), (int)Button::QMark, false);
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
						DangerBlock temp(pos, {}, size, editor.placingStep == 3 ? Button::QMark : Button::None);
						temp.Draw(texLine, texPaintGray, font, texCursor, false);

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
					temp.Draw(texReverserBackEnabled, texReverserBackDisabled, texReverserOutline, texReverserArrows, false);
					if (editor.placingStep == 1)
						DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.placingPos.x, (int)editor.placingPos.y, GREEN);
					if (editor.placingStep == 2)
					{
						DrawLine((int)editor.v1.x, (int)editor.v1.y, (int)editor.v2.x, (int)editor.v2.y, GREEN);
						DrawButtonText(font, texCursor, editor.v1, (int)Button::QMark, false);
					}
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
				case ATPrompt:
				{
					if (editor.placingStep == 0)
						DrawRectangle((int)editor.placingPos.x - 32, (int)editor.placingPos.y - 32, 64, 64, WHITE);
					else
						DrawButtonText(font, texCursor, editor.v1, (int)Button::QMark, false);
					break;
				}
				case ATItem:
				{
					ItemType itype;
					if (editor.v5 == 0.f) itype = ITSunglasses;
					else if (editor.v5 == 1.f) itype = ITHat;
					else if (editor.v5 == 2.f) itype = ITBall;
					Item temp(editor.placingPos, itype);
					temp.Draw(texItems);
					break;
				}
				case ATCheckpoint:
				{
					Vector2 cpPos = editor.placingStep == 0 ? editor.placingPos : editor.v1;
					Vector2 cpOff = { cpPos.x - 96.f, cpPos.y - 96.f };
					Rectangle cpRect = { cpOff.x, cpOff.y, 192.f, 192.f };
					DrawRectangleLinesEx(cpRect, 2.f / game->camera.zoom, GREEN);
					if (editor.placingStep == 1)
						DrawLineEx(editor.v1, { editor.v1.x + (editor.placingPos.x > editor.v1.x ? 256.f : -256.f), editor.v1.y }, 2.f / game->camera.zoom, GREEN);
					break;
				}
				}
			}

			EndMode2D();

			if (lightningFlashTime > 0.2f)
				DrawTexture(texLightning, 200, 0, WHITE);

			if (drawFetchText)
			{
				DrawTextEx(font, fetchText, { 640.f - fetchTextWidth / 2, 320.f }, 80, 0, BLACK);
				DrawTextEx(font, fetchText, { 643.f - fetchTextWidth / 2, 323.f }, 80, 0, WHITE);
			}

			if (state == LOSE)
			{
				DrawTextEx(font, loseText, { 640.f - loseTextWidth / 2, 320.f }, 80, 0, BLACK);
				DrawTextEx(font, loseText, { 643.f - loseTextWidth / 2, 323.f }, 80, 0, WHITE);

				DrawTextEx(font, loseDetailsText, Vector2Subtract({ 640.f, 470.f }, Vector2Scale(loseDetailsSize, 0.5f)), 40, 0, BLACK);
				DrawTextEx(font, loseDetailsText, Vector2Subtract({ 642.f, 472.f }, Vector2Scale(loseDetailsSize, 0.5f)), 40, 0, WHITE);
			}
			else if (state == WIN)
			{
				DrawTextEx(font, winText, { 640.f - winTextWidth / 2, 320.f }, 80, 0, BLACK);
				DrawTextEx(font, winText, { 643.f - winTextWidth / 2, 323.f }, 80, 0, WHITE);

				DrawTextEx(font, winDetailsText, Vector2Subtract({ 640.f, 470.f }, Vector2Scale(winDetailsSize, 0.5f)), 40, 0, BLACK);
				DrawTextEx(font, winDetailsText, Vector2Subtract({ 642.f, 472.f }, Vector2Scale(winDetailsSize, 0.5f)), 40, 0, WHITE);
			}

			//DrawFPS(10, 10);
		}
		EndDrawing();
	}

	for (int i = 0; i < 3; i++)
	{
		UnloadTexture(texDogBack[i]);
		UnloadTexture(texDogOutline[i]);
		UnloadTexture(texItems[i]);
	}
	UnloadTexture(texDogLose);
	UnloadTexture(texLine);
	UnloadTexture(texPaintBlue);
	UnloadTexture(texPaintGray);
	UnloadTexture(texPaintLightGreen);
	UnloadTexture(texPaintLightBlue);
	UnloadTexture(texReverserBackEnabled);
	UnloadTexture(texReverserBackDisabled);
	UnloadTexture(texReverserOutline);
	UnloadTexture(texReverserArrows);
	UnloadTexture(texCurveSolid);
	UnloadTexture(texCurveOutline);
	UnloadTexture(texCursor);
	UnloadTexture(texBG);

	for (int i = 0; i < 7; i++)
		UnloadSound(sfxButton[i]);
	UnloadSound(sfxThrow);
	UnloadSound(sfxWoof);
	UnloadSound(sfxReverse);
	UnloadSound(sfxLose);
	UnloadSound(sfxWin);
	UnloadSound(sfxThunder);

	UnloadMusicStream(music);
	
	UnloadFont(font);
	UnloadImage(icon);
	
	CloseWindow();

	delete game;
	return 0;
}
