#include <Engine/CoDUO.h>
#include <Hook/Detours.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	_declspec(naked) void VehicleCrashFix_n() noexcept
	{
		_asm
		{
			test eax, eax // check eax for nullptr
			jz eax_null // jump if null
		}

		_asm // restore
		{
			mov edx, dword ptr[eax + 0xd0]
		}

		_asm jmp[VehicleCrashFixHook.Return] // jump back

		eax_null:
		_asm // eax is null
		{
			mov esi, uo_game_mp_x86
			add esi, 0x0001b4d1
			jmp esi
		}
	}

	_declspec(naked) void VEH_UnlinkPlayerFix_n() noexcept
	{
		// Disables 'VEH_UnlinkPlayer: Player is not using a vehicle' script error
		_asm
		{
			pop edi
			pop esi
			mov eax, 0 // return value
			pop ebx
			add esp, 0x10
			ret
		}
	}

	// This can randomly throw access violation during map change
    int __cdecl hkFUN_00421510()
    {
        __try
        {
			float* param_1 = (float*)FUN_00421510Hook.CapturedContext.ecx;
			int* param_2 = (int*)FUN_00421510Hook.CapturedContext.eax;

            FUN_00421510Hook.SetECX(param_1);
            FUN_00421510Hook.SetEAX(param_2);
            return FUN_00421510Hook.Invoke();
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
			return 0;
        }
    }
}