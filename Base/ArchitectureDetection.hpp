#ifndef BASE_ARCHITECTURE_DETECTION_HPP
#define BASE_ARCHITECTURE_DETECTION_HPP

#if COMPILER_INTEL

    #error [Architecture]: Architecture not supported.

#elif COMPILER_CLANG

    #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
        #define ARCH_X64 1
    #elif defined(i386) || defined(__i386) || defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__aarch64__)
        #define ARCH_ARM64 1
    #elif defined(__arm__)
        #define ARCH_ARM32 1
    #else
        #error [Architecture]: Architecture not supported.
    #endif

#elif COMPILER_MSVC

    #if defined(_M_AMD64)
        #define ARCH_X64 1
    #elif defined(_M_IX86)
        #define ARCH_X86 1
    #elif defined(_M_ARM64)
        #define ARCH_ARM64 1
    #elif defined(_M_ARM)
        #define ARCH_ARM32 1
    #else
        #error [Architecture]: Architecture not supported.
    #endif

#elif COMPILER_GCC

    #if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
        #define ARCH_X64 1
    #elif defined(i386) || defined(__i386) || defined(__i386__)
        #define ARCH_X86 1
    #elif defined(__aarch64__)
        #define ARCH_ARM64 1
    #elif defined(__arm__)
        #define ARCH_ARM32 1
    #else
        #error [Architecture]: Architecture not supported.
    #endif

#else

    #error [Architecture]: Architecture not supported.

#endif

#if defined(ARCH_X64) || defined(ARCH_ARM64)

    #define ARCH_64BIT 1

#elif defined(ARCH_X86) || defined(ARCH_ARM32)

    #define ARCH_32BIT 1

#endif

#if ARCH_ARM32 || ARCH_ARM64 || ARCH_X64 || ARCH_X86

    #define ARCH_LITTLE_ENDIAN 1

#else

    #error [Endianness]: Endianness of this architecture not understood.

#endif

////////////////////////////////
// Zero All Undefined Options

#if !defined(ARCH_32BIT)
    #define ARCH_32BIT 0
#endif

#if !defined(ARCH_64BIT)
    #define ARCH_64BIT 0
#endif

#if !defined(ARCH_X64)
    #define ARCH_X64 0
#endif

#if !defined(ARCH_X86)
    #define ARCH_X86 0
#endif

#if !defined(ARCH_ARM64)
    #define ARCH_ARM64 0
#endif

#if !defined(ARCH_ARM32)
    #define ARCH_ARM32 0
#endif

#if !defined(ARCH_LITTLE_ENDIAN)
    #define ARCH_LITTLE_ENDIAN 0
#endif

#endif // BASE_ARCHITECTURE_DETECTION_HPP
