#pragma once
#include <lua.hpp>

namespace LuaState
{
	lua_State* Lua();
	void Init();
	void Dispose();
	void Clear();

	void SetMethods();
	void SetCallbacks();
}