#ifndef BASE_KEYWORDS_HPP
#define BASE_KEYWORDS_HPP

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define internal      static
#define global        static 
#define local_persist static

#if COMPILER_MSVC
    #define thread_static __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
    #define thread_static __thread
#else
    #error [Keyword]: thread_static not defined for this compiler.
#endif

#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
    #pragma section(".rdata$", read)
    #define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
    #define read_only __attribute__((section(".rodata")))
#else
    #define read_only
#endif

#if LANG_CPP
    #define C_LINKAGE_BEGIN extern "C" {
    #define C_LINKAGE_END }
    #define C_LINKAGE extern "C"
#else
    #define C_LINKAGE_BEGIN
    #define C_LINKAGE_END
    #define C_LINKAGE
#endif

/// TODO: Move away from std library
#define StaticAssert(condition, message) static_assert(condition, message)
#define Assert(condition, message)       assert((void(message), condition))

#define InvalidCodePath() Assert(false, "InvalidCodePath")
#define NotImplemented()  Assert(false, "NotImplemented")

/// TODO: If needed add depth/ID
#define DeferLoop(begin, end) for(bool Glue(_DeferLoopCounter_, __LINE__) = ((begin), false); !Glue(_DeferLoopCounter_, __LINE__); Glue(_DeferLoopCounter_, __LINE__) = true, (end))

template <typename... Ts>
constexpr void Unused(Ts&&...)
{
}

consteval void NoOp(void)
{
}

#endif // BASE_KEYWORDS_HPP
