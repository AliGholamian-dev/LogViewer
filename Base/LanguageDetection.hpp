#ifndef BASE_LANGUAGE_DETECTION_HPP
#define BASE_LANGUAGE_DETECTION_HPP

////////////////////////////////
// Cpp
#if defined(__cplusplus)

    #define LANG_CPP 1

////////////////////////////////
// C
#else

    #define LANG_C 1

#endif

////////////////////////////////
// Zero All Undefined Options

#if !defined(LANG_CPP)
    #define LANG_CPP 0
#endif

#if !defined(LANG_C)
    #define LANG_C 0
#endif

#endif // BASE_LANGUAGE_DETECTION_HPP
