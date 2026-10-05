#include <cstddef>
#include <cstdint>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <limits>
#include <ratio>
#include <utility>
#include <atomic>
#pragma warning(push, 0)
    #pragma warning(disable: 5039)
    #pragma warning(disable: 4865)
    #pragma warning(disable: 4514) 
    #pragma warning(disable: 4505)
    #pragma warning(disable: 5245)
    #pragma warning(disable: 4668)
    #include <Windows.h>
    #include <shellapi.h>
#pragma warning(pop)

#define NEED_ASYNC 1
#define NEED_MAIN_WINDOW 0

/// Base layer
#include "Base/ContextDetection.hpp"
#include "Base/Keywords.hpp"
#include "Base/BasicDataTypes.hpp"
#include "Base/Atomic.hpp"
#include "Base/Flags.hpp"
#include "Base/Algorithm.hpp"
#include "Base/Math.hpp"
#include "Base/SystemInfo.hpp"
#include "Base/Memory.hpp"
#include "Base/Arena.hpp"
#include "Base/Arena.cpp"
#include "Base/String.hpp"
#include "Base/String.cpp"
#include "Base/Time.hpp"
#include "Base/Thread.hpp"
#include "Base/ThreadContext.hpp"
#include "Base/ThreadContext.cpp"
#include "Base/Thread.cpp"

/// EntryPoint layer
#include "EntryPoint/EntryPoint.hpp"
#include "EntryPoint/EntryPoint.cpp"

/// Service layer
#include "Service/WindowManager/WindowManager.hpp"

/// Platform layer
#include "Platform/Win32/Win32_Base.hpp"
#include "Platform/Win32/Win32_Base.cpp"
#include "Platform/Win32/Win32_EntryPoint.cpp"
#include "Platform/Win32/Win32_WindowManager.cpp"

/// App layer
#include "GUIApp/GUIApp.hpp"
#include "GUIApp/GUIApp.cpp"

/// LogViewer layer
#include "LogViewer/LogViewer.cpp"