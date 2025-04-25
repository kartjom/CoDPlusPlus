#include <Engine/CoDUO.h>

namespace CoDUO
{
	int __cdecl FS_ReadFile(const char* qpath, void** buffer)
	{
		auto FS_ReadFile_f = (decltype(FS_ReadFile)*)(0x0042e600);
		return FS_ReadFile_f(qpath, buffer);
	}
}