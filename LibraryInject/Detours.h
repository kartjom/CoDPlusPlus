#pragma once
#include <wtypes.h>

#define DeclareDetour(fn) \
static DWORD fn##_Ret; \
static void fn##_t()

#define ImplementDetour(fn) \
DWORD Detours::fn##_Ret; \
_declspec(naked) void Detours::fn##_t()

#define SAFE_CALL(code) \
_asm {pushad} \
{ \
code \
} \
_asm {popad}

#define _restore _asm
#define JumpBack(name) _asm jmp [name##_Ret]

class Detours
{
public:
	DeclareDetour(GScr_LoadGameTypeScript);
	DeclareDetour(ShootCallback);
};

