#include "seek_steering.h"
#include "stdafx.h"
#include "character.h"

SeekSteering::SeekSteering(Character* character, const USVec2D& targetPosition)
	: mCharacter(character)
	, mTargetPosition(targetPosition)
{
}

SeekSteering::~SeekSteering()
{
}

USVec2D SeekSteering::GetSteering()
{
	if(!mCharacter)
		return USVec2D(0, 0);

	auto pos = mCharacter->GetLoc();

	auto initVel = mCharacter->GetLinearVelocity();
	auto targetVel = mTargetPosition - pos;
	auto acceleration = targetVel - initVel;
	
	acceleration.SetLength(mCharacter->mParams.max_acceleration);
	
	drawPos = pos;
	drawVel = targetVel;
	drawAcc = acceleration;

	return acceleration;
}

void SeekSteering::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	gfxDevice.SetPenColor(1.0f, 0.3f, 0.3f, 0.7f);
	MOAIDraw::DrawLine(drawPos, drawPos + drawVel);
	gfxDevice.SetPenColor(0.4f, 1.0f, 0.6f, 0.7f);
	MOAIDraw::DrawLine(drawPos, drawPos + drawAcc);
}
