#ifndef ENTRY_POINT_ENTRY_POINT_HPP
#define ENTRY_POINT_ENTRY_POINT_HPP

/// TODO: Parse arguments
/// TODO: Introduce async thread, which does some work then calls EntryPoint_Async_Enter
/// EntryPoint_Async_Enter will then call to EntryPoint_Async_Update which is implemented by user 
/// TODO: maybe run main alos with multiple threads, not needed now (One should be exactly this thread)
/// TODO: Wait for all threads (main and async to join)
internal void EntryPoint_Async_Enter(void *params);
internal void EntryPoint_Async_Update(void); /// TODO: Pass lane ctx? or params?
internal void EntryPoint_Main_InitApplication(int argc, char **argv); /// TODO: Parse command line and return it
internal void EntryPoint_Main_RunApplication(void); /// TODO: Get parsed command line struct as input
internal void EntryPoint_CallMainThreadEntryPoint(int argc, char **argv);

#endif // BASE_ENTRY_POINT_HPP
