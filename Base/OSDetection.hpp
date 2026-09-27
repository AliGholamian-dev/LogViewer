#ifndef BASE_OS_DETECTION_HPP
#define BASE_OS_DETECTION_HPP

#if COMPILER_INTEL

    #error [OS]: This compiler/OS combo is not supported.

#elif COMPILER_CLANG

    #if defined(_WIN32)
        #define OS_WINDOWS 1
    #elif defined(__gnu_linux__) || defined(__linux__)
        #define OS_LINUX 1
    #elif defined(__APPLE__) && defined(__MACH__)
        #define OS_MAC 1
    #else
        #error [OS]: This compiler/OS combo is not supported.
    #endif

#elif COMPILER_MSVC

    #if defined(_WIN32)
        #define OS_WINDOWS 1
    #else
        #error [OS]: This compiler/OS combo is not supported.
    #endif

#elif COMPILER_GCC

    #if defined(__gnu_linux__) || defined(__linux__)
        #define OS_LINUX 1
    #else
       #error [OS]: This compiler/OS combo is not supported.
    #endif

#else

    #error [OS]: This compiler/OS combo is not supported.

#endif

////////////////////////////////
// Zero All Undefined Options

#if !defined(OS_WINDOWS)
    #define OS_WINDOWS 0
#endif

#if !defined(OS_LINUX)
    #define OS_LINUX 0
#endif

#if !defined(OS_MAC)
    #define OS_MAC 0
#endif

#endif // BASE_OS_DETECTION_HPP
