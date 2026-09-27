cmake_minimum_required(VERSION 3.10)
project(LogViewer LANGUAGES CXX)

add_executable(LogViewer WIN32 PlatformPort/Main/Win32_Main.cpp)
set_target_properties(LogViewer PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF EXPORT_COMPILE_COMMANDS ON)
target_include_directories(LogViewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    target_compile_definitions(LogViewer PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
    target_link_libraries(LogViewer PRIVATE ole32 user32 gdi32 shell32)
endif()