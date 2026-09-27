#include <concepts>
#include <type_traits>
#include <ratio>
#include <cstring>
#include <limits>
#include <Windows.h>
#include <objbase.h>
#include <shellapi.h>

/// TODO: order and break (or even a preprocessor) to put the files and functions in correct order
/// So that function is defined before the first use

#include "Base/CompilerDetection.hpp"
#include "Base/LanguageDetection.hpp"
#include "Base/ArchitectureDetection.hpp"
#include "Base/OSDetection.hpp"
#include "Base/BasicDataTypes.hpp"
#include "Base/Keywords.hpp"
#include "Base/Flags.hpp"
#include "Base/Algorithm.hpp"
#include "Base/SystemInfo.hpp"
#include "Base/Memory.hpp"
#include "Base/Arena.hpp"
#include "Base/Arena.cpp"
#include "Base/Time.hpp"
/// TODO: Time platform port, when set and get is added
#include "Base/String.hpp"
#include "Base/String.cpp"
#include "Base/Thread.hpp"
#include "Base/ThreadContext.hpp"
#include "Base/ThreadContext.cpp"
#include "Base/EntryPoint.hpp"
#include "Base/EntryPoint.cpp"
#include "PlatformPort/Base/Win32_Platform.hpp"
#include "PlatformPort/Base/Win32_Platform.cpp"

#include "WindowManager/WindowManager.hpp"
#include "WindowManager/WindowManager.cpp"
#include "PlatformPort/WindowManager/Win32_WindowManager.hpp"
#include "PlatformPort/WindowManager/Win32_WindowManager.cpp"

internal void EntryPoint_EnterAsyncMain(void)
{

}

/// TODO: Get rid of CRT (Watch handmade hero example)
internal void EntryPoint_EnterApplicationMain(void)
{
    WindowManager_Init();
    {
        WindowManager_OpenWindow();
        MSG msg;
        BOOL bRet;
        while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
        { 
            if (bRet == -1)
            {
                // handle the error and possibly exit
            }
            else
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
        } 
    }
    WindowManager_DeInit();
}
