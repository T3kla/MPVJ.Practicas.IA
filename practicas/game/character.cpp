#include <stdafx.h>
#include "character.h"
#include "pathfinding/pathfinder.h"

static Character* MainChar = nullptr;
static vector<USVec2D> *Path = nullptr;

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND (MOAIEntity2D)
	RTTI_END

	MainChar = this;
}

Character::~Character()
{

}

void Character::OnStart()
{
	ReadParams("params.xml", mParams);
	mSteering = SeekSteering(this, Pathfinder::GetPath());
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	mLinearVelocity += mSteering.GetSteering() * step;

	SetLoc(GetLoc() + mLinearVelocity * step);
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);

	mSteering.DrawDebug();
}

void Character::Reset()
{
	//MainChar->SetLoc();
}

// Lua configuration

void Character::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity2D::RegisterLuaFuncs(state);
	
	luaL_Reg regTable [] = {
		{ "setLinearVel",			_setLinearVel},
		{ "setAngularVel",			_setAngularVel},
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Character::_setLinearVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(2, 0.0f);
	self->SetLinearVelocity(pX, pY);
	return 0;	
}

int Character::_setAngularVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float angle = state.GetValue<float>(2, 0.0f);
	self->SetAngularVelocity(angle);

	return 0;
}
	