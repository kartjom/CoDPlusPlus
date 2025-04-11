#pragma once
#include <Zydis/Zydis.h>
#include <Windows.h>
#include <stdexcept>
#include <vector>
#include <format>

namespace Hook
{
#pragma optimize("", off)
	struct Context
	{
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
		uint32_t esi;
		uint32_t edi;
	};
	inline volatile Context CapturedContext;

	inline __declspec(naked) void CaptureContext() noexcept
	{
		_asm
		{
			mov CapturedContext.eax, eax
			mov CapturedContext.ebx, ebx
			mov CapturedContext.ecx, ecx
			mov CapturedContext.edx, edx
			mov CapturedContext.esi, esi
			mov CapturedContext.edi, edi

			ret
		}
	}
#pragma optimize("", on)
}

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

	protected:
		inline uint32_t CalculateInstructionLength(uintptr_t hookAddress, uint32_t minLength = 5)
		{
			ZydisDecoder decoder;
			ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32);

			ZyanU32 totalLength = 0;
			ZydisDecodedInstruction instruction;
			while (totalLength < minLength)
			{
				if (ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, nullptr, (void*)(hookAddress + totalLength), 16, &instruction)))
				{
					totalLength += instruction.length;
				}
			}

			return totalLength;
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

		inline void Inject(uintptr_t hookAddress, void* jumpTo, int len = 0)
		{
			this->EnsureNotHooked("Detour hook is already applied");

			if (len < 5)
			{
				len = this->CalculateInstructionLength(hookAddress, 5);
			}

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

		inline void Dispose()
		{
			Address = 0;
			Trampoline = 0;
			Return = 0;
			Length = 0;

			OriginalBytes.clear();

			this->SetHooked(false);
		}

		inline void Remove()
		{
			this->EnsureHooked("Detour hook is not applied");

			DWORD protection;
			VirtualProtect((void*)Address, Length, PAGE_EXECUTE_READWRITE, &protection);
				memcpy((void*)Address, OriginalBytes.data(), Length);
			VirtualProtect((void*)Address, Length, protection, &protection);

			this->Dispose();
		}
	};

	/// <summary>
	/// Specialized Detour that captures registers state
	/// </summary>
	class DetourContextHook : public BaseHook
	{
	private:
		BYTE* Gateway; // Used for context capturing
		uintptr_t Address; // where we hook
		uintptr_t Trampoline; // where we jump to
		int Length; // number of bytes we override
		std::vector<BYTE> OriginalBytes;

	public:
		uintptr_t Return; // where we jump back

		inline DetourContextHook()
			: Gateway(nullptr), Address(0), Trampoline(0), Return(0), Length(0), OriginalBytes()
		{
		}

		inline void Inject(uintptr_t hookAddress, void* jumpTo, int len = 0)
		{
			this->EnsureNotHooked("Detour hook is already applied");

			if (len < 5)
			{
				len = this->CalculateInstructionLength(hookAddress, 5);
			}

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

			// Allocate our custom gateway
			Gateway = (BYTE*)VirtualAlloc(0, 12, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (!Gateway)
			{
				MessageBoxA(NULL, "Detour context hook gateway allocation failed", "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

			uintptr_t relativeCallAddress = (uintptr_t)Hook::CaptureContext - ((uintptr_t)Gateway + 5);
			uintptr_t relativeJmpAddress = (uintptr_t)jumpTo - ((uintptr_t)Gateway + 10);

			// Capture context and jump
			Gateway[0] = 0xE8; // CALL opcode
			*(uintptr_t*)(Gateway + 1) = relativeCallAddress; // Address of CaptureContext

			Gateway[5] = 0xE9; // JMP opcode
			*(uintptr_t*)(Gateway + 6) = relativeJmpAddress; // Jump to hooked function

			// Patch original function to jump to our gateway
			uintptr_t relativeAddress = ((uintptr_t)Gateway - hookAddress) - 5;
			*(BYTE*)hookAddress = 0xE9; // JMP
			*(uintptr_t*)(hookAddress + 1) = relativeAddress;

			VirtualProtect((void*)hookAddress, len, protection, &protection);

			this->SetHooked(true);
		}

		inline void Dispose()
		{
			Address = 0;
			Trampoline = 0;
			Return = 0;
			Length = 0;

			if (Gateway)
			{
				VirtualFree((void*)Gateway, 0, MEM_RELEASE);
				Gateway = nullptr;
			}

			OriginalBytes.clear();

			this->SetHooked(false);
		}

		inline void Remove()
		{
			this->EnsureHooked("Detour context hook is not applied");

			DWORD protection;
			VirtualProtect((void*)Address, Length, PAGE_EXECUTE_READWRITE, &protection);
				memcpy((void*)Address, OriginalBytes.data(), Length);
			VirtualProtect((void*)Address, Length, protection, &protection);

			this->Dispose();
		}
	};

	template<typename FnType>
	class TrampolineHook : public BaseHook
	{
	private:
		DetourContextHook Detour;

	public:
		FnType OriginalFn;

		inline TrampolineHook()
			: Detour(), OriginalFn(nullptr)
		{
		}

		inline void Inject(void* original, void* hooked, int len = 0)
		{
			OriginalFn = (FnType)original;
			OriginalFn = (FnType)TrampHook32((BYTE*)original, (BYTE*)hooked, len);
		}

		inline void Inject(uintptr_t original, void* hooked, int len = 0)
		{
			Inject((void*)original, hooked, len);
		}

		inline void Inject(const char* dll, const char* func, void* hooked, int len = 0)
		{
			void* original = (void*)GetProcAddress( GetModuleHandleA(dll), func );
			if (!original)
			{
				std::string message = std::format("Dll lookup failed for {} {}", dll, func);
				MessageBoxA(NULL, message.c_str(), "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

			Inject(original, hooked, len);
		}

		inline void Dispose()
		{
			Detour.Dispose();

			if (OriginalFn)
			{
				VirtualFree((void*)OriginalFn, 0, MEM_RELEASE);
				OriginalFn = nullptr;
			}

			this->SetHooked(false);
		}

		inline void Remove()
		{
			this->EnsureHooked("Trampoline hook is not applied");

			Detour.Remove();
			this->Dispose();
		}

	private:
		inline void* TrampHook32(BYTE* src, BYTE* dst, int len)
		{
			this->EnsureNotHooked("Trampoline hook is already applied");

			if (len < 5)
			{
				len = this->CalculateInstructionLength((uintptr_t)src, 5);
			}

			if (len < 5)
			{
				MessageBoxA(NULL, "Trampoline hook length must be at least 5 bytes", "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

			// Create Gateway
			BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (!gateway)
			{
				MessageBoxA(NULL, "Trampoline hook gateway allocation failed", "Error", MB_OK | MB_ICONERROR);
				exit(-1);
			}

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