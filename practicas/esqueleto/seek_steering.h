#pragma once

class Character;

class SeekSteering
{
public:
	SeekSteering();
	SeekSteering(Character* character, std::vector<USVec2D> *path);
	~SeekSteering();

	USVec2D GetSteering();
	void DrawDebug();

private:
	Character* mCharacter;
	std::vector<USVec2D>* mPath;

	USVec2D mTarget = USVec2D(0, 0);
	USVec2D mTargetAhead = USVec2D(0, 0);
	int mTargetIndex = 0;

	USVec2D drawPos, drawVel, drawAcc;
};

