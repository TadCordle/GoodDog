#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	if (floorsCount < MAX_FLOORS)
		floors[floorsCount++] = Floor(start, end);
	else
		printf("Floor limit hit!\n");
}

void Game::AddCurve(Vector2 pos, CurveType type)
{
	if (curvesCount < MAX_CURVES)
		curves[curvesCount++] = Curve(pos, type);
	else
		printf("Curve limit hit!\n");
}

void Game::AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button)
{
	if (elevatorsCount < MAX_ELEVATORS)
		elevators[elevatorsCount++] = Elevator(start, end, newStart, newEnd, travelTime, button);
	else
		printf("Elevator limit hit!\n");
}

void Game::AddDangerBlock(Vector2 pos1, Vector2 pos2, Vector2 size, Button button)
{
	if (dangerBlocksCount < MAX_DANGERBLOCKS)
		dangerBlocks[dangerBlocksCount++] = DangerBlock(pos1, pos2, size, button);
	else
		printf("Danger block limit hit!\n");
}

void Game::AddReverser(Vector2 pos1, Vector2 pos2, Direction dir, Button button)
{
	if (reversersCount < MAX_REVERSERS)
		reversers[reversersCount++] = Reverser(pos1, pos2, dir, button);
	else
		printf("Reverser limit hit!\n");
}

void Game::AddCameraZone(Vector2 pos, Vector2 size, Camera2D params)
{
	if (cameraZonesCount < MAX_CAMERAZONES)
		cameraZones[cameraZonesCount++] = CameraZone(pos, size, params);
	else
		printf("Camera zone limit hit!\n");
}

void Game::AddPrompt(Vector2 pos, Button button)
{
	if (promptsCount < MAX_PROMPTS)
		prompts[promptsCount++] = Prompt(pos, button);
	else
		printf("Prompt limit hit!\n");
}

void Game::AddItem(Vector2 pos, ItemType itemType)
{
	if (itemsCount < MAX_ITEMS)
		items[itemsCount++] = Item(pos, itemType);
	else
		printf("Item limit hit!\n");
}

void Game::AddCheckpoint(Vector2 pos, float musicStartTime, bool dogFlipped)
{
	if (checkpointsCount < MAX_CHECKPOINTS)
		checkpoints[checkpointsCount++] = Checkpoint(pos, musicStartTime, dogFlipped);
	else
		printf("Checkpoint limit hit!\n");
}

