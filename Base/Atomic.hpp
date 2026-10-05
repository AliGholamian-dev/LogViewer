#ifndef BASE_ATOMIC_HPP
#define BASE_ATOMIC_HPP

template<typename T>
T Atomic_Eval(const volatile T* value);

template<typename T>
T Atomic_EvalAndAssign(volatile T* value, T newValue);

template<>
Bool8 Atomic_Eval<Bool8>(const volatile Bool8* value);

template<>
Bool8 Atomic_EvalAndAssign<Bool8>(volatile Bool8* value, Bool8 newValue);

#endif // BASE_ATOMIC_HPP
