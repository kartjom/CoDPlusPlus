#pragma once
#define DeclareDetour(fn) \
static DWORD fn##_r; \
static void fn##_n() noexcept

#define DeclareType(callConv, retType, fn) typedef retType(callConv* fn##_t)

#define DeclareOverrideArg1(callConv, retType, fn, arg1) \
static fn##_t fn##_o; \
static retType callConv fn##_w(arg1); \
DeclareDetour(fn)

#define DeclareOverrideArg2(callConv, retType, fn, arg1, arg2) \
static fn##_t fn##_o; \
static retType callConv fn##_w(arg1, arg2); \
DeclareDetour(fn)

#define ImplementDetour(fn) \
DWORD Detours::fn##_r; \
_declspec(naked) void Detours::fn##_n() noexcept

#define ImplementOverride(retType, callConv, fn) \
fn##_t Detours::fn##_o = nullptr; \
retType callConv Detours::fn##_w

#define _restore _asm
#define JumpBack(name) _asm jmp [name##_r]
#define Original(fn) fn##_o
#define Wrapper(fn) fn##_w