#include "CoDUO.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

namespace CoDUO
{
	void trap_Trace(trace_t* trace, vec3_t* start, vec3_t* end, int passEntityNum, int contentMask)
	{
		_asm
		{
			mov eax, uo_game_mp_x86
			add eax, 0x0008699c
			mov eax, [eax]
			push eax
			push contentMask
			push passEntityNum
			push end
			push start
			push trace
			push 0x2e
			call syscall

			add esp, 0x1C
		}
	}
}

namespace CoDUO
{
	vec3_t AnglesToDirection(vec3_t& viewangles, float spread)
	{
		// Seed the random number generator
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(-spread / 2.0, spread / 2.0);

		// Convert degrees to radians
		float yaw = viewangles.y * (M_PI / 180.0);
		float pitch = viewangles.x * (M_PI / 180.0);

		// Apply random spread
		if (spread != 0)
		{
			yaw += dist(gen);
			pitch += dist(gen);
		}

		// Calculate the direction vector
		vec3_t direction;
		direction.x = cos(yaw) * cos(pitch);
		direction.z = -sin(pitch);
		direction.y = sin(yaw) * cos(pitch);

		return direction;
	}

	trace_t G_GetEyeTrace(gentity_t* player, float range, float spread)
	{
		trace_t tr{};

		if (player->client)
		{
			vec3_t start;
			G_GetPlayerViewOrigin(player, start);

			vec3_t dir = AnglesToDirection(player->client->viewangles, spread);
			vec3_t end = start + (dir * range);

			trap_Trace(&tr, &start, &end, player->number, MASK_SHOT);
		}

		return tr;
	}
}