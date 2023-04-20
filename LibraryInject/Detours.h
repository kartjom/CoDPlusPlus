#pragma once
#include <wtypes.h>

#define DeclareDetour(fn) \
static DWORD fn##_Ret; \
static void fn();

#define ImplementDetour(fn) \
DWORD Detours::fn##_Ret; \
_declspec(naked) void Detours::fn()

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
	DeclareDetour(GScr_LoadGameTypeScript_h)
	DeclareDetour(ShootCallback_h)
};

