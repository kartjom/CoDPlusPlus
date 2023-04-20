#pragma once
#include <Windows.h>

#define DetourRet(hookAddress, jumpTo, len) Hook::DetourA(hookAddress, jumpTo, len, &jumpTo##_Ret)

class Hook
{
public:
	static constexpr DWORD BaseAddress = 0x400000;

	static void DetourA(DWORD hookAddress, void* jumpTo, int len, DWORD* ret);
	
	template <typename T>
	static T LoadFromDLL(const char* dll, const char* method);
};

template <typename T>
T Hook::LoadFromDLL(const char* dll, const char* method)
{
	return (T)GetProcAddress(GetModuleHandleA(dll), method);
}