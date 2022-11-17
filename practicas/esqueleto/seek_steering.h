#pragma once

#include <moaicore/MOAIEntity2D.h>

class Character;

class SeekSteering
{
public:
	SeekSteering(Character* character = nullptr, const USVec2D& targetPosition = {0,0});
	~SeekSteering();

	USVec2D GetSteering();
	void DrawDebug();

private:
	Character* mCharacter;
	USVec2D mTargetPosition;

	USVec2D drawPos, drawVel, drawAcc;
};

