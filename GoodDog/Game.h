#pragma once

#include "raylib.h"
#include "WobblyRender.h"

#define DOG_WOBBLE_RATE 0.2f
#define WALL_WOBBLE_RATE 0.75f
#define HOP_TIMER 0.833333333333

#define MAX_FLOORS 1024
#define MAX_CURVES 1024
#define MAX_ELEVATORS 1024
#define MAX_DANGERBLOCKS 1024
#define MAX_REVERSERS 1024
#define MAX_CAMERAZONES 1024
#define MAX_PROMPTS 1024
#define MAX_ITEMS 1024
#define MAX_CHECKPOINTS 1024

enum Button {
	None = 0,
	Cancel = 1,
	QMark = 2,
	Mouse = 3,
	A = KEY_A,
	B = KEY_B,
	C = KEY_C,
	D = KEY_D,
	E = KEY_E,
	F = KEY_F,
	G = KEY_G,
	H = KEY_H,
	I = KEY_I,
	J = KEY_J,
	K = KEY_K,
	L = KEY_L,
	M = KEY_M,
	N = KEY_N,
	O = KEY_O,
	P = KEY_P,
	Q = KEY_Q,
	R = KEY_R,
	S = KEY_S,
	T = KEY_T,
	U = KEY_U,
	V = KEY_V,
	W = KEY_W,
	X = KEY_X,
	Y = KEY_Y,
	Z = KEY_Z
};

Button GetButtonFromKeyPressed();
Button GetButtonFromKeyReleased();

enum CurveType { 
	NE, 
	SE, 
	SW, 
	NW 
};

struct DogRotationTarget
{
	float targetAngle = 0.f;
	float angularSpeed = 0.f;
};

struct Floor
{
	Vector2 start = {}, end = {};
	WobblyLine line1, line2;

	Floor() {}
	Floor(Vector2 _start, Vector2 _end);
	void Update(float dt, float wobbleRate);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning);
};

struct Elevator
{
	Vector2 start = {};
	Vector2 end = {};
	Vector2 newStart = {};
	Vector2 newEnd = {};
	WobblyLine line1, line2;
	float travelTime = 0.f;
	float currentTravelTime = 0.f;
	Button button;
	bool held = false;

	Elevator() {}
	Elevator(Vector2 _start, Vector2 _end, Vector2 _newStart, Vector2 _newEnd, float _travelTime, Button _button);
	void Update(float dt, float wobbleRate, Camera2D camera);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning);

	inline Vector2 GetCurrentStart() { float t = currentTravelTime / travelTime; return Vector2Lerp(start, newStart, t); }
	inline Vector2 GetCurrentEnd()   { float t = currentTravelTime / travelTime; return Vector2Lerp(  end,   newEnd, t); }
};

struct DangerBlock
{
	WobblyRectangle wobblyRectangle;
	Vector2 pos1 = {};
	Vector2 pos2 = {};
	Vector2 dimensions = {};
	float currentTravelTime = 0.f;
	Button button;
	bool held = false;

	DangerBlock() {}
	DangerBlock(Vector2 _pos1, Vector2 _pos2, Vector2 _dimensions, Button _button);
	void Update(float dt, float wobbleRate, Camera2D camera);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, Font& font, Texture2D& cursorTex, bool lightning);

	inline Vector2 GetCurrentPos() { float t = currentTravelTime / 0.2f; return Vector2Lerp(pos1, pos2, t); }
};

enum Direction
{
	Left,
	Right,
	Up,
	Down
};

struct Reverser
{
	Vector2 pos1 = {};
	Vector2 pos2 = {};
	Direction dir;
	Button button;
	bool held = false;
	float enabled = 1.f;
	float currentTravelTime = 0.f;
	WobblyTexture texFront, texBack;

	Reverser() {}
	Reverser(Vector2 _pos1, Vector2 _pos2, Direction _dir, Button _button);
	void Update(float dt, float wobbleRate, Camera2D camera);
	void Draw(Texture2D& texBackEnabled, Texture2D& texBackDisabled, Texture2D& texOutline, Texture2D& texArrows, bool lightning);

