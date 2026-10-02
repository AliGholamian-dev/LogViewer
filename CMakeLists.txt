cmake_minimum_required(VERSION 3.10)
project(LogViewer LANGUAGES CXX)

add_executable(LogViewer WIN32 LogViewer/Win32_LogViewer.cpp)
set_target_properties(LogViewer PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_include_directories(LogViewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    target_compile_options(LogViewer PRIVATE /Wall /WX /wd4514 /wd4505 /wd4820)
    target_compile_definitions(LogViewer PRIVATE WIN32_LEAN_AND_MEAN UNICODE _UNICODE NOMINMAX)
    target_link_libraries(LogViewer PRIVATE Kernel32 KernelBase User32 Shell32)
endif()