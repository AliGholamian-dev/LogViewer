cmake_minimum_required(VERSION 3.10)
project(LogViewer LANGUAGES CXX)

add_executable(LogViewer WIN32 Platform/Win32/Win32_LogViewer.cpp)
set_target_properties(LogViewer PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_include_directories(LogViewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    target_compile_options(LogViewer PRIVATE /Wall /WX /wd4514 /wd4505 /wd4820 /wd5045)
    target_compile_definitions(LogViewer PRIVATE WIN32_LEAN_AND_MEAN UNICODE _UNICODE NOMINMAX)
    target_link_libraries(LogViewer PRIVATE Kernel32 User32 Shell32 Advapi32)
endif()