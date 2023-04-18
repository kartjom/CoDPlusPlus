#pragma once
#include <Windows.h>

class Hook
{
public:
	static constexpr DWORD BaseAddress = 0x400000;

	static void Detour(DWORD hookAddress, void* jumpTo, int len, DWORD* ret);
	
	template <typename T>
	static T LoadFromDLL(const char* dll, const char* method);
};

template <typename T>
T Hook::LoadFromDLL(const char* dll, const char* method)
{
	return (T)GetProcAddress(GetModuleHandleA(dll), method);
}