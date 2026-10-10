#ifndef BASE_LINKED_LIST_HPP
#define BASE_LINKED_LIST_HPP

/// TODO: Replace with templates

#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

/// singly-linked, singly-headed lists (stacks)
#define SLL_StackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLL_StackPop_N(f,next) ((f)=(f)->next)

/// singly-linked, singly-headed list helpers
#define SLL_StackPush(f,n) SLL_StackPush_N(f,n,next)
#define SLL_StackPop(f) SLL_StackPop_N(f,next)

/// doubly-linked-lists
#define DLL_Insert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
CheckNil(nil,p) ? \
((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
((p)==(l)) ? \
((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :\
(((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLL_PushBack_NPZ(nil,f,l,n,next,prev) DLL_Insert_NPZ(nil,f,l,l,n,next,prev)
#define DLL_PushFront_NPZ(nil,f,l,n,next,prev) DLL_Insert_NPZ(nil,l,f,f,n,prev,next)
#define DLL_Remove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
((n) == (l) ? (l) = (l)->prev : (0)),\
(CheckNil(nil,(n)->prev) ? (0) :\
((n)->prev->next = (n)->next)),\
(CheckNil(nil,(n)->next) ? (0) :\
((n)->next->prev = (n)->prev)))


/// doubly-linked-list helpers
#define DLL_Insert_NP(f,l,p,n,next,prev) DLL_Insert_NPZ(nullptr,f,l,p,n,next,prev)
#define DLL_PushBack_NP(f,l,n,next,prev) DLL_PushBack_NPZ(nullptr,f,l,n,next,prev)
#define DLL_PushFront_NP(f,l,n,next,prev) DLL_PushFront_NPZ(nullptr,f,l,n,next,prev)
#define DLL_Remove_NP(f,l,n,next,prev) DLL_Remove_NPZ(nullptr,f,l,n,next,prev)
#define DLL_Insert(f,l,p,n) DLL_Insert_NPZ(nullptr,f,l,p,n,next,prev)
#define DLL_PushBack(f,l,n) DLL_PushBack_NPZ(nullptr,f,l,n,next,prev)
#define DLL_PushFront(f,l,n) DLL_PushFront_NPZ(nullptr,f,l,n,next,prev)
#define DLL_Remove(f,l,n) DLL_Remove_NPZ(nullptr,f,l,n,next,prev)

#endif // BASE_LINKED_LIST_HPP
