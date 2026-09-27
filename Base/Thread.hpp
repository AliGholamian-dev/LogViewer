#ifndef BASE_THREAD_HPP
#define BASE_THREAD_HPP

using ThreadEntryPointFunctionType = void(void *params);

struct Thread 
{
    void* impl;
};

// enum class ThreadJoinResult
// {
//     TimedOut,
//     Joined
// };

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params);
internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds);
internal void Thread_Detach(Thread thread);

#endif // BASE_THREAD_HPP
