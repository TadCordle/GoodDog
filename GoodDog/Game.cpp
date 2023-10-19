#include "Game.h"

void Game::AddFloor(Vector2 start, Vector2 end)
{
	floors[floorsCount++] = Floor(start, end);
}

void Game::AddCurve(Vector2 pos, CurveType type)
{
	curves[curvesCount++] = Curve(pos, type);
}

void Game::AddElevator(Vector2 start, Vector2 end, Vector2 newStart, Vector2 newEnd, float travelTime, Button button)
{
	elevators[elevatorsCount++] = Elevator(start, end, newStart, newEnd, travelTime, button);
}

void Game::AddDangerBlock(Vector2 pos1, Vector2 pos2, Vector2 size, Button button)
{
	dangerBlocks[dangerBlocksCount++] = DangerBlock(pos1, pos2, size, button);
}

void Game::AddReverser(Vector2 pos1, Vector2 pos2, Direction dir, Button button)
{
	reversers[reversersCount++] = Reverser(pos1, pos2, dir, button);
}

void Game::AddCameraZone(Vector2 pos, Vector2 size, Camera2D params)
{
	cameraZones[cameraZonesCount++] = CameraZone(pos, size, params);
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

void Floor::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	DrawPaintLine(paintTex, { start.x, start.y }, { end.x, end.y });
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

void Elevator::Update(float dt, float wobbleRate)
{
	line1.Update(dt, wobbleRate);
	line2.Update(dt, wobbleRate);

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f) currentTravelTime = 0.f;
	if (currentTravelTime > travelTime) currentTravelTime = travelTime;
}

void Elevator::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Vector2 s = GetCurrentStart();
	Vector2 e = GetCurrentEnd();
	DrawPaintLine(paintTex, { s.x, s.y }, { e.x, e.y });
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

void DangerBlock::Update(float dt, float wobbleRate)
{
	wobblyRectangle.Update(dt, wobbleRate);

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.4f) currentTravelTime = 0.4f;
}

void DangerBlock::Draw(Texture2D& lineTex, Texture2D& paintTex)
{
	Vector2 p = GetCurrentPos();
	Vector2 topLeft  = { p.x - dimensions.x / 2.f, p.y - dimensions.y / 2.f };
	Vector2 botRight = { p.x + dimensions.x / 2.f, p.y + dimensions.y / 2.f };
	wobblyRectangle.Draw(lineTex, paintTex, topLeft, botRight);
	// TODO: Draw prompt
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

void Reverser::Update(float dt, float wobbleRate)
{
	texFront.Update(dt, wobbleRate);
	texBack.Update(dt, wobbleRate);

	if (enabled < 1.f)
	{
		enabled -= dt * 3.f;
		if (enabled < 0.f)
			enabled = 0.f;
	}

	currentTravelTime += dt * (IsKeyDown((int)button) ? 1 : -1);
	if (currentTravelTime < 0.f)  currentTravelTime = 0.f;
	if (currentTravelTime > 0.4f) currentTravelTime = 0.4f;
}

void Reverser::Draw(Texture2D& texBackEnabled, Texture2D& texBackDisabled, Texture2D& texOutline, Texture2D& texArrows)
{
	Vector2 p = GetCurrentPos();
	Vector2 scale = { 0.75f, 0.75f };
	float angle = dir == Up || dir == Down ? 90.f : 0.f;
	bool flipped = dir == Left || dir == Up;
	if (enabled < 1.f)
	{
		texBack.Draw(texBackDisabled, p, scale, angle, flipped, false, 1.f);
	}
	if (enabled > 0.f)
	{
		texBack.Draw(texBackEnabled, p, scale, angle, flipped, false, enabled);
		texFront.Draw(texArrows, p, scale, angle, flipped, false, enabled);
	}
	texFront.Draw(texOutline, p, scale, angle, flipped, true, 1.f);
}

Curve::Curve(Vector2 _pos, CurveType _type)
{
	pos = _pos;
	type = _type;
}

void Curve::Draw(Texture2D& lineTex, Texture2D& paintTex)
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
	DrawTexturePro(paintTex, srcRect, dstRect, origin, 0.f, WHITE);
	DrawTexturePro(lineTex, srcRect, dstRect, origin, 0.f, WHITE);
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
		if (HitCurve(point, { -16.f, 128.f }) && right.x ==  1.f && up.y ==  1.f) return {  90.f,  -80.f };
		if (HitCurve(point, { -96.f, -64.f }) && right.x ==  1.f && up.y == -1.f) return { -90.f, -240.f };
		if (HitCurve(point, { -64.f, -96.f }) && right.y ==  1.f && up.x == -1.f) return { 360.f,  240.f };
		if (HitCurve(point, { 128.f, -16.f }) && right.y ==  1.f && up.x ==  1.f) return { 180.f,   80.f };
	} 
	else if (type == SW)
	{
		if (HitCurve(point, { -16.f, 128.f }) && right.x == -1.f && up.y ==  1.f) return { 270.f,   80.f };
		if (HitCurve(point, {  64.f, -64.f }) && right.x == -1.f && up.y == -1.f) return {  90.f,  240.f };
		if (HitCurve(point, {  32.f, -96.f }) && right.y ==  1.f && up.x ==  1.f) return {   0.f, -240.f };
		if (HitCurve(point, {-160.f, -16.f }) && right.y ==  1.f && up.x == -1.f) return { 180.f,  -80.f };
	}
	else if (type == NE)
	{
		if (HitCurve(point, { -16.f,-160.f }) && right.x ==  1.f && up.y == -1.f) return {  90.f,   80.f };
		if (HitCurve(point, { -96.f,  32.f }) && right.x ==  1.f && up.y ==  1.f) return { 270.f,  240.f };
		if (HitCurve(point, { -64.f,  64.f }) && right.y == -1.f && up.x == -1.f) return { 180.f, -240.f };
		if (HitCurve(point, { 128.f, -16.f }) && right.y == -1.f && up.x ==  1.f) return {   0.f,  -80.f };
	}
	else
	{
		if (HitCurve(point, { -16.f,-160.f }) && right.x == -1.f && up.y == -1.f) return { -90.f,  -80.f };
		if (HitCurve(point, {  64.f,  32.f }) && right.x == -1.f && up.y ==  1.f) return {  90.f, -240.f };
		if (HitCurve(point, {  32.f,  64.f }) && right.y == -1.f && up.x ==  1.f) return { 180.f,  240.f };
		if (HitCurve(point, {-160.f, -16.f }) && right.y == -1.f && up.x == -1.f) return { 360.f,   80.f };
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
		   point.y > pos.y - size.y / 2.f && 
		   point.y < pos.x - size.y / 2.f;
}

Button GetButtonFromKeyPressed()
{
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
	return Button::None;
}