void Game::Serialize(const char* path)
{
	FILE* file;
	fopen_s(&file, path, "w");
	if (!file)
	{
		printf("Couldn't open level.txt!\n");
		return;
	}

	for (int i = 0; i < floorsCount; i++)
	{
		Floor& floor = floors[i];
		fprintf(file, "%d\n", (int)AssetType::ATFloor);
		fprintf(file, "%f %f %f %f\n", floor.start.x, floor.start.y, floor.end.x, floor.end.y);
	}

	for (int i = 0; i < curvesCount; i++)
	{
		Curve& curve = curves[i];
		fprintf(file, "%d\n", (int)AssetType::ATCurve);
		fprintf(file, "%f %f %d\n", curve.pos.x, curve.pos.y, (int)curve.type);
	}

	for (int i = 0; i < elevatorsCount; i++)
	{
		Elevator& elevator = elevators[i];
		fprintf(file, "%d\n", (int)AssetType::ATElevator);
		fprintf(file, "%f %f %f %f %f %f %f %f %f %d\n", elevator.start.x, elevator.start.y, elevator.end.x, elevator.end.y, elevator.newStart.x, elevator.newStart.y, elevator.newEnd.x, elevator.newEnd.y, elevator.travelTime, (int)elevator.button);
	}

	for (int i = 0; i < dangerBlocksCount; i++)
	{
		DangerBlock& block = dangerBlocks[i];
		fprintf(file, "%d\n", (int)AssetType::ATDangerBlock);
		fprintf(file, "%f %f %f %f %f %f %d\n", block.pos1.x, block.pos1.y, block.pos2.x, block.pos2.y, block.dimensions.x, block.dimensions.y, (int)block.button);
	}

	for (int i = 0; i < reversersCount; i++)
	{
		Reverser& reverser = reversers[i];
		fprintf(file, "%d\n", (int)AssetType::ATReverser);
		fprintf(file, "%f %f %f %f %d %d\n", reverser.pos1.x, reverser.pos1.y, reverser.pos2.x, reverser.pos2.y, (int)reverser.dir, (int)reverser.button);
	}

	for (int i = 0; i < cameraZonesCount; i++)
	{
		CameraZone& zone = cameraZones[i];
		fprintf(file, "%d\n", (int)AssetType::ATCameraZone);
		fprintf(file, "%f %f %f %f %f %f %f %f %f\n", zone.pos.x, zone.pos.y, zone.size.x, zone.size.y, zone.params.offset.x, zone.params.offset.y, zone.params.target.x, zone.params.target.y, zone.params.zoom);
	}

	for (int i = 0; i < promptsCount; i++)
	{
		Prompt& prompt = prompts[i];
		fprintf(file, "%d\n", (int)AssetType::ATPrompt);
		fprintf(file, "%f %f %d\n", prompt.pos.x, prompt.pos.y, (int)prompt.button);
	}

	for (int i = 0; i < itemsCount; i++)
	{
		Item& item = items[i];
		fprintf(file, "%d\n", (int)AssetType::ATItem);
		fprintf(file, "%f %f %d\n", item.pos.x, item.pos.y, (int)item.itemType);
	}

	for (int i = 0; i < checkpointsCount; i++)
	{
		Checkpoint& checkpoint = checkpoints[i];
		fprintf(file, "%d\n", (int)AssetType::ATCheckpoint);
		fprintf(file, "%f %f %f %d\n", checkpoint.pos.x, checkpoint.pos.y, checkpoint.musicStartTime, checkpoint.dogFlipped ? 1 : 0);
	}

	fflush(file);
	fclose(file);
	printf("Level saved!\n");
}

void Game::Deserialize(const char* path)
{
	FILE* file;
	fopen_s(&file, path, "r");
	if (!file)
	{
		printf("Couldn't find level.txt!\n");
		return;
	}

	while (!feof(file))
	{
		int type;
		int _ = fscanf_s(file, "%d\n", &type);
		switch ((AssetType)type)
		{
		case ATFloor:
		{
			Vector2 start, end;
			_ = fscanf_s(file, "%f %f %f %f\n", &start.x, &start.y, &end.x, &end.y);
			AddFloor(start, end);
			break;
		}
		case ATCurve:
		{
			Vector2 pos;
			int curveType;
			_ = fscanf_s(file, "%f %f %d\n", &pos.x, &pos.y, &curveType);
			AddCurve(pos, (CurveType)curveType);
			break;
		}
		case ATElevator:
		{
			Vector2 start, end, newStart, newEnd;
			float travelTime;
			int button;
			_ = fscanf_s(file, "%f %f %f %f %f %f %f %f %f %d\n", &start.x, &start.y, &end.x, &end.y, &newStart.x, &newStart.y, &newEnd.x, &newEnd.y, &travelTime, &button);
			AddElevator(start, end, newStart, newEnd, travelTime, (Button)button);
			break;
		}
		case ATDangerBlock:
		{
			Vector2 pos1, pos2, size;
			int button;
			_ = fscanf_s(file, "%f %f %f %f %f %f %d\n", &pos1.x, &pos1.y, &pos2.x, &pos2.y, &size.x, &size.y, &button);
			AddDangerBlock(pos1, pos2, size, (Button)button);
			break;
		}
		case ATReverser:
		{
			Vector2 pos1, pos2;
			int dir, button;
			_ = fscanf_s(file, "%f %f %f %f %d %d\n", &pos1.x, &pos1.y, &pos2.x, &pos2.y, &dir, &button);
			AddReverser(pos1, pos2, (Direction)dir, (Button)button);
			break;
		}
		case ATCameraZone:
		{
			Vector2 pos, size;
			Camera2D params = { 0 };
			_ = fscanf_s(file, "%f %f %f %f %f %f %f %f %f\n", &pos.x, &pos.y, &size.x, &size.y, &params.offset.x, &params.offset.y, &params.target.x, &params.target.y, &params.zoom);
			AddCameraZone(pos, size, params);
			break;
		}
		case ATPrompt:
		{
			Vector2 pos;
			int button;
			_ = fscanf_s(file, "%f %f %d\n", &pos.x, &pos.y, &button);
			AddPrompt(pos, (Button)button);
			break;
		}
		case ATItem:
		{
			Vector2 pos;
			int itemType;
			_ = fscanf_s(file, "%f %f %d\n", &pos.x, &pos.y, &itemType);
			AddItem(pos, (ItemType)itemType);
			break;
		}
		case ATCheckpoint:
		{
			Vector2 pos;
			float musicStartTime;
			int flipped;
			_ = fscanf_s(file, "%f %f %f %d\n", &pos.x, &pos.y, &musicStartTime, &flipped);
			AddCheckpoint(pos, musicStartTime, flipped == 1);
			break;
		}
		}
	}
	fclose(file);
}

