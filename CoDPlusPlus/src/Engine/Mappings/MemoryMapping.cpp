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

	void __cdecl RemoveRefToObject(uint32_t id)
	{
		auto RemoveRefToObject_f = (decltype(RemoveRefToObject)*)(0x00484100);
		RemoveRefToObject_f(id);
	}
}