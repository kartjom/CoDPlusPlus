#include <Engine/CoDUO.h>

namespace CoDUO
{
	void RemoveRefToVector(vec3_t* vectorValue)
	{
		_asm
		{
			mov ecx, vectorValue
			mov edx, 0x004841c0
			call edx
		}
	}

	void RemoveRefToObject(uint32_t id)
	{
		_asm
		{
			push id
			mov eax, 0x00484100
			call eax

			add esp, 0x4
		}
	}
}