#ifndef PLATFORM_PORT_BASE_WIN32_PLATFORM_HPP
#define PLATFORM_PORT_BASE_WIN32_PLATFORM_HPP

internal void Win32_InitPlatform(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);
internal void Win32_DeInitPlatform(void);

/// TODO: Maybe init all platform functions in one file not to expose this, or have a base plarform data which is shared between modules  
internal HINSTANCE Win32_GetModule(void);

#endif // PLATFORM_PORT_BASE_WIN32_PLATFORM_HPP
