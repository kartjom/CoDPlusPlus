#include "LuaState.h"
#include "CoDUO.h"
#include <iostream>

using namespace CoDUO;
using namespace CoDUO::Gsc;
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
		lua_register(Instance, "Scr_RunScript", [](lua_State* L) {
			int scriptHandle = lua_tonumber(L, 1);
			int argc = lua_tonumber(L, 2);

			Scr_RunScript(scriptHandle, argc);
			return 0;
		});

		lua_register(Instance, "Scr_AddUndefined", [](lua_State* L) {
			Scr_AddUndefined();
			return 0;
		});

		lua_register(Instance, "Scr_AddBool", [](lua_State* L) {
			int arg1 = lua_toboolean(L, 1);

			Scr_AddBool(arg1);
			return 0;
		});

		lua_register(Instance, "Scr_AddInt", [](lua_State* L) {
			int arg1 = (int)lua_tonumber(L, 1);

			Scr_AddInt(arg1);
			return 0;
		});

		lua_register(Instance, "Scr_AddFloat", [](lua_State* L) {
			float arg1 = (float)lua_tonumber(L, 1);

			Scr_AddFloat(arg1);
			return 0;
		});

		lua_register(Instance, "Scr_AddString", [](lua_State* L) {
			const char* arg1 = lua_tostring(L, 1);

			Scr_AddString(arg1);
			return 0;
		});

		lua_register(Instance, "Scr_AddEntityNum", [](lua_State* L) {
			int arg1 = (int)lua_tonumber(L, 1);

			Scr_AddEntityNum(arg1);
			return 0;
		});

		lua_register(Instance, "Scr_MakeArray", [](lua_State* L) {
			Scr_MakeArray();
			return 0;
		});

		lua_register(Instance, "Scr_AddArray", [](lua_State* L) {
			Scr_AddArray();
			return 0;
		});

		lua_register(Instance, "Scr_AddArrayStringIndexed", [](lua_State* L) {
			int arg1 = (int)lua_tonumber(L, 1);

			Scr_AddArrayStringIndexed(arg1);
			return 0;
		});

		lua_register(Instance, "G_NewString", [](lua_State* L) {
			const char* arg1 = lua_tostring(L, 1);

			int ret = G_NewString(arg1);
			lua_pushnumber(L, ret);

			return 1;
		});

		lua_register(Instance, "SL_ConvertToString", [](lua_State* L) {
			int arg1 = (int)lua_tonumber(L, 1);

			const char* ret = SL_ConvertToString(arg1);
			lua_pushstring(L, ret);

			return 1;
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