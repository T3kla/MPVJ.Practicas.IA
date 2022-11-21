#include "seek_steering.h"

#include "stdafx.h"
#include "character.h"

SeekSteering::SeekSteering()
{
}

SeekSteering::SeekSteering(Character* character = nullptr, std::vector<USVec2D>* path = nullptr)
	: mCharacter(character)
	, mPath(path)
{
}

SeekSteering::~SeekSteering()
{
}

USVec2D SeekSteering::GetSteering()
{
	if(!mCharacter)
		return USVec2D(0, 0);
	if(!mPath)
		return USVec2D(0, 0);

	auto pos = mCharacter->GetLoc();

	// Calculate target position
	mTarget = mPath->at(mTargetIndex);

	auto dir = mTarget - pos;
	if(dir.Length()< mCharacter->mParams.arrive_radius)
		if (++mTargetIndex >= mPath->size())
			mTargetIndex = 0;
	
	// calculate target ahead position
	mTargetAhead = mTarget;
	
	auto ahead = mCharacter->mParams.look_ahead;
	if (mTargetIndex < mPath->size() - 1)
	{
		auto projected = mPath->at(mTargetIndex + 1) - mTarget;
		projected.SetLength(ahead);
		mTargetAhead += projected;
	}
	
	// Calculate the desired velocity
	auto initVel = mCharacter->GetLinearVelocity();
	auto targetVel = mTargetAhead - pos;
	auto acceleration = targetVel - initVel;
	
	if(acceleration.Length() > mCharacter->mParams.max_acceleration)
		acceleration.SetLength(mCharacter->mParams.max_acceleration);
	
	drawPos = pos;
	drawVel = targetVel;
	drawAcc = acceleration;

	return acceleration;
}

void SeekSteering::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	gfxDevice.SetPenColor(1.0f, 0.3f, 0.3f, 0.6f);
	MOAIDraw::DrawLine(drawPos, drawPos + drawVel);
	gfxDevice.SetPenColor(0.4f, 1.0f, 0.6f, 0.6f);
	MOAIDraw::DrawLine(drawPos, drawPos + drawAcc);
	gfxDevice.SetPenColor(0.8f, 0.4f, 0.8f, 0.8f);
	MOAIDraw::DrawEllipseFill(mTarget.mX, mTarget.mY, 5.0f, 5.0f, 14);
	gfxDevice.SetPenColor(0.5f, 0.4f, 0.8f, 0.8f);
	MOAIDraw::DrawEllipseFill(mTargetAhead.mX, mTargetAhead.mY, 8.0f, 8.0f, 14);
}
