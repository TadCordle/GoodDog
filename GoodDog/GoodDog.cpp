#include <iostream>
#include "raylib.h"
#include "WobblyTexture.h"

#define DOG_WOBBLE_RATE 0.2f
#define WALL_WOBBLE_RATE 0.75f
#define HOP_TIMER 0.833333333333

enum Button { N, E, S, W };
enum CurveType { NE, SE, SW, NW };

struct Floor
{
	float startX;
	float endX;
	float Y;
};

struct DangerBlock
{
	Rectangle dimensions;
	Vector2 startPos;
	Vector2 endPos;
	Button button;
};

struct Elevator
{
	float startX;
	float endX;
	float startY;
	float endY;
	float lerpTime;
};

struct Reverser
{
	Rectangle dimensions;
	Vector2 pos;
	float enabled;
};

struct Curve
{
	Vector2 pos;
	CurveType type;
};

struct Game
{
	Floor floors[256];
	Curve curves[256];
	DangerBlock dangerBlocks[256];
	Elevator elevators[256];
	Reverser reversers[256];
};

enum GameState
{
	CUTSCENE,
	GOING,
	WIN,
	LOSE
};

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

	GameState state = CUTSCENE;

	WobblyLine testLine(texLine, { 0.f, 540.f }, { 1280.f, 540.f });
	WobblyLine testLine2(texLine, { 0.f, 565.f }, { 1280.f, 565.f });

	// Cutscene state
	float cutsceneTimer = 0.f;

	// Gameplay state
	double hopTimer = -HOP_TIMER / 2.f;
	int frame = 0;
	float hopOffset = 0.f;

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

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

			pos.x += (dogFlipped ? -1.f : 1.f) * 240.f * dt;

			hopOffset += (frame == 2 ? -120.f : 120.f) * dt;
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
		testLine.Update(dt, WALL_WOBBLE_RATE);
		testLine2.Update(dt, WALL_WOBBLE_RATE);

		BeginDrawing();

		ClearBackground(DARKPURPLE);
		Vector2 drawPos = { pos.x, pos.y + hopOffset };
		texDogBack[frame].Draw(drawPos, {dogSpriteScale, dogSpriteScale}, dogSpriteAngle, dogFlipped);
		texDogOutline[frame].Draw(drawPos, {dogSpriteScale, dogSpriteScale}, dogSpriteAngle, dogFlipped);
		
		testLine.Draw();
		testLine2.Draw();

		EndDrawing();
	}

	for (int i = 0; i < 3; i++)
	{
		texDogOutline[i].Unload();
		texDogBack[i].Unload();
	}
	UnloadTexture(texLine);
	CloseWindow();
	return 0;
}
