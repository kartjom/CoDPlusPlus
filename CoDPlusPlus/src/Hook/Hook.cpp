#include "Hook.h"

namespace Hook
{
	void Detour(DWORD hookAddress, void* jumpTo, int len, DWORD* ret)
	{
		*ret = hookAddress + len;

		DWORD protection;
		VirtualProtect((void*)hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);

		DWORD relativeAddress = ((DWORD)jumpTo - hookAddress) - 5;

		*(BYTE*)hookAddress = 0xE9; // JMP
		*(DWORD*)(hookAddress + 1) = relativeAddress;

		VirtualProtect((void*)hookAddress, len, protection, &protection);
	}

	void Patch(DWORD patchAddress, void* buffer, int len)
	{
		DWORD protection;
		VirtualProtect((void*)patchAddress, len, PAGE_EXECUTE_READWRITE, &protection);

		memcpy((void*)patchAddress, buffer, len);

		VirtualProtect((void*)patchAddress, len, protection, &protection);
	}
}