#ifdef CLIENT

#include "ImGuiManager.h"
#include "WinApiHelper.h"

#pragma comment(lib, "opengl32.lib")
//#include <gl/GL.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include <print>
#include <format>
#include <Structs/vec3_t.h>
#include <Utils/OpenGLHelper.h>
#include <Engine/CoDUO.h>
#include <Engine/ScriptLayer/Gsc/Async/Awaiter.h>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

using namespace OpenGLHelper;
using namespace CoDUO;
using namespace CoDUO::Gsc;
using namespace CoDUO::Gsc::Async;

namespace ImGuiManager
{
	void DevGuiMenu();
	void DrawServerEntities();
	void DrawServerEntityList();
}

namespace ImGuiManager
{
	void Initialize(HDC hDc)
	{
		if (WindowFromDC(hDc) == hWnd) return; // Nothing changed, everything initialized

		if (WindowFromDC(hDc) != hWnd && IsInitialized) // Window handle changed, need to re-init
		{
			std::println("[ImGui] - Window handle changed, destroying context");

			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}

		hWnd = WindowFromDC(hDc);
		LONG wLPTR = SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WinApiHelper::h_WndProc);

		if (!wLPTR) return;

		WinApiHelper::o_WndProc = (WNDPROC)wLPTR;
		wglContext = wglCreateContext(hDc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL3_Init("#version 460");

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.IniFilename = nullptr;

		IsInitialized = true;
		std::println("[ImGui] - Initialized");
	}

	HGLRC BeginFrame(HDC hDc)
	{
		HGLRC o_WglContext = wglGetCurrentContext();
		wglMakeCurrent(hDc, wglContext);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		return o_WglContext;
	}

	void EndFrame(HDC hDc, HGLRC o_WglContext)
	{
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		wglMakeCurrent(hDc, o_WglContext);
	}

	void Dispose()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void CursorToCenter()
	{
		int width, height;
		RECT window_rect;

		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);

		GetWindowRect(hWnd, &window_rect);
		if (window_rect.left < 0) {
			window_rect.left = 0;
		}
		if (window_rect.top < 0) {
			window_rect.top = 0;
		}
		if (window_rect.right >= width) {
			window_rect.right = width - 1;
		}
		if (window_rect.bottom >= height - 1) {
			window_rect.bottom = height - 1;
		}
		int window_center_x = (window_rect.right + window_rect.left) / 2;
		int window_center_y = (window_rect.top + window_rect.bottom) / 2;

