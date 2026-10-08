int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Unused(hInstance, hPrevInstance, pCmdLine, nCmdShow);
    Win32_InitPlatform();
    {
    //     EntryPoint_CallMainThreadEntryPoint(); /// TODO pass command line
    }
    Win32_DeInitPlatform();
    return 0;
}
