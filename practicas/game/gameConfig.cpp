#include <stdafx.h>
#include "gameConfig.h"

#include "character.h"
#include "pathfinding/pathfinder.h"

void Configure(MOAIGlobals* globals)
{
	REGISTER_LUA_CLASS(Pathfinder)
	REGISTER_LUA_CLASS(Character)
}