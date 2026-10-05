#ifndef BASE_ATOMIMC_HPP
#define BASE_ATOMIMC_HPP

template <typename T>
concept AlwaysFalse = false;

template<typename T>
inline T Atomic_Eval(const T* atomicVar)
{
    return std::atomic_ref<const T>(*atomicVar).load(std::memory_order_seq_cst);
}

template<typename T>
inline T Atomic_EvalAndAssign(T* atomicVar, T newValue)
{
    return std::atomic_ref<T>(*atomicVar).exchange(newValue, std::memory_order_seq_cst);
}

#endif // BASE_ATOMIMC_HPP
