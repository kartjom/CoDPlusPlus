#include <Hook/Detours.h>
#include <Engine/CoDUO.h>
#include <print>
#include <fstream>
#include <filesystem>

using namespace CoDUO;
using namespace CoDUO::Gsc;

namespace Hook::Detour
{
	void Scr_ExecThread_GscReturnValue();
	void EvaluateMapBindings();
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

	_declspec(naked) void MapBindings_n() noexcept
	{
		_asm pushad
		EvaluateMapBindings();
		_asm popad

		_asm // restore
		{
			mov esi, dword ptr ds : [0x9677C0]
			test esi, esi
		}

		_asm jmp[MapBindingsHook.Return] // jump back
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
			case VarType::Entity:
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

	void __cdecl EvaluateMapBindings()
	{
		constexpr const char* map_bindings = "codplusplus/map_bindings.txt";

		char* mapname = Cmd_Argv[1];
		if (mapname == nullptr || *mapname == '\0' || !std::filesystem::exists(map_bindings)) return;

		std::ifstream file(map_bindings, std::ifstream::in);
		if (!file.is_open()) return;

		std::string _default;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string key, value;
			if (std::getline(iss, key, '=') && std::getline(iss, value))
			{
				if (_default.empty() && _stricmp(key.c_str(), "default") == 0)
				{
					_default = value;
					continue;
				}

				if (_stricmp(key.c_str(), mapname) == 0)
				{
					Cvar_Set("fs_game", value.c_str(), 1);

					file.close();
					return;
				}
			}
		}
		file.close();

		if (!_default.empty())
		{
			Cvar_Set("fs_game", _default.c_str(), 1);
		}
	}
}