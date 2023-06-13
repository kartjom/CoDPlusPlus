#include "LuaState.h"
#include "CoDUO.h"

using namespace CoDUO;

namespace LuaState
{
	inline lua_State* Instance;

	lua_State* Lua()
	{
		return Instance;
	}

	void Init()
	{
		Instance = luaL_newstate();
		luaL_openlibs(Instance);

		LuaState::SetMethods();
	}

	void Dispose()
	{
		lua_close(Instance);
	}

	void Clear()
	{
		Dispose();
		Init();
	}

	void SetMethods()
	{
		lua_register(Instance, "Scr_AddUndefined", [](lua_State * L) {
			Scr_AddUndefined();
			return 0;
		});

		lua_register(Instance, "Scr_AddEntityNum", [](lua_State * L) {
			int index = lua_tonumber(L, 1);

			Scr_AddEntityNum(index);
			return 0;
		});

		lua_register(Instance, "Scr_RunScript", [](lua_State * L) {
			int scriptHandle = lua_tonumber(L, 1);
			int argc = lua_tonumber(L, 2);

			Scr_RunScript(scriptHandle, argc);
			return 0;
		});
	}

	void SetCallbacks()
	{
		lua_pushnumber(Instance, CodeCallback.OnPlayerShoot);
		lua_setglobal(Instance, "CodeCallback_OnPlayerShoot");

		lua_pushnumber(Instance, CodeCallback.OnPlayerMelee);
		lua_setglobal(Instance, "CodeCallback_OnPlayerMelee");

		lua_pushnumber(Instance, CodeCallback.OnProjectileBounce);
		lua_setglobal(Instance, "CodeCallback_OnProjectileBounce");

		lua_pushnumber(Instance, CodeCallback.OnProjectileExplode);
		lua_setglobal(Instance, "CodeCallback_OnProjectileExplode");
	}
}