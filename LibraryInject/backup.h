//#include <Windows.h>
//#include <iostream>
//#include <stdexcept>
//#include "lua.hpp"
//
//typedef int(__cdecl* Scr_LoadScript_t)(const char* file);
//typedef int(__cdecl* Scr_GetFunctionHandle_t)(const char* file, const char* function);
//typedef int(__cdecl* Scr_ExecThread_t)(int handle, unsigned int paramCount);
//typedef int(__cdecl* Scr_FreeThread_t)(int handle);
//
//typedef int(__stdcall* LoadCallbacks_t)();
//
////Scr_LoadScript_t Scr_LoadScript = (Scr_LoadScript_t)(0x480150);
//Scr_LoadScript_t Scr_LoadScript;
//Scr_GetFunctionHandle_t Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)(0x47FE50);
//Scr_ExecThread_t Scr_ExecThread = (Scr_ExecThread_t)(0x48F3E0);
//Scr_FreeThread_t Scr_FreeThread = (Scr_FreeThread_t)(0x48F640);
//
//LoadCallbacks_t LoadCallbacks;
//
//static lua_State* lua;
//
//bool functionsInitialized = false;
//
//bool callbackInitialized = false;
//int Handle_MyCustomCallback = 0;
//int threadHandle = 0;
//
//DWORD appAddr;
//DWORD mpModule;
//
//void Detour(DWORD hookAddress, void* jumpTo, int len, DWORD* ret)
//{
//	*ret = hookAddress + len;
//
//	DWORD protection;
//	VirtualProtect((void*)hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);
//
//	DWORD relativeAddress = ((DWORD)jumpTo - hookAddress) - 5;
//
//	*(BYTE*)hookAddress = 0xE9; // JMP
//	*(DWORD*)(hookAddress + 1) = relativeAddress;
//
//	VirtualProtect((void*)hookAddress, len, protection, &protection);
//}
//
//DWORD Shoot_Ret;
//_declspec(naked) void Shoot_Func()
//{
//	static int bullets;
//
//	_asm
//	{
//		pushad
//		pushfd
//	}
//
//	{
//		_asm {
//			mov bullets, esi
//		}
//
//		/*int loaded;
//		if (!callbackInitialized)
//		{
//			loaded = Scr_LoadScript("maps/mp/scripts");
//			Handle_MyCustomCallback = Scr_GetFunctionHandle("maps/mp/scripts", "PlayerShoot");
//
//			if (Handle_MyCustomCallback != 0)
//			{
//				callbackInitialized = true;
//			}
//		}
//
//		if (Handle_MyCustomCallback != 0)
//		{
//			int threadHandle = Scr_ExecThread(Handle_MyCustomCallback, 0);
//			Scr_FreeThread(threadHandle);
//		}*/
//
//		lua_getglobal(lua, "Callback_PlayerShoot");
//		lua_pushnumber(lua, bullets);
//		lua_pcall(lua, 1, 1, 0);
//
//		bullets = lua_tonumber(lua, -1);
//		lua_pop(lua, 1);
//	}
//
//	_asm
//	{
//		popfd
//		popad
//		mov esi, bullets
//		sub dword ptr ds : [ecx] , esi
//		mov edx, dword ptr ds : [edx]
//		mov ecx, dword ptr ds : [edx + eax * 0x4 + 0x334]
//		jmp[Shoot_Ret]
//	}
//}
//
//DWORD LoadScript_Ret;
//__declspec(naked) void LoadScript()
//{
//	_asm
//	{
//		call LoadCallbacks
//		pushad
//		pushfd
//	}
//
//	Scr_LoadScript("maps/mp/gametypes/_callbacksetup");
//	//Handle_MyCustomCallback = Scr_GetFunctionHandle("maps/mp/scripts", "PlayerShoot");
//
//	_asm
//	{
//		popfd
//		popad
//		jmp[LoadScript_Ret]
//	}
//}
//
//int L_print(lua_State* L)
//{
//	const char* str = lua_tostring(L, 1);
//	std::cout << str << std::endl;
//
//	return 0;
//}
//
//void printHex(int value)
//{
//	std::cout << std::hex << value << std::dec << std::endl;
//}
//
//DWORD WINAPI MainThread(LPVOID param)
//{
//	AllocConsole();
//	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
//
//	appAddr = (DWORD)GetModuleHandleA(NULL);
//	mpModule = (DWORD)GetModuleHandleA("uo_game_mp_x86.dll");
//
//	LoadCallbacks = (LoadCallbacks_t)(mpModule + 0x361C0);
//	Scr_LoadScript = (Scr_LoadScript_t)(mpModule + 0x10ECD4);
//
//	lua = luaL_newstate();
//	lua_register(lua, "print", L_print);
//
//	Detour(mpModule + 0x122AD, Shoot_Func, 11, &Shoot_Ret);
//	Detour(mpModule + 0x36536, LoadScript, 5, &LoadScript_Ret);
//
//	while (true)
//	{
//		luaL_dofile(lua, "cod_main.lua");
//		Sleep(1000);
//	}
//
//	//FreeLibraryAndExitThread((HMODULE)param, 0);
//
//	return 0;
//}
//
//BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
//{
//	switch (dwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		CreateThread(0, 0, MainThread, hModule, 0, 0);
//		break;
//	}
//
//	return TRUE;
//}