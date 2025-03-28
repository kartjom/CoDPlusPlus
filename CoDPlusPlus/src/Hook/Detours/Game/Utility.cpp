#include <Engine/CoDUO.h>
#include <Hook/Detours.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void ServerTick();
}

namespace Hook::Detour
{
	_declspec(naked) void ServerTick_n() noexcept
	{
		_asm pushad
		ServerTick();
		_asm popad

		_asm // restore
		{
			sub esp, 0x254
		}

		_asm jmp[ServerTickHook.Return] // jump back
	}
}

namespace Hook::Detour
{
	void ServerTick()
	{
	}
}