Floor::Floor(Vector2 _start, Vector2 _end)
{
	start = _start;
	end = _end;
	line1 = WobblyLine();
	line2 = WobblyLine();
}

void Floor::Update(float dt, float wobbleRate)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);
}

void Floor::Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning)
{
	DrawPaintLine(paintTex, { start.x, start.y }, { end.x, end.y }, lightning);
	Vector2 offsetDir = Vector2Normalize({ start.y - end.y, end.x - start.x });
	Vector2 offsetPos = Vector2Scale(offsetDir, 12.f);
	Vector2 offsetNeg = Vector2Scale(offsetDir, -12.f);
	line1.Draw(lineTex, { start.x + offsetNeg.x, start.y + offsetNeg.y }, { end.x + offsetNeg.x, end.y + offsetNeg.y });
	line2.Draw(lineTex, { start.x + offsetPos.x, start.y + offsetPos.y }, { end.x + offsetPos.x, end.y + offsetPos.y });
}

Elevator::Elevator(Vector2 _start, Vector2 _end, Vector2 _newStart, Vector2 _newEnd, float _travelTime, Button _button)
{
	start = _start;
	end = _end;
	newStart = _newStart;
	newEnd = _newEnd;
	travelTime = _travelTime;
	button = _button;
	line1 = WobblyLine();
	line2 = WobblyLine();
}

void Elevator::Update(float dt, float wobbleRate, Camera2D camera)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);
	if (button != Button::Mouse)
		currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	else
	{
		if (held)
		{
			if (IsMouseButtonUp(MouseButton::MOUSE_BUTTON_LEFT))
				held = false;
		}
		else
		{
			bool mousePressed = IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT);
			Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
			if (mousePressed && IsMouseOverLine(mousePos, GetCurrentStart(), GetCurrentEnd()))
				held = true;
		}
		currentTravelTime += dt * (held ? 1 : -1);
	}
	if (currentTravelTime < 0.f) currentTravelTime = 0.f;
	if (currentTravelTime > travelTime) currentTravelTime = travelTime;
}

void Elevator::Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning)
{
	Vector2 s = GetCurrentStart();
	Vector2 e = GetCurrentEnd();
	DrawPaintLine(paintTex, { s.x, s.y }, { e.x, e.y }, lightning);
	Vector2 offsetDir = Vector2Normalize({ s.y - e.y, e.x - s.x });
	Vector2 offsetPos = Vector2Scale(offsetDir, 12.f);
	Vector2 offsetNeg = Vector2Scale(offsetDir, -12.f);
	line1.Draw(lineTex, { s.x + offsetNeg.x, s.y + offsetNeg.y }, { e.x + offsetNeg.x, e.y + offsetNeg.y });
	line2.Draw(lineTex, { s.x + offsetPos.x, s.y + offsetPos.y }, { e.x + offsetPos.x, e.y + offsetPos.y });
}

