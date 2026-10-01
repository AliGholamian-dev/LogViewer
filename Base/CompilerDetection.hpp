#ifndef BASE_COMPILER_DETECTION_HPP
#define BASE_COMPILER_DETECTION_HPP

#if defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER)

   #define COMPILER_INTEL 1

#elif defined(__clang__)

   #define COMPILER_CLANG 1

#elif defined(_MSC_VER)

   #define COMPILER_MSVC 1

   #if _MSC_VER >= 1920
      #define COMPILER_MSVC_YEAR 2019
   #elif _MSC_VER >= 1910
      #define COMPILER_MSVC_YEAR 2017
   #elif _MSC_VER >= 1900
      #define COMPILER_MSVC_YEAR 2015
   #elif _MSC_VER >= 1800
      #define COMPILER_MSVC_YEAR 2013
   #elif _MSC_VER >= 1700
      #define COMPILER_MSVC_YEAR 2012
   #elif _MSC_VER >= 1600
      #define COMPILER_MSVC_YEAR 2010
   #elif _MSC_VER >= 1500
      #define COMPILER_MSVC_YEAR 2008
   #elif _MSC_VER >= 1400
      #define COMPILER_MSVC_YEAR 2005
   #else
      #define COMPILER_MSVC_YEAR 0
   #endif

#elif defined(__GNUC__) || defined(__GNUG__)

   #define COMPILER_GCC 1

#else

   #error [Compiler]: Compiler not supported.

#endif

#if !defined(COMPILER_INTEL)

   #define COMPILER_INTEL 0

#endif

#if !defined(COMPILER_CLANG)

   #define COMPILER_CLANG 0

#endif

#if !defined(COMPILER_MSVC)

   #define COMPILER_MSVC 0

#endif

#if !defined(COMPILER_GCC)

   #define COMPILER_GCC 0

#endif

#endif // BASE_COMPILER_DETECTION_HPP
