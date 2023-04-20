#include "Detours.h"
#include "ImGuiManager.h"

//#define ImplementOverrideArg1(retType, callingConv, name, arg1) \
//typedef retType(callingConv* name##_t)(arg1); \
//name##_t name##_o;
//
//#define ImplementOverrideArg2(retType, callingConv, name, arg1, arg2) \
//typedef retType(callingConv* name##_t)(arg1, arg2); \
//name##_t name##_o;
//
//ImplementOverrideArg2(BOOL, __stdcall, SetPhysicalCursorPos, int x, int y)
//
////typedef BOOL(__stdcall* SetPhysicalCursorPos_t)(int x, int y);
////SetPhysicalCursorPos_t SetPhysicalCursorPos_o;
//
//BOOL __stdcall SetPhysicalCursorPos_w(int x, int y)
//{
//	if (ImGuiManager::ShouldShow)
//	{
//		return false;
//	}
//
//	return SetPhysicalCursorPos_o(x, y);
//}
//
//ImplementDetour(SetPhysicalCursorPos_h)
//{
//	_asm
//	{
//		call dword ptr[SetPhysicalCursorPos_w]
//		jmp[SetPhysicalCursorPos_t_Ret]
//	}
//}