DangerBlock::DangerBlock(Vector2 _pos1, Vector2 _pos2, Vector2 _dimensions, Button _button)
{
	pos1 = _pos1;
	pos2 = _pos2;
	dimensions = _dimensions;
	button = _button;
	wobblyRectangle = WobblyRectangle();
}

void DangerBlock::Update(float dt, float wobbleRate, Camera2D camera)
{
	wobblyRectangle.Update(dt, wobbleRate);
	if (button != Button::Mouse)
		currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	else
	{
		if (held)
		{
			if (IsMouseButtonUp(MouseButton::MOUSE_BUTTON_LEFT))
				held = false;
		}
		else
		{
			bool mousePressed = IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT);
			Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
			if (mousePressed && IsMouseOverRectangle(mousePos, GetCurrentPos(), dimensions))
				held = true;
		}
		currentTravelTime += dt * (held ? 1 : -1);
	}
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.2f) currentTravelTime = 0.2f;
}

void DangerBlock::Draw(Texture2D& lineTex, Texture2D& paintTex, Font& font, Texture& cursorTex, bool lightning)
{
	Vector2 p = GetCurrentPos();
	Vector2 topLeft  = { p.x - dimensions.x / 2.f, p.y - dimensions.y / 2.f };
	Vector2 botRight = { p.x + dimensions.x / 2.f, p.y + dimensions.y / 2.f };
	wobblyRectangle.Draw(lineTex, paintTex, topLeft, botRight, lightning);
	if (button != Button::None && button != Button::Cancel)
		DrawButtonText(font, cursorTex, p, (int)button, lightning);
}

Reverser::Reverser(Vector2 _pos1, Vector2 _pos2, Direction _dir, Button _button)
{
	pos1 = _pos1;
	pos2 = _pos2;
	dir = _dir;
	button = _button;
	texFront = WobblyTexture();
	texBack = WobblyTexture();
}

void Reverser::Update(float dt, float wobbleRate, Camera2D camera)
{
	texFront.Update(dt, wobbleRate);
	texBack.Update(dt, wobbleRate);

	if (enabled < 1.f)
	{
		enabled -= dt * 3.f;
		if (enabled < 0.f)
			enabled = 0.f;
	}

	if (button != Button::Mouse)
		currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	else
	{
		if (held)
		{
			if (IsMouseButtonUp(MouseButton::MOUSE_BUTTON_LEFT))
				held = false;
		}
		else
		{
			bool mousePressed = IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT);
			Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
			Vector2 dimensions = (dir == Left || dir == Right) ? Vector2{ 60.f, 220.f } : Vector2{ 220.f, 60.f };
			if (mousePressed && IsMouseOverRectangle(mousePos, GetCurrentPos(), dimensions))
				held = true;
		}
		currentTravelTime += dt * (held ? 1 : -1);
	}
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.3f) currentTravelTime = 0.3f;
}

void Reverser::Draw(Texture2D& texBackEnabled, Texture2D& texBackDisabled, Texture2D& texOutline, Texture2D& texArrows, bool lightning)
{
	Vector2 p = GetCurrentPos();
	Vector2 scale = { 0.75f, 0.75f };
	float angle = dir == Up || dir == Down ? 90.f : 0.f;
	bool flipped = dir == Left || dir == Up;
	if (enabled < 1.f)
	{
		texBack.Draw(texBackDisabled, p, scale, angle, flipped, false, 1.f, lightning);
	}
	if (enabled > 0.f)
	{
		texBack.Draw(texBackEnabled, p, scale, angle, flipped, false, enabled, lightning);
		texFront.Draw(texArrows, p, scale, angle, flipped, false, enabled);
	}
	texFront.Draw(texOutline, p, scale, angle, flipped, true, 1.f);
}

