#pragma once
#include <Utils/DevGui/SharedState.h>
#include <Engine/CoDUO.h>
#include "imgui.h"

#include <Engine/MapBindings/MapBindings.h>
#include <Engine/Async/Task/Task.h>

namespace DevGui::Server::Helper
{
	inline void RegisteredCallbacksTab();
	inline void RegisteredCommandsTab();
	inline void ActiveMapBindingsTab();
	inline void ActiveAsyncTasksTab();
}

namespace DevGui::Server
{
	inline void RenderTab()
	{
		using namespace CoDUO;

		ImGui::BeginDisabled(!uo_game_mp_x86);
		if (ImGui::BeginTabItem("Server"))
		{
			DevGui::Server::Helper::RegisteredCallbacksTab();
			ImGui::Spacing();

			DevGui::Server::Helper::RegisteredCommandsTab();
			ImGui::Spacing();

			DevGui::Server::Helper::ActiveMapBindingsTab();
			ImGui::Spacing();

			DevGui::Server::Helper::ActiveAsyncTasksTab();

			ImGui::EndTabItem();
		}
		ImGui::EndDisabled();
	}
}

namespace DevGui::Server::Helper
{
	void RegisteredCallbacksTab()
	{
		using namespace CoDUO;
		using namespace CoDUO::Gsc;

		if (ImGui::TreeNode("Registered Callbacks"))
		{
			ImGui::Spacing();

			ImGui::Text("Last return value");
			ImGui::SameLine();
			ImGui::SetCursorPosX(250);

			ScrVar& scr_var = Scr_InternalReturnValue;
			const char* typeName = scr_var.GetTypeName();

			switch (scr_var.GetType())
			{
			case VarType::Undefined:
				ImGui::Text(typeName);
				break;
			case VarType::String:
			case VarType::LocalizedString:
				ImGui::Text("%s '%s'", typeName, scr_var.GetStringRef().c_str());
				break;
			case VarType::Vector:
			{
				const vec3_t& vec = scr_var.GetVectorRef();
				ImGui::Text("%s (%.2f %.2f %.2f)", typeName, vec.x, vec.y, vec.z);
				break;
			}
			case VarType::Float:
				ImGui::Text("%s %.2f", typeName, scr_var.GetNumber<float>());
				break;
			case VarType::Integer:
				ImGui::Text("%s %d", typeName, scr_var.GetNumber<int>());
				break;
			case VarType::Entity:
			{
				gentity_t* ent = scr_var.GetEntity();
				const char* name = nullptr;
				if (ent) {
					name = ent->client ? ent->client->name : SL_ConvertToString(ent->classname);
				}

				ImGui::Text("%s %s", typeName, name);
				break;
			}
			default:
				ImGui::Text("%s %d", typeName, scr_var.GetNumber<int>());
			}

			ImGui::Spacing();

			auto active = ImVec4(1, 1, 1, 1);
			auto inactive = ImVec4(0.7f, 0.7f, 0.7f, 1);

			#define DrawCallbackInfo(name) ImGui::TextColored(CodeCallback.##name ? active : inactive, #name, CodeCallback.##name); \
			ImGui::SameLine(); \
			ImGui::SetCursorPosX(250); \
			ImGui::TextColored(CodeCallback.##name ? active : inactive, "%d", CodeCallback.##name)

			DrawCallbackInfo(OnInitialize);
			DrawCallbackInfo(OnPlayerShoot);
			DrawCallbackInfo(OnPlayerMelee);
			DrawCallbackInfo(OnPlayerSay);
			DrawCallbackInfo(OnPlayerVote);
			DrawCallbackInfo(OnPlayerInactivity);
			DrawCallbackInfo(OnPlayerVoteCalled);
			DrawCallbackInfo(OnProjectileBounce);
			DrawCallbackInfo(OnProjectileExplode);

			ImGui::TreePop();
		}
	}

	void RegisteredCommandsTab()
	{
		using namespace CoDUO::Gsc;

		if (ImGui::TreeNode("Registered Commands"))
		{
			if ( !gsc_commands.empty() )
			{
				ImGui::Spacing();
				for (auto& [key, value] : gsc_commands)
				{
					ImGui::Text(key.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(250);
					ImGui::Text("%d", value);
				}
			}

			if ( !gsc_clientcommands.empty() )
			{
				ImGui::Spacing();
				for (auto& [key, value] : gsc_clientcommands)
				{
					ImGui::Text(key.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(250);
					ImGui::Text("%d", value);
				}
			}

			ImGui::TreePop();
		}
	}

	void ActiveMapBindingsTab()
	{
		using namespace MapBindings;

		if (ImGui::TreeNode("Active Map Bindings"))
		{
			ImGui::Spacing();

			MapBindingEntry& mapBinding = MapBindings::GetBindings();
			for (auto& [key, value] : mapBinding.Bindings)
			{
				ImGui::Text(key.c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(250);
				ImGui::Text(value.c_str());
			}

			if (!mapBinding.Default.empty())
			{
				ImGui::Text("default");
				ImGui::SameLine();
				ImGui::SetCursorPosX(250);
				ImGui::Text(mapBinding.Default.c_str());
			}

			ImGui::TreePop();
		}
	}

	void ActiveAsyncTasksTab()
	{
		using namespace CoDUO::Gsc::Async;

		if (ImGui::TreeNode("Active Async Tasks"))
		{
			ImGui::Spacing();

			{
				std::unique_lock<std::mutex> lock(TaskListMutex);

				for (auto& [key, value] : AllocatedTasks)
				{
					switch (value->Type.load())
					{
					case TaskType::Http:
						ImGui::Text("[%d] Http", key);
						break;
					default:
						ImGui::Text("[%d] Task", key);
					}

					ImGui::SameLine();
					ImGui::SetCursorPosX(250);

					switch (value->Status.load())
					{
					case TaskStatus::New:
						ImGui::Text("New");
						break;
					case TaskStatus::InProgress:
						ImGui::Text("In progress");
						break;
					case TaskStatus::Finished:
						ImGui::Text("Finished");
						break;
					}
				}
			}

			ImGui::TreePop();
		}
	}
}