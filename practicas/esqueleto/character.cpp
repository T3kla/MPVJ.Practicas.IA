#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include <params.h>

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND (MOAIEntity2D)
	RTTI_END

	mSteering = SeekSteering(this, USVec2D(100.0f, 100.0f));
}

Character::~Character()
{

}

void Character::OnStart()
{
    ReadParams("params.xml", mParams);
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	// Acceleration
	//auto acceleration = USVec2D(-20.f,9.8f);
	//mLinearVelocity += acceleration * step;

	mLinearVelocity += mSteering.GetSteering() * step;
	
	// Position
	SetLoc(GetLoc() + mLinearVelocity * step);
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);

	mSteering.DrawDebug();

	//MOAIDraw::DrawPoint(0.0f, 0.0f);
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
	float pY = state.GetValue<float>(3, 0.0f);
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
	