Curve::Curve(Vector2 _pos, CurveType _type)
{
	pos = _pos;
	type = _type;
}

void Curve::Draw(Texture2D& lineTex, Texture2D& paintTex, bool lightning)
{
	Rectangle srcRect = {};
	switch (type)
	{
		case NE: srcRect = { 128.f,   0.f, 128.f, 128.f }; break;
		case SE: srcRect = { 128.f, 128.f, 128.f, 128.f }; break;
		case SW: srcRect = {   0.f, 128.f, 128.f, 128.f }; break;
		case NW: srcRect = {   0.f,   0.f, 128.f, 128.f }; break;
	}

	Rectangle dstRect = { pos.x, pos.y, 128.f, 128.f };
	Vector2 origin = { 64.f, 64.f };
	DrawTexturePro(paintTex, srcRect, dstRect, origin, 0.f, lightning ? BLACK : WHITE);
	DrawTexturePro(lineTex, srcRect, dstRect, origin, 0.f, lightning ? BLACK : WHITE);
}

bool Curve::HitCurve(Vector2 dogPos, Vector2 offset)
{
	const float amt = 48.f;
	Vector2 p = { pos.x + offset.x, pos.y + offset.y };
	return dogPos.x - (p.x - amt) > 0.f && dogPos.x - (p.x + amt) < 0.f && dogPos.y - (p.y - amt) > 0.f && dogPos.y - (p.y + amt) < 0.f;
}

DogRotationTarget Curve::GetRotationTarget(Vector2 point, Vector2 up, Vector2 right)
{
	if (type == SE)
	{
		if (HitCurve(point, { -16.f, 128.f }) && right.x ==  1.f && up.y ==  1.f) return {  90.f,  -90.f };
		if (HitCurve(point, { -96.f, -64.f }) && right.x ==  1.f && up.y == -1.f) return { -90.f, -240.f };
		if (HitCurve(point, { -64.f, -96.f }) && right.y ==  1.f && up.x == -1.f) return { 360.f,  240.f };
		if (HitCurve(point, { 128.f, -16.f }) && right.y ==  1.f && up.x ==  1.f) return { 180.f,   80.f };
	} 
	else if (type == SW)
	{
		if (HitCurve(point, { -16.f, 128.f }) && right.x == -1.f && up.y ==  1.f) return { 270.f,   90.f };
		if (HitCurve(point, {  64.f, -64.f }) && right.x == -1.f && up.y == -1.f) return {  90.f,  240.f };
		if (HitCurve(point, {  32.f, -96.f }) && right.y ==  1.f && up.x ==  1.f) return {   0.f, -240.f };
		if (HitCurve(point, {-160.f, -16.f }) && right.y ==  1.f && up.x == -1.f) return { 180.f,  -90.f };
	}
	else if (type == NE)
	{
		if (HitCurve(point, { -16.f,-160.f }) && right.x ==  1.f && up.y == -1.f) return {  90.f,   90.f };
		if (HitCurve(point, { -96.f,  32.f }) && right.x ==  1.f && up.y ==  1.f) return { 270.f,  240.f };
		if (HitCurve(point, { -64.f,  64.f }) && right.y == -1.f && up.x == -1.f) return { 180.f, -240.f };
		if (HitCurve(point, { 128.f, -16.f }) && right.y == -1.f && up.x ==  1.f) return {   0.f,  -90.f };
	}
	else
	{
		if (HitCurve(point, { -16.f,-160.f }) && right.x == -1.f && up.y == -1.f) return { -90.f,  -90.f };
		if (HitCurve(point, {  64.f,  32.f }) && right.x == -1.f && up.y ==  1.f) return {  90.f, -240.f };
		if (HitCurve(point, {  32.f,  64.f }) && right.y == -1.f && up.x ==  1.f) return { 180.f,  240.f };
		if (HitCurve(point, {-160.f, -16.f }) && right.y == -1.f && up.x == -1.f) return { 360.f,   90.f };
	}
	return DogRotationTarget();
}

