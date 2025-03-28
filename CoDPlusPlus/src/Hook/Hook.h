#pragma once
#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <format>

namespace Hook
{
	constexpr DWORD BaseAddress = 0x400000;

	class DetourHook
	{
	public:
		uintptr_t Address; // where we hook
		uintptr_t Trampoline; // where we jump to
		uintptr_t Return; // where we return to
		uintptr_t Length; // number of bytes we override

		std::vector<BYTE> OriginalBytes;

		inline DetourHook()
			: Address(0), Trampoline(0), Return(0), Length(0), OriginalBytes()
		{
		}

		inline void Inject(uintptr_t hookAddress, void* jumpTo, int len)
		{
			Address = hookAddress;
			Trampoline = (uintptr_t)jumpTo;
			Return = hookAddress + len;
			Length = len;

			DWORD protection;
			VirtualProtect((void*)hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);

			OriginalBytes.resize(len);
			memcpy_s(OriginalBytes.data(), len, (void*)hookAddress, len);

			DWORD relativeAddress = ((DWORD)jumpTo - hookAddress) - 5;

			*(BYTE*)hookAddress = 0xE9; // JMP
			*(DWORD*)(hookAddress + 1) = relativeAddress;

			VirtualProtect((void*)hookAddress, len, protection, &protection);
		}
	};

	template<typename FnType>
	class TrampolineHook
	{
	public:
		FnType OriginalFn;

		inline TrampolineHook()
			: OriginalFn(nullptr)
		{
		}

		inline void Inject(BYTE* original, BYTE* hooked, const uintptr_t len)
		{
			OriginalFn = (FnType)original;
			OriginalFn = (FnType)TrampHook32(original, hooked, len);
		}

		inline void Inject(const char* dll, const char* func, BYTE* hooked, const uintptr_t len)
		{
			BYTE* original = (BYTE*)GetProcAddress( GetModuleHandleA(dll), func );
			Inject(original, hooked, len);
		}

	private:
		inline void Detour32(BYTE* src, BYTE* dst, const uintptr_t len)
		{
			DWORD curProtection;
			VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

			uintptr_t relativeAddress = dst - src - 5;

			*src = 0xE9;

			*(uintptr_t*)(src + 1) = relativeAddress;

			VirtualProtect(src, len, curProtection, &curProtection);
		}

		inline BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
		{
			if (len < 5) return 0;

			//Create Gateway
			BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

			//write the stolen bytes to the gateway
			memcpy_s(gateway, len, src, len);

			//Get the gateway to destination address
			uintptr_t gatewayRelativeAddr = src - gateway - 5;

			// add the jmp opcode to the end of the gateway
			*(gateway + len) = 0xE9;

			//Write the address of the gateway to the jmp
			*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

			//Perform the detour
			Detour32(src, dst, len);

			return gateway;
		}
	};

	class MemoryPatch
	{
	public:
		std::vector<BYTE> OriginalBytes;

		inline MemoryPatch()
			: OriginalBytes()
		{
		}

		inline void Inject(uintptr_t patchAddress, const std::vector<BYTE>& bytes)
		{
			int length = bytes.size();

			DWORD protection;
			VirtualProtect((void*)patchAddress, length, PAGE_EXECUTE_READWRITE, &protection);

			OriginalBytes.resize(length);
			memcpy_s(OriginalBytes.data(), length, (void*)patchAddress, length);

			memcpy((void*)patchAddress, bytes.data(), length);

			VirtualProtect((void*)patchAddress, length, protection, &protection);
		}
	};
};