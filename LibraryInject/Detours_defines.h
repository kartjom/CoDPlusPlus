#pragma once
#define DeclareDetour(fn) \
static DWORD fn##_Ret; \
static void fn##_t() noexcept

#define DeclareTypeArg1(retType, callConv, fn, arg1) typedef retType(callConv* fn##_t)(arg1)
#define DeclareTypeArg2(retType, callConv, fn, arg1, arg2) typedef retType(callConv* fn##_t)(arg1, arg2)

#define DeclareOverrideArg1(retType, callConv, fn, arg1) \
static fn##_t fn##_o; \
static retType callConv fn##_w(arg1); \
DeclareDetour(fn)

#define DeclareOverrideArg2(retType, callConv, fn, arg1, arg2) \
static fn##_t fn##_o; \
static retType callConv fn##_w(arg1, arg2); \
DeclareDetour(fn)

#define ImplementDetour(fn) \
DWORD Detours::fn##_Ret; \
_declspec(naked) void Detours::fn##_t() noexcept

#define ImplementOverride(retType, callConv, fn) \
fn##_t Detours::fn##_o = nullptr; \
retType callConv Detours::fn##_w

#define _restore _asm
#define JumpBack(name) _asm jmp [name##_Ret]
#define Original(fn) fn##_o
#define Wrapper(fn) fn##_w