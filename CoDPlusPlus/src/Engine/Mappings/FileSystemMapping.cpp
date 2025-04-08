#include <Engine/CoDUO.h>

namespace CoDUO
{
	int FS_ReadFile(const char* qpath, void** buffer)
	{
		_asm
		{
			push buffer
			push qpath

			mov eax, 0x0042e600
			call eax

			add esp, 0x8
		}
	}
}