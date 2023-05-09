#include "OpenGLHelper.h"
#include "Hook.h"
#include "Detours.h"

namespace OpenGLHelper
{
	void InjectDetours()
	{
		Detours::wglSwapBuffers_o = Hook::LoadFromDLL<wglSwapBuffers_t>("opengl32.dll", "wglSwapBuffers");
		DetourRet(Hook::BaseAddress + 0xF6723, Detours::wglSwapBuffers, 6);
	}
}