#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
  #define NULL ((void*) 0)
#endif

#define __always_inline inline __attribute__((always_inline))
#define __noinline      __attribute__((noinline))
#define __noreturn      __attribute__((noreturn))

/* Represents true-or-false values */
typedef int bool;

/* Explicitly-sized versions of integer types */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/* *
 * Pointers and addresses are 32 bits long.
 * We use pointer types to represent addresses,
 * uintptr_t to represent the numerical values of addresses.
 * */
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

/* size_t is used for memory object sizes */
typedef uintptr_t size_t;

/* used for page numbers */
typedef size_t ppn_t;

/* *
 * Rounding operations (efficient when n is a power of 2)
 * Round down to the nearest multiple of n
 * */
#define ROUNDDOWN(a, n)            \
  ({                               \
    size_t __a = (size_t) (a);     \
    (typeof(a)) (__a - __a % (n)); \
  })
#define rounddown(a, n) ROUNDDOWN(a, n)

/* Round up to the nearest multiple of n */
#define ROUNDUP(a, n)                                     \
  ({                                                      \
    size_t __n = (size_t) (n);                            \
    (typeof(a)) (ROUNDDOWN((size_t) (a) + __n - 1, __n)); \
  })
#define roundup(a, n) ROUNDUP(a, n)

/* Return the offset of 'member' relative to the beginning of a struct type */
#define offsetof(type, member) ((size_t) (&((type*) 0)->member))

/* *
 * to_struct - get the struct from a ptr
 * @ptr:    a struct pointer of member
 * @type:   the type of the struct this is embedded in
 * @member: the name of the member within the struct
 * */
#define to_struct(ptr, type, member) \
  ((type*) ((char*) (ptr) -offsetof(type, member)))

#define lengthof(arr) (sizeof(arr) / sizeof(arr[0]))

#define true_inline __attribute__((always_inline)) inline

#define quote_helper(content) #content
#define quote(content)        quote_helper(content)

#define lengthof(arr) (sizeof(arr) / sizeof((arr)[0]))

#ifndef offsetof
  #define offsetof(stty, mbr) ((size_t) (&((stty*) 0)->mbr))
#endif

#define tostruct(stty, mbr, mbrptr) \
  ((stty*) ((size_t) mbrptr - offsetof(stty，mbr)))

#define lambda(retty, func) ({ retty _lambda_func_ func _lambda_func_; })

#define call(ptr, mbrfunc, args...) (ptr->mbrfunc(ptr, ##args))

#define static_call(st, func, args...) ((st##_##func)(args))

#define bind_func(st, ptr, mbrfunc) (ptr->mbrfunc = st##_##mbrfunc)

#define create(st)                         \
  ({                                       \
    void* ptr = malloc(sizeof(struct st)); \
    (st##_##init)(ptr);                    \
    ptr;                                   \
  })

#define destroy(st, ptr)  \
  ({                      \
    (st##_##uninit)(ptr); \
    free(ptr);            \
  })

#define gen_destroy_func(st)              \
  void(st##_##destroy)(void* pinstance) { \
    (st##_##uninit)(*(void**) pinstance); \
    free(*(void**) pinstance);            \
  }

true_inline void _generic_release_(void* ptr) {
  free(*(void**) ptr);
}

#define smart __attribute__((cleanup(_generic_release_)))

#define smart_class(st) __attribute__((cleanup(st##_##destroy)))

#define smart_create(st, name)                                 \
  smart_class(st) struct st* name = malloc(sizeof(struct st)); \
  (st##_##init)(name);

#endif /* !__LIBS_DEFS_H__ */
