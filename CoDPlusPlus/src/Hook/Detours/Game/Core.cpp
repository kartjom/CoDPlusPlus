#include <Hook/Detours.h>
#include <Engine/CoDUO.h>
#include <Utils/String/String.h>
#include <Engine/MapBindings/MapBindings.h>
#include <format>
#include <print>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void Scr_ExecThread_GscReturnValue();
}

namespace Hook::Detour
{
	_declspec(naked) void ScrThread_ReturnValue_n() noexcept
	{
		_asm // restore
		{
			mov eax, 0x0048f0b0
			call eax
		}

		_asm pushad
		Scr_ExecThread_GscReturnValue();
		_asm popad

		_asm jmp[ScrThread_ReturnValueHook.Return] // jump back
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

		SV_MapHook.OriginalFn();
	}
}

namespace Hook::Detour
{
	void __cdecl Scr_ExecThread_GscReturnValue()
	{
		try
		{
			uintptr_t addrToStackTop = *(uintptr_t*)(0x00b6ac90);
			VariableValue* var = (VariableValue*)addrToStackTop;

			Scr_ReturnValue = {};
			Scr_ReturnValue.Type = (VarType)var->type;
			switch (Scr_ReturnValue.Type)
			{
			case VarType::Undefined:
				Scr_ReturnValue.Integer = 0;
				break;
			case VarType::String:
			{
				const char* str = SL_ConvertToString(var->StringIndex);
				Scr_ReturnValue.String = (str ? str : "<null>");
				break;
			}
			case VarType::Vector:
				Scr_ReturnValue.Vector = *var->Vector;
				break;
			case VarType::Float:
				Scr_ReturnValue.Float = var->Float;
				break;
			case VarType::Integer:
				Scr_ReturnValue.Integer = var->Integer;
				break;
			case VarType::Object:
				Scr_ReturnValue.Integer = var->Integer;
				break;
			case VarType::Function:
				Scr_ReturnValue.Integer = var->Integer;
				break;
			default:
				Scr_ReturnValue.Integer = var->Integer;
			}
		}
		catch (std::exception& e)
		{
			std::println("Scr_ExecThread_GscReturnValue exception: {}", e.what());
		}
	}
}