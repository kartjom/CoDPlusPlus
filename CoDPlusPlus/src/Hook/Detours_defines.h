#pragma once
#define DeclareDetour(fn) \
inline DWORD fn##_r = 0; \
void fn##_n() noexcept

#define DeclareType(callConv, retType, fn) typedef retType(callConv* fn##_t)

#define DeclareOverrideArg1(callConv, retType, fn, arg1) \
inline fn##_t fn##_o = nullptr; \
retType callConv fn##_w(arg1); \
DeclareDetour(fn)

#define DeclareOverrideArg2(callConv, retType, fn, arg1, arg2) \
inline fn##_t fn##_o = nullptr; \
retType callConv fn##_w(arg1, arg2); \
DeclareDetour(fn)

#define ImplementDetour(fn) _declspec(naked) void fn##_n() noexcept
#define ImplementOverride(retType, callConv, fn) retType callConv Detours::fn##_w

#define _restore _asm
#define JumpBack(name) _asm jmp [name##_r]
#define Original(fn) fn##_o
#define Wrapper(fn) fn##_w