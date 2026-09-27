#ifndef BASE_KEYWORDS_HPP
#define BASE_KEYWORDS_HPP

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

#if LANG_CPP
    #define C_LINKAGE_BEGIN extern "C"{
    #define C_LINKAGE_END }
    #define C_LINKAGE extern "C"
#else
    #define C_LINKAGE_BEGIN
    #define C_LINKAGE_END
    #define C_LINKAGE
#endif

template <typename... Ts>
constexpr void Unused(Ts&&...)
{
}

consteval void NoOp(void)
{
}

#endif // BASE_KEYWORDS_HPP
