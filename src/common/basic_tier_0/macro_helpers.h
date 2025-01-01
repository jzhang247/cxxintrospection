#pragma once





#define CONCATE_(x,y) x##y
#define CONCATE(x,y) CONCATE_(x,y)

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)

#define STRINGTOW(x) CONCATE(L, x)
#define TOWSTRING(x) STRINGTOW(TOSTRING(x))

#if defined(_UNICODE)
#define STRINGTOT(x) x
#define TOTSTRING(x) STRINGTOT(TOSTRING(x))
#else
#define STRINGTOT(x) CONCATE(L,x)
#define TOTSTRING(x) STRINGTOT(TOSTRING(x))
#endif

// Macro overload
#pragma region
// Usage: #define {overloaded macro name}(...) MACRO_OVERLOAD_{n}(__VA_ARGS__, {macro taking n args},...,{macro taking 1 args})(__VA_ARGS__)
#define MACRO_OVERLOAD_2(a1, a2, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_3(a1, a2, a3, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_4(a1, a2, a3, a4, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_5(a1, a2, a3, a4, a5, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_6(a1, a2, a3, a4, a5, a6, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_7(a1, a2, a3, a4, a5, a6, a7, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_8(a1, a2, a3, a4, a5, a6, a7, a8, TargetMacro, ...) TargetMacro
#define MACRO_OVERLOAD_9(a1, a2, a3, a4, a5, a6, a7, a8, a9, TargetMacro, ...) TargetMacro
#pragma endregion