		SetCursorPos(window_center_x, window_center_y);
	}

	bool Toggle()
	{
		return InteractiveMode ? Hide() : Show();
	}

	bool Show()
	{
		CursorToCenter();
		s_wmv->mouseInitialized = false;

		return InteractiveMode = true;
	}

	bool Hide()
	{
		CursorToCenter();
		s_wmv->mouseInitialized = true;

		return InteractiveMode = false;
	}

	void InteractiveTick()
	{
		DevGuiMenu();

		if (!InteractiveMode)
		{
			Hide();
		}
	}

	void Tick()
	{
		if (DevGuiState.force_fov)
		{
			static cvar_t* cg_fov = Cvar_Get("cg_fov", "80", CVAR_ARCHIVE | CVAR_CHEAT);

			if (DevGuiState.fov != cg_fov->value)
			{
				Cvar_Set("cg_fov", va("%.0f", DevGuiState.fov), 1);
			}
		}

		DrawServerEntities();
		DrawServerEntityList();
	}

	void DevGuiMenu()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_Once);

		if (!ImGui::Begin("CoDPlusPlus DevGui", &InteractiveMode))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginTabBar("##TabBar"))
		{
			if (ImGui::BeginTabItem("Client"))
			{
				static cvar_t* cg_fov = Cvar_Get("cg_fov", "80", CVAR_ARCHIVE | CVAR_CHEAT);

				if (ImGui::SliderFloat("Field of View", &cg_fov->value, 80.0f, 100.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp))
				{
					DevGuiState.fov = cg_fov->value;
					Cvar_Set("cg_fov", va("%.0f", cg_fov->value), 1);
				}

				if (ImGui::Checkbox("Force custom FoV", &DevGuiState.force_fov))
				{
					DevGuiState.fov = cg_fov->value;
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Utility"))
			{
				HWND console = GetConsoleWindow();

				DevGuiState.show_console = IsWindowVisible(console);
				ImGui::BeginDisabled(!console);
				if (ImGui::Checkbox("Show Console", &DevGuiState.show_console))
				{
					ShowWindow(console, DevGuiState.show_console);
				}
				ImGui::EndDisabled();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Rendering"))
			{
				if (uo_game_mp_x86)
				{
					ImGui::Checkbox("Draw entities on screen", &DevGuiState.draw_gentity);
					ImGui::Checkbox("Draw entity list", &DevGuiState.draw_gentity_window);
				}

				ImGui::EndTabItem();
			}

			if (uo_game_mp_x86 && ImGui::BeginTabItem("Server"))
			{
				if (ImGui::TreeNode("Registered Callbacks"))
				{
					ImGui::Spacing();

					ImGui::Text("Last return value");
					ImGui::SameLine();
					ImGui::SetCursorPosX(250);

					switch (Scr_ReturnValue.Type)
					{
					case VarType::Undefined:
						ImGui::Text("undefined");
						break;
					case VarType::String:
						ImGui::Text("string '%s'", Scr_ReturnValue.String.c_str());
						break;
					case VarType::Vector:
						ImGui::Text("vector ({:.2f} {:.2f} {:.2f})", Scr_ReturnValue.Vector.x, Scr_ReturnValue.Vector.y, Scr_ReturnValue.Vector.z);
						break;
					case VarType::Float:
						ImGui::Text("float %f", Scr_ReturnValue.Float);
						break;
					case VarType::Integer:
						ImGui::Text("int %d", Scr_ReturnValue.Integer);
						break;
					case VarType::Entity:
						ImGui::Text("entity %d", Scr_ReturnValue.Integer);
						break;
					case VarType::Function:
						ImGui::Text("function %d", Scr_ReturnValue.Integer);
						break;
					default:
						ImGui::Text("default %d", Scr_ReturnValue.Integer);
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
					DrawCallbackInfo(OnVoteCalled);
					DrawCallbackInfo(OnProjectileBounce);
					DrawCallbackInfo(OnProjectileExplode);

					ImGui::TreePop();
				}

				ImGui::Spacing();

				if (ImGui::TreeNode("Registered Console Commands"))
				{
					ImGui::Spacing();

					for (auto& [key, value] : gsc_commands)
					{
						ImGui::Text(key.c_str());
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text("%d", value);
					}

					ImGui::TreePop();
				}

				ImGui::Spacing();

				if (ImGui::TreeNode("Registered Client Commands"))
				{
					ImGui::Spacing();

					for (auto& [key, value] : gsc_clientcommands)
					{
						ImGui::Text(key.c_str());
						ImGui::SameLine();
						ImGui::SetCursorPosX(250);
						ImGui::Text("%d", value);
					}

					ImGui::TreePop();
				}

				ImGui::Spacing();

				if (ImGui::TreeNode("Active Async Tasks"))
				{
					ImGui::Spacing();

					{
						std::unique_lock<std::mutex> lock(TaskResultsMutex);

						for (auto& [key, value] : PendingTasks)
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

							switch (value->AwaitStatus.load())
							{
							case AwaiterStatus::New:
								ImGui::Text("New");
								break;
							case AwaiterStatus::InProgress:
								ImGui::Text("In progress");
								break;
							case AwaiterStatus::Finished:
								ImGui::Text("Finished");
								break;
							}
						}
					}

					ImGui::TreePop();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	void DrawServerEntities()
	{
		if (!uo_game_mp_x86 || !DevGuiState.draw_gentity) return;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(refdef->width, refdef->height));
		ImGui::Begin("DrawServerEntities", 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

		char ct_formatted[72];
		char origin_formatted[48];
		char list_entry[120];

		for (int i = 0; i <= GENTITY_COUNT; i++)
		{
			gentity_t* ent = &g_entities[i];
			if (!ent->classname) continue;

			const char* classname = SL_ConvertToString(ent->classname);
			if (!classname) continue;

			const char* targetname = nullptr;
			if (ent->targetname) targetname = SL_ConvertToString(ent->targetname);

			vec3_t screen;
			if (WorldToScreen(ent->currentOrigin, screen, refdef) && IsOnScreen(screen, refdef->width, refdef->height))
			{			
				*fmt::format_to(ct_formatted, "[{}] {} {}", i, classname, targetname ? targetname : "") = '\0';
				*fmt::format_to(origin_formatted, "{:.2f} {:.2f} {:.2f}", ent->currentOrigin.x, ent->currentOrigin.y, ent->currentOrigin.z) = '\0';

				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y), ImColor(1.0f, 1.0f, 1.0f, 1.0f), ct_formatted);
				ImGui::GetWindowDrawList()->AddText(ImVec2(screen.x, screen.y + 16), ImColor(0.75f, 0.75f, 0.75f, 1.0f), origin_formatted);
			}
		}

		ImGui::End();
	}

	void DrawServerEntityList()
	{
		if (!uo_game_mp_x86 || !DevGuiState.draw_gentity_window) return;

		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchSame;

		ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(600, 350), ImGuiCond_Once);
		if (ImGui::Begin("Entity List", &DevGuiState.draw_gentity_window))
		{
			if (ImGui::BeginTable("table_scrolly", 4, flags))
			{
				ImGui::TableSetupScrollFreeze(0, 1);
				ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 40);
				ImGui::TableSetupColumn("Classname", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Targetname", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Origin", ImGuiTableColumnFlags_WidthFixed, 200);
				ImGui::TableHeadersRow();

				// Populate table rows
				for (int i = 0; i <= GENTITY_COUNT; i++)
				{
					gentity_t* ent = &g_entities[i];
					if (!ent->classname) continue;

					const char* classname = SL_ConvertToString(ent->classname);
					if (!classname) continue;

					const char* targetname = nullptr;
					if (ent->targetname) targetname = SL_ConvertToString(ent->targetname);

					// Add row to the table
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", i);

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(classname);
					
					ImGui::TableSetColumnIndex(2);
					ImGui::Text(targetname ? "%s" : "", targetname);

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%.2f %.2f %.2f", ent->currentOrigin.x, ent->currentOrigin.y, ent->currentOrigin.z);
				}

				ImGui::EndTable();
			}
		}
		ImGui::End(); // End "Entity List" window
	}
}

#endif