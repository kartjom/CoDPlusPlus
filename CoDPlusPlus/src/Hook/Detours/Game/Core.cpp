#include <Hook/Detours.h>
#include <Engine/CoDUO.h>
#include <Utils/String/String.h>
#include <Engine/MapBindings/MapBindings.h>
#include <Engine/ScriptLayer/Gsc/ScrVar/ScrVar.h>
#include <format>
#include <print>

using namespace CoDUO;
using namespace CoDUO::Gsc;
namespace Hook::Detour
{
	uint16_t __cdecl hkScr_ExecThread(int32_t scriptHandle, uint32_t argc)
	{
		typedef uint16_t(__cdecl* VM_Execute_t)(uint16_t, uint32_t, uint32_t);
		VM_Execute_t VM_Execute = (VM_Execute_t)(0x0048f0b0);

		int32_t& scrVmPub_function_count = *(int32_t*)(0x00b6acc8);
		uint32_t& scrVmGlob_starttime = *(uint32_t*)(0x00b6acc4);
		uint16_t& scrVarPub_levelId = *(uint16_t*)(0x00b6ac8c);
		int32_t& scrVarPub_programBuffer = *(int32_t*)(0x0389fe18);
		VariableValue*& scrVmPub_top = *(VariableValue**)(0x00b6ac90);
		uint32_t& scrVmPub_inparamcount = *(uint32_t*)(0x00b6acd4);

		if (scrVmPub_function_count == 0)
		{
			scrVmGlob_starttime = (uint32_t)__rdtsc();
		}

		uint16_t callback = VM_Execute(scrVarPub_levelId, scrVarPub_programBuffer + scriptHandle, argc);

		// Capture the return value before it's disposed
		Scr_InternalReturnValue = ScrVar::From(scrVmPub_top);

		switch (scrVmPub_top->type)
		{
		case VarType::String:
		case VarType::LocalizedString:
			SL_RemoveRefToString(scrVmPub_top->stringValue);
			break;
		case VarType::Vector:
			RemoveRefToVector(scrVmPub_top->vectorValue);
			break;
		case VarType::Object:
			RemoveRefToObject(scrVmPub_top->pointerValue);
		}

		scrVmPub_top->type = VarType::Undefined;
		--scrVmPub_top;
		--scrVmPub_inparamcount;

		return callback;
	}

	void __cdecl hkSV_Map()
	{
		const char* mapName = Cmd_Argv(1);

		if (mapName)
		{
			std::string expanded = std::format("maps/mp/{}.bsp", mapName);
			String::TransformToLower(expanded);

			if (FS_ReadFile(expanded.c_str(), NULL) > 0)
			{
				std::string binding = MapBindings::GetBindingForMap(mapName);

				if (!binding.empty())
				{
					Cvar_Set("fs_game", binding.c_str(), 1);
					std::println("[CoDPlusPlus] - Changed fs_game: {}", binding);
				}
			}
		}

		SV_MapHook.Invoke();
	}
}