CameraZone::CameraZone(Vector2 _pos, Vector2 _size, Camera2D _params)
{
	pos = _pos;
	size = _size;
	params = _params;
}

bool CameraZone::ContainsPoint(Vector2 point)
{
	return point.x < pos.x + size.x / 2.f && 
		   point.x > pos.x - size.x / 2.f && 
		   point.y < pos.y + size.y / 2.f && 
		   point.y > pos.y - size.y / 2.f;
}

Prompt::Prompt(Vector2 _pos, Button _button)
{
	pos = _pos;
	button = _button;
}

void Prompt::Draw(Font& font, Texture& cursorTex, bool lightning)
{
	DrawButtonText(font, cursorTex, pos, (int)button, lightning);
}

Item::Item(Vector2 _pos, ItemType _itemType)
{
	pos = _pos;
	itemType = _itemType;
}

void Item::Draw(Texture2D paintTexs[3])
{
	if (enabled)
		DrawTextureEx(paintTexs[(int)itemType], Vector2Subtract(pos, {48.f, 48.f}), 0.f, 0.75f, WHITE);
}

Checkpoint::Checkpoint(Vector2 _pos, float _musicStartTime, bool _dogFlipped)
{
	pos = _pos;
	musicStartTime = _musicStartTime;
	dogFlipped = _dogFlipped;
}

Button GetButtonFromKeyPressed()
{
	if (IsKeyPressed(KeyboardKey::KEY_BACKSPACE)) return Button::Cancel;
	if (IsKeyPressed(KeyboardKey::KEY_A)) return Button::A;
	if (IsKeyPressed(KeyboardKey::KEY_B)) return Button::B;
	if (IsKeyPressed(KeyboardKey::KEY_C)) return Button::C;
	if (IsKeyPressed(KeyboardKey::KEY_D)) return Button::D;
	if (IsKeyPressed(KeyboardKey::KEY_E)) return Button::E;
	if (IsKeyPressed(KeyboardKey::KEY_F)) return Button::F;
	if (IsKeyPressed(KeyboardKey::KEY_G)) return Button::G;
	if (IsKeyPressed(KeyboardKey::KEY_H)) return Button::H;
	if (IsKeyPressed(KeyboardKey::KEY_I)) return Button::I;
	if (IsKeyPressed(KeyboardKey::KEY_J)) return Button::J;
	if (IsKeyPressed(KeyboardKey::KEY_K)) return Button::K;
	if (IsKeyPressed(KeyboardKey::KEY_L)) return Button::L;
	if (IsKeyPressed(KeyboardKey::KEY_M)) return Button::M;
	if (IsKeyPressed(KeyboardKey::KEY_N)) return Button::N;
	if (IsKeyPressed(KeyboardKey::KEY_O)) return Button::O;
	if (IsKeyPressed(KeyboardKey::KEY_P)) return Button::P;
	if (IsKeyPressed(KeyboardKey::KEY_Q)) return Button::Q;
	if (IsKeyPressed(KeyboardKey::KEY_R)) return Button::R;
	if (IsKeyPressed(KeyboardKey::KEY_S)) return Button::S;
	if (IsKeyPressed(KeyboardKey::KEY_T)) return Button::T;
	if (IsKeyPressed(KeyboardKey::KEY_U)) return Button::U;
	if (IsKeyPressed(KeyboardKey::KEY_V)) return Button::V;
	if (IsKeyPressed(KeyboardKey::KEY_W)) return Button::W;
	if (IsKeyPressed(KeyboardKey::KEY_X)) return Button::X;
	if (IsKeyPressed(KeyboardKey::KEY_Y)) return Button::Y;
	if (IsKeyPressed(KeyboardKey::KEY_Z)) return Button::Z;
	if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) return Button::Mouse;
	return Button::None;
}

