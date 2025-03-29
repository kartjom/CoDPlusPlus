#pragma once
#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <format>

namespace Hook
{
	class BaseHook
	{
	protected:
		bool _IsHooked = false;

	public:
		inline bool IsHooked() const
		{
			return _IsHooked;
		}

		inline void SetHooked(bool value)
		{
			_IsHooked = value;
		}

		inline void EnsureHooked(const char* errorMessage)
		{
			if (!this->IsHooked())
			{
				MessageBoxA(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}
		}

		inline void EnsureNotHooked(const char* errorMessage)
		{
			if (this->IsHooked())
			{
				MessageBoxA(NULL, errorMessage, "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}
		}
	};

	class DetourHook : public BaseHook
	{
	private:
		uintptr_t Address; // where we hook
		uintptr_t Trampoline; // where we jump to
		int Length; // number of bytes we override
		std::vector<BYTE> OriginalBytes;

	public:
		uintptr_t Return; // where we jump back

		inline DetourHook()
			: Address(0), Trampoline(0), Return(0), Length(0), OriginalBytes()
		{
		}

		inline void Inject(uintptr_t hookAddress, void* jumpTo, int len)
		{
			this->EnsureNotHooked("Detour hook is already applied");

			if (len < 5)
			{
				MessageBoxA(NULL, "Detour length must be at least 5 bytes", "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

			Address = hookAddress;
			Trampoline = (uintptr_t)jumpTo;
			Return = hookAddress + len;
			Length = len;

			DWORD protection;
			VirtualProtect((void*)hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);

			OriginalBytes.resize(len);
			memcpy_s(OriginalBytes.data(), len, (void*)hookAddress, len);

			uintptr_t relativeAddress = ((uintptr_t)jumpTo - hookAddress) - 5;

			*(BYTE*)hookAddress = 0xE9; // JMP
			*(uintptr_t*)(hookAddress + 1) = relativeAddress;

			VirtualProtect((void*)hookAddress, len, protection, &protection);

			this->SetHooked(true);
		}

		inline void Remove()
		{
			this->EnsureHooked("Detour hook is not applied");

			DWORD protection;
			VirtualProtect((void*)Address, Length, PAGE_EXECUTE_READWRITE, &protection);
			memcpy((void*)Address, OriginalBytes.data(), Length);
			VirtualProtect((void*)Address, Length, protection, &protection);

			Address = 0;
			Trampoline = 0;
			Return = 0;
			Length = 0;
			OriginalBytes.clear();

			this->SetHooked(false);
		}
	};

	template<typename FnType>
	class TrampolineHook : public BaseHook
	{
	private:
		DetourHook Detour;

	public:
		FnType OriginalFn;

		inline TrampolineHook()
			: Detour(), OriginalFn(nullptr)
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

		inline void Remove()
		{
			this->EnsureHooked("Trampoline hook is not applied");

			Detour.Remove();
			VirtualFree((void*)OriginalFn, 0, MEM_RELEASE);

			OriginalFn = nullptr;

			this->SetHooked(false);
		}

	private:
		inline BYTE* TrampHook32(BYTE* src, BYTE* dst, const uintptr_t len)
		{
			this->EnsureNotHooked("Trampoline hook is already applied");

			if (len < 5)
			{
				MessageBoxA(NULL, "Trampoline hook length must be at least 5 bytes", "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

			// Create Gateway
			BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

			// Write the stolen bytes to the gateway
			memcpy_s(gateway, len, src, len);

			// Get the gateway to destination address
			uintptr_t gatewayRelativeAddr = src - gateway - 5;

			// Add the jmp opcode to the end of the gateway
			*(gateway + len) = 0xE9;

			// Write the address of the gateway to the jmp
			*(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

			// Perform the detour
			Detour.SetHooked(false);
			Detour.Inject((uintptr_t)src, dst, len);

			this->SetHooked(true);

			return gateway;
		}
	};

	class MemoryPatch : public BaseHook
	{
	private:
		uintptr_t Address;
		int Length;
		std::vector<BYTE> OriginalBytes;

	public:
		inline MemoryPatch()
			: Address(0), Length(0), OriginalBytes()
		{
		}

		inline void Inject(uintptr_t patchAddress, const std::vector<BYTE>& bytes)
		{
			this->EnsureNotHooked("Patch is already applied");

			Address = patchAddress;
			Length = bytes.size();

			DWORD protection;
			VirtualProtect((void*)patchAddress, Length, PAGE_EXECUTE_READWRITE, &protection);

			OriginalBytes.resize(Length);
			memcpy_s(OriginalBytes.data(), Length, (void*)patchAddress, Length);

			memcpy((void*)patchAddress, bytes.data(), Length);
			VirtualProtect((void*)patchAddress, Length, protection, &protection);

			this->SetHooked(true);
		}

		inline void Remove()
		{
			this->EnsureHooked("Patch is not applied");

			DWORD protection;
			VirtualProtect((void*)Address, Length, PAGE_EXECUTE_READWRITE, &protection);
			memcpy((void*)Address, OriginalBytes.data(), Length);
			VirtualProtect((void*)Address, Length, protection, &protection);

			Address = 0;
			OriginalBytes.clear();

			this->SetHooked(false);
		}
	};
};