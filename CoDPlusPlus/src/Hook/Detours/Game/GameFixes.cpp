#include <Engine/CoDUO.h>
#include <Hook/Detours.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Detours
{
	ImplementDetour(VehicleCrashFix)
	{
		_asm
		{
			test eax, eax // check eax for nullptr
			jz eax_null // jump if null
		}

		_restore
		{
			mov edx, dword ptr[eax + 0xd0]
		}

		JumpBack(VehicleCrashFix)

		eax_null:
		_asm // eax is null
		{
			mov esi, uo_game_mp_x86
			add esi, 0x0001b4d1
			jmp esi
		}
	}

	ImplementDetour(VEH_UnlinkPlayerFix)
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
}