	inline Vector2 GetCurrentPos() { float t = currentTravelTime / 0.3f; return Vector2Lerp(pos1, pos2, t); }
};

struct Curve
{
	Vector2 pos = {};
	CurveType type;
	WobblyTexture texOutline;

	Curve() {}
	Curve(Vector2 _pos, CurveType _type);
	void Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning);
	bool HitCurve(Vector2 dogPos, Vector2 offset);
	DogRotationTarget GetRotationTarget(Vector2 point, Vector2 up, Vector2 right);
};

struct CameraZone
{
	Vector2 pos = {};
	Vector2 size = {};
	Camera2D params;

	CameraZone() {}
	CameraZone(Vector2 _pos, Vector2 _size, Camera2D _params);
	bool ContainsPoint(Vector2 _point);
};

struct Prompt
{
	Vector2 pos = {};
	Button button;

	Prompt() {}
	Prompt(Vector2 _pos, Button _button);
	void Draw(Font& font, Texture2D& cursorTex, bool lightning);
};

enum ItemType
{
	ITSunglasses,
	ITHat,
	ITBall
};

struct Item
{
	Vector2 pos = {};
	ItemType itemType;
	bool enabled = true;

	Item() {}
	Item(Vector2 _pos, ItemType _itemType);
	void Draw(Texture2D paintTexs[3]);
};

struct Checkpoint
{
	Vector2 pos = {};
	float musicStartTime = 0.f;
	bool dogFlipped = false;

	Checkpoint() {}
	Checkpoint(Vector2 _pos, float _musicStartTime, bool dogFlipped);
};

enum GameState
{
	CUTSCENE,
	GOING,
	WIN,
	LOSE,
	EDITOR
};

struct Game
{
	Floor floors[MAX_FLOORS] = {};
	Curve curves[MAX_CURVES] = {};
	DangerBlock dangerBlocks[MAX_DANGERBLOCKS] = {};
	Elevator elevators[MAX_ELEVATORS] = {};
	Reverser reversers[MAX_REVERSERS] = {};
	CameraZone cameraZones[MAX_CAMERAZONES] = {};
	Prompt prompts[MAX_PROMPTS] = {};
	Item items[MAX_ITEMS] = {};
	Checkpoint checkpoints[MAX_CHECKPOINTS] = {};

	int floorsCount = 0;
	int reversersCount = 0;
	int elevatorsCount = 0;
	int dangerBlocksCount = 0;
	int curvesCount = 0;
	int cameraZonesCount = 0;
	int promptsCount = 0;
	int itemsCount = 0;
	int checkpointsCount = 0;

	Camera2D camera = {};

	Vector2 dogStartingPos = {};

	Game() {}
	void AddFloor(Vector2 start, Vector2 end);
	void AddCurve(Vector2 pos, CurveType type);
	void AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button);
	void AddDangerBlock(Vector2 pos1, Vector2 pos2, Vector2 size, Button button);
	void AddReverser(Vector2 pos1, Vector2 pos2, Direction dir, Button button);
	void AddCameraZone(Vector2 pos, Vector2 size, Camera2D params);
	void AddPrompt(Vector2 pos, Button button);
	void AddItem(Vector2 pos, ItemType itemType);
	void AddCheckpoint(Vector2 pos, float musicStartTime, bool dogFlipped);

	void Serialize(const char* path);
	void Deserialize(const char* path);
};

enum AssetType
{
	ATNone,
	ATFloor,
	ATCurve,
	ATElevator,
	ATDangerBlock,
	ATReverser,
	ATCameraZone,
	ATPrompt,
	ATItem,
	ATCheckpoint
};

struct EditorState
{
	Vector2 placingPos = {};
	AssetType placingAsset = ATNone;
	int placingStep = 0;

	Vector2 v1 = {};
	Vector2 v2 = {};
	Vector2 v3 = {};
	Vector2 v4 = {};
	float v5 = 0.f;
	Button button = Button::None;
	bool showLimits = false;

	void UpdatePlacing(AssetType at) { placingAsset = at; placingStep = 0; };
};

bool IsMouseOverRectangle(Vector2 cursor, Vector2 pos, Vector2 size);
bool IsMouseOverLine(Vector2 cursor, Vector2 start, Vector2 end);