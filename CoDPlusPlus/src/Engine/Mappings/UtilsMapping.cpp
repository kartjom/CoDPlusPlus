#include <Engine/CoDUO.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

namespace CoDUO
{
	void trap_Trace(trace_t* trace, vec3_t* start, vec3_t* end, int passEntityNum, int contentMask)
	{
		int bulletPriorityMap = uo_game_mp_x86 + 0x0008699c;
		syscall(0x2e, trace, start, end, passEntityNum, contentMask, bulletPriorityMap);
	}
}

namespace CoDUO
{
	void AngleVectors(float* angles, float* forward, float* right, float* up)
	{
		_asm
		{
			mov edx, angles
			mov esi, forward
			mov edi, right
			mov ebx, up

			mov eax, uo_game_mp_x86
			add eax, 0x00017250
			call eax
		}
	}

	void Bullet_Endpos(float spread, float* forward, float* out_vec)
	{
		_asm
		{
			push spread

			mov edi, forward
			mov esi, out_vec

			mov eax, uo_game_mp_x86
			add eax, 0x0000f7d0
			call eax

			add esp, 0x4
		}
	}

	trace_t G_GetEyeTrace(gentity_t* player, float range, float spread)
	{
		trace_t tr{};

		if (player->client)
		{
			float trace_spread = spread != std::numeric_limits<float>::lowest() ? spread : BG_GetAimSpread(player);

			vec3_t vec[4];
			G_GetPlayerViewOrigin(player, vec[3]);

			vec3_t viewangles = { player->client->viewangles2.x , player->client->viewangles2.y , player->client->viewangles.z };
			AngleVectors(viewangles, vec[0], vec[1], vec[2]);

			vec3_t end;
			Bullet_Endpos(trace_spread, vec[0], end);

			if (range != std::numeric_limits<float>::lowest())
			{
				SetRayLength(vec[3], end, range);
			}

			trap_Trace(&tr, &vec[3], &end, player->number, MASK_SHOT);
		}

		return tr;
	}

	float GetWeaponSpread_00011890(int fullyAiming, int time, int weaponIndex, gclient_t* client)
	{
		_asm
		{
			push fullyAiming

			mov eax, time
			mov edx, weaponIndex
			mov ecx, client

			mov esi, uo_game_mp_x86
			add esi, 0x00011890
			call esi

			add esp, 0x4
		}
	}

	float BG_GetAimSpread(gentity_t* ent)
	{
		int level_time = *(int*)(uo_game_mp_x86 + 0x0030fcac);

		char* weaponDef = (char*)BG_GetWeaponDef(ent->weapon);

		if (weaponDef)
		{
			float unknown = *(float*)((char*)(ent->client) + 0x4698);

			float stanceSpread = GetWeaponSpread_00011890(ent->client->aimProgress == 1.0, level_time, ent->weapon, ent->client);
			float spread = (*(float*)(weaponDef + 0x2a4) - stanceSpread) * unknown + stanceSpread;

			return spread;
		}

		return 0;
	}

	void SetRayLength(vec3_t& start, vec3_t& end, float newLength)
	{
		vec3_t direction = { end.x - start.x, end.y - start.y, end.z - start.z };
		float currentLength = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		// If the current length is not zero, scale the direction vector to the new length
		if (currentLength != 0) {
			float scale = newLength / currentLength;
			direction.x *= scale;
			direction.y *= scale;
			direction.z *= scale;

			// Update the end point of the ray
			end.x = start.x + direction.x;
			end.y = start.y + direction.y;
			end.z = start.z + direction.z;
		}
	}
}