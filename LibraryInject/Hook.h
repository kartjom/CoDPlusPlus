#pragma once
#include <Windows.h>
#include <stdexcept>

#define DetourRet(hookAddress, jumpTo, len) Hook::Detour(hookAddress, jumpTo##_n, len, &jumpTo##_r)

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
	void* functionPtr = GetProcAddress(GetModuleHandleA(dll), method);
	if (functionPtr == nullptr)
	{
		throw std::runtime_error("Failed to load function from DLL");
	}

	return (T)functionPtr;
}