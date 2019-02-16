
#ifndef COMPILER_H_
#define COMPILER_H_

#if defined(__GNUC__)
    #define LIKELY(expr)       __builtin_expect(!!(expr), 1)
    #define UNLIKELY(expr)     __builtin_expect(!!(expr), 0)
#else
	#define LIKELY(expr)       (expr)
	#define UNLIKELY(expr)     (expr)
#endif

#endif
