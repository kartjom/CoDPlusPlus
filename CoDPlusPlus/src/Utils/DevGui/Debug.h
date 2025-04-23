#pragma once
#include <Utils/DevGui/SharedState.h>
#include <Engine/CoDUO.h>
#include "imgui.h"

#include <Utils/OpenGLHelper.h>

namespace DevGui::Debug::Helper
{
	inline void ShowEntitiesOnScreenAction();
	inline void ShowEntityListAction();
}

namespace DevGui::Debug
{
	inline void RenderTab()
	{
		using namespace CoDUO;

		if (ImGui::BeginTabItem("Debug"))
		{
			// Rendering
			ImGui::BeginDisabled(!uo_game_mp_x86);
				ImGui::Checkbox("Show Entities On Screen", &DevGuiState.ShowEntitiesOnScreen);
				ImGui::Checkbox("Show Entity List", &DevGuiState.ShowEntityList);
			ImGui::EndDisabled();

			// Show Console
			HWND console = GetConsoleWindow();
			DevGuiState.ShowConsole = IsWindowVisible(console);

			ImGui::BeginDisabled(!console);
			if (ImGui::Checkbox("Show Console", &DevGuiState.ShowConsole))
			{
				ShowWindow(console, DevGuiState.ShowConsole);
			}
			ImGui::EndDisabled();

			ImGui::EndTabItem();
		}
	}

	inline void RenderActions()
	{
		if (DevGuiState.ShowEntitiesOnScreen)
			DevGui::Debug::Helper::ShowEntitiesOnScreenAction();

		if (DevGuiState.ShowEntityList)
			DevGui::Debug::Helper::ShowEntityListAction();
	}

	inline void ForceFoV()
	{
		using namespace CoDUO;

		if (!DevGuiState.ForceFoV) return;

		static cvar_t* cg_fov = Cvar_Get("cg_fov", "80", CVAR_ARCHIVE | CVAR_CHEAT);
		if (DevGuiState.FoV != cg_fov->value)
		{
			Cvar_Set("cg_fov", va("%.0f", DevGuiState.FoV), 1);
		}
	}
}

namespace DevGui::Debug::Helper
{
	void ShowEntitiesOnScreenAction()
	{
		using namespace OpenGLHelper;
		using namespace CoDUO;

		if (!uo_game_mp_x86) return;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(refdef.width, refdef.height));
		ImGui::Begin("DrawServerEntities", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

		constexpr int CT_SIZE = 128;
		constexpr int ORIGIN_SIZE = 64;

		char ct_formatted[CT_SIZE];
		char origin_formatted[ORIGIN_SIZE];

		for (int i = 0; i <= GENTITY_COUNT; i++)
		{
			gentity_t* ent = &g_entities[i];
			if (!ent->classname) continue;

			const char* classname = SL_ConvertToString(ent->classname);
			if (!classname) continue;

			const char* targetname = nullptr;
			if (ent->targetname) targetname = SL_ConvertToString(ent->targetname);

			vec3_t screen;
			if (WorldToScreen(ent->currentOrigin, screen, refdef) && IsOnScreen(screen, refdef.width, refdef.height))
			{
				snprintf(ct_formatted, CT_SIZE, "[%d] %s %s", i, classname, targetname ? targetname : "");
				snprintf(origin_formatted, ORIGIN_SIZE, "%.2f %.2f %.2f", ent->currentOrigin.x, ent->currentOrigin.y, ent->currentOrigin.z);

				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y), ImColor(1.0f, 1.0f, 1.0f, 1.0f), ct_formatted);
				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y + 16), ImColor(0.75f, 0.75f, 0.75f, 1.0f), origin_formatted);
			}
		}

		ImGui::End();
	}

	void ShowEntityListAction()
	{
		using namespace CoDUO;

		if (!uo_game_mp_x86) return;

		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame;
		static int selected = -1;

		ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Once);
		if (ImGui::Begin("Entity List", &DevGuiState.ShowEntityList))
		{
			if (ImGui::BeginTable("table_scrolly", 5, flags))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40);
				ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 130);
				ImGui::TableSetupColumn("Classname", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Targetname", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, 200);
				ImGui::TableHeadersRow();

				// Populate table rows
				for (int i = 0; i <= GENTITY_COUNT; i++)
				{
					gentity_t* ent = &g_entities[i];

					if (i == selected && !ent->inuse) selected = -1;

					const char* classname = "";
					if (ent->inuse && ent->classname) classname = SL_ConvertToString(ent->classname);

					const char* targetname = nullptr;
					if (ent->inuse && ent->targetname) targetname = SL_ConvertToString(ent->targetname);

					// Add row to the table
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::BeginDisabled(!ent->inuse);
					if (ImGui::Selectable(va("%d", i), i == selected, ImGuiSelectableFlags_SpanAllColumns))
					{
						selected = i;
					}

					if (!ent->inuse)
					{
						ImGui::EndDisabled();
						continue;
					}

					if (ent->inuse && ImGui::BeginPopupContextItem())
					{
						selected = i;

						if (ImGui::Button("Go to"))
						{
							gentity_t* host = &g_entities[0];
							G_SetOrigin(host, ent->currentOrigin);

							ImGui::CloseCurrentPopup();
						}

						if (ImGui::Button("Bring"))
						{
							gentity_t* host = &g_entities[0];
							G_SetOrigin(ent, host->currentOrigin);

							ImGui::CloseCurrentPopup();
						}

						if (ImGui::Button("Delete"))
						{
							G_DeleteEntity(ent);

							ImGui::CloseCurrentPopup();
						}

						ImGui::EndPopup();
					}

					ImGui::TableNextColumn();
					ImGui::TextUnformatted(G_EntityTypeString(ent->eType));

					ImGui::TableNextColumn();
					if (ent->eType == ET_PLAYER)
					{
						ImGui::Text("%s '%s'", classname, ent->client->name);
					}
					else
					{
						ImGui::TextUnformatted(classname);
					}

					ImGui::TableNextColumn();
					ImGui::Text(targetname ? "%s" : "", targetname);

					ImGui::TableNextColumn();
					ImGui::Text("%.2f %.2f %.2f", ent->currentOrigin.x, ent->currentOrigin.y, ent->currentOrigin.z);
					ImGui::EndDisabled();
				}

				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}