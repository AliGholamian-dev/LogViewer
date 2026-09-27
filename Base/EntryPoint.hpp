#ifndef BASE_ENTRY_POINT_HPP
#define BASE_ENTRY_POINT_HPP

internal int EntryPoint_MainThread(void);
internal void EntryPoint_SupplementThread(ThreadEntryPointFunctionType *entryPointFunction, void *params);
internal void EntryPoint_EnterApplicationMain(void);
internal void EntryPoint_EnterAsyncMain(void);

#endif // BASE_ENTRY_POINT_HPP
