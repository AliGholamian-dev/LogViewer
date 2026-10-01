internal void EntryPoint_Async_Enter(void *params)
{
    Unused(params);
    /// TODO:
}

internal void EntryPoint_CallMainThreadEntryPoint(int argc, char **argv)
{
    EntryPoint_Main_InitApplication(argc, argv);
    EntryPoint_Main_RunApplication();
}