Button GetButtonFromKeyReleased()
{
	if (IsKeyReleased(KeyboardKey::KEY_BACKSPACE)) return Button::Cancel;
	if (IsKeyReleased(KeyboardKey::KEY_A)) return Button::A;
	if (IsKeyReleased(KeyboardKey::KEY_B)) return Button::B;
	if (IsKeyReleased(KeyboardKey::KEY_C)) return Button::C;
	if (IsKeyReleased(KeyboardKey::KEY_D)) return Button::D;
	if (IsKeyReleased(KeyboardKey::KEY_E)) return Button::E;
	if (IsKeyReleased(KeyboardKey::KEY_F)) return Button::F;
	if (IsKeyReleased(KeyboardKey::KEY_G)) return Button::G;
	if (IsKeyReleased(KeyboardKey::KEY_H)) return Button::H;
	if (IsKeyReleased(KeyboardKey::KEY_I)) return Button::I;
	if (IsKeyReleased(KeyboardKey::KEY_J)) return Button::J;
	if (IsKeyReleased(KeyboardKey::KEY_K)) return Button::K;
	if (IsKeyReleased(KeyboardKey::KEY_L)) return Button::L;
	if (IsKeyReleased(KeyboardKey::KEY_M)) return Button::M;
	if (IsKeyReleased(KeyboardKey::KEY_N)) return Button::N;
	if (IsKeyReleased(KeyboardKey::KEY_O)) return Button::O;
	if (IsKeyReleased(KeyboardKey::KEY_P)) return Button::P;
	if (IsKeyReleased(KeyboardKey::KEY_Q)) return Button::Q;
	if (IsKeyReleased(KeyboardKey::KEY_R)) return Button::R;
	if (IsKeyReleased(KeyboardKey::KEY_S)) return Button::S;
	if (IsKeyReleased(KeyboardKey::KEY_T)) return Button::T;
	if (IsKeyReleased(KeyboardKey::KEY_U)) return Button::U;
	if (IsKeyReleased(KeyboardKey::KEY_V)) return Button::V;
	if (IsKeyReleased(KeyboardKey::KEY_W)) return Button::W;
	if (IsKeyReleased(KeyboardKey::KEY_X)) return Button::X;
	if (IsKeyReleased(KeyboardKey::KEY_Y)) return Button::Y;
	if (IsKeyReleased(KeyboardKey::KEY_Z)) return Button::Z;
	if (IsMouseButtonReleased(MouseButton::MOUSE_BUTTON_LEFT)) return Button::Mouse;
	return Button::None;
}

bool IsMouseOverRectangle(Vector2 cursor, Vector2 pos, Vector2 size)
{
	return cursor.x < pos.x + size.x / 2.f &&
		   cursor.x > pos.x - size.x / 2.f &&
		   cursor.y < pos.y + size.y / 2.f &&
		   cursor.y > pos.y - size.y / 2.f;
}

bool IsMouseOverLine(Vector2 cursor, Vector2 start, Vector2 end)
{
	Vector2 floorCenter = Vector2Lerp(start, end, 0.5f);
	Vector2 floorRight = Vector2Normalize(Vector2Subtract(end, start));
	Vector2 floorUp = { floorRight.y, -floorRight.x };
	float mouseXToFloor = Vector2DotProduct(Vector2Subtract(cursor, floorCenter), floorRight);
	float floorLength = Vector2DotProduct(Vector2Subtract(end, start), floorRight);
	if (fabs(mouseXToFloor) < fabs(floorLength) / 2.f + 64.f)
	{
		Vector2 closestFloorPos = Vector2Lerp(start, end, mouseXToFloor / floorLength + 0.5f);
		float dist = Vector2DotProduct(Vector2Subtract(cursor, closestFloorPos), floorUp);
		return fabs(dist) < 24.f;
	}
	return false;
}