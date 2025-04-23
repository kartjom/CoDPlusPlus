#pragma once
#include <Utils/DevGui/SharedState.h>
#include <Engine/CoDUO.h>
#include "imgui.h"

namespace DevGui::Client
{
	inline void RenderTab()
	{
		using namespace CoDUO;

		if (ImGui::BeginTabItem("Client"))
		{
			static cvar_t* cg_fov = Cvar_Get("cg_fov", "80", CVAR_ARCHIVE | CVAR_CHEAT);

			if (ImGui::SliderFloat("Field of View", &cg_fov->value, 80.0f, 100.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp))
			{
				DevGuiState.FoV = cg_fov->value;
				Cvar_Set("cg_fov", va("%.0f", cg_fov->value), 1);
			}

			if (ImGui::Checkbox("Force Custom FoV", &DevGuiState.ForceFoV))
			{
				DevGuiState.FoV = cg_fov->value;
			}

			ImGui::EndTabItem();
		}
	}
}