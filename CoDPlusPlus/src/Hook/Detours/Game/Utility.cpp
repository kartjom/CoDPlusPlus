#include <Engine/CoDUO.h>
#include <Hook/Detours.h>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void __cdecl hkG_RunFrame(int levelTime)
	{
		G_RunFrameHook.Invoke(levelTime);
	}
}