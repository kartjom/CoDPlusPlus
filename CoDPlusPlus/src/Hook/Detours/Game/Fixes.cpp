#include <Engine/CoDUO.h>
#include <Hook/Detours.h>
#include <print>

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

	// Prevents 'VEH_UnlinkPlayer: Player is not using a vehicle' script error
	qboolean __cdecl hkVEH_UnlinkPlayer(int param_1)
	{
		gentity_t* player = (gentity_t*)VEH_UnlinkPlayerHook.CapturedContext.eax;

		// Player is not using vehicle
		if ((player->client->eFlags & 0x100000U) == 0)
		{
			std::println("VEH_UnlinkPlayer: Trying to prevent script error for - {}", player->client->name);

			// Simulate player sitting in vehicle
			player->client->eFlags |= 0x100000U;

			// Check if our current vehicle is valid
			if (g_entities[player->ownerNum].eType != ET_VEHICLE)
			{
				// If not, try to retrieve last used vehicle
				if (g_entities[player->otherEntityNum].eType == ET_VEHICLE)
				{
					player->ownerNum = player->otherEntityNum;
					std::println("VEH_UnlinkPlayer: Retrieved last used vehicle - {}", player->ownerNum);
				}
				else
				{
					player->client->eFlags &= ~0x100000U;
					player->otherEntityNum = 0;
					player->ownerNum = 1023;

					std::println("VEH_UnlinkPlayer: Could not unlink {} - preventing crash; vehicle may still be active", player->client->name);
					return 1;
				}
			}
		}

		VEH_UnlinkPlayerHook.SetEAX(player);
		return VEH_UnlinkPlayerHook.Invoke(param_1);
	}

	// Prevents crash from random access violation during map change - temp solution
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