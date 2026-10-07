#ifndef BASE_KEYWORDS_HPP
#define BASE_KEYWORDS_HPP

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define internal      static
#define global        static 
#define local_persist static

#if LANG_CPP
    #define C_LINKAGE_BEGIN extern "C" {
    #define C_LINKAGE_END }
    #define C_LINKAGE extern "C"
#else
    #define C_LINKAGE_BEGIN
    #define C_LINKAGE_END
    #define C_LINKAGE
#endif

#define StaticAssert(condition, message) static_assert(condition, message)
#define Assert(condition, message)       assert((void(message), condition))

#define InvalidCodePath() Assert(false, "InvalidCodePath")
#define NotImplemented()  Assert(false, "NotImplemented")

template <typename... Ts>
internal constexpr void Unused(Ts&&...)
{
}

internal consteval void NoOp(void)
{
}

#define DeferLoop(begin, end) for(bool Glue(_DeferLoopCounter_, __LINE__) = ((begin), false); !Glue(_DeferLoopCounter_, __LINE__); Glue(_DeferLoopCounter_, __LINE__) = true, (end))

#endif // BASE_KEYWORDS_HPP
