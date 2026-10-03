internal void EntryPoint_Async_Enter(void *params)
{
    Unused(params);
    /// TODO: Setup LaneCTX and ...
    /// TODO: maybe handle platform specific requests and leave the app sepcific to EntryPoint_Async_Update which is implemented by the user
}

internal void EntryPoint_CallMainThreadEntryPoint(void)
{
    /// TODO: Setup LaneCTX and ...
    EntryPoint_Main_InitApplication();
    EntryPoint_Main_RunApplication();
}
