#ifndef BASE_ATOMIC_HPP
#define BASE_ATOMIC_HPP

template<typename T>
T Atomic_Evaluate(const volatile T* value);

template<typename T>
T Atomic_EvaluateAndAssign(volatile T* value, T newValue);

template<>
Bool8 Atomic_Evaluate<Bool8>(const volatile Bool8* value);

template<>
Bool8 Atomic_EvaluateAndAssign<Bool8>(volatile Bool8* value, Bool8 newValue);

template<>
Bool32 Atomic_Evaluate<Bool32>(const volatile Bool32* value);

template<>
Bool32 Atomic_EvaluateAndAssign<Bool32>(volatile Bool32* value, Bool32 newValue);

#endif // BASE_ATOMIC_HPP
