
#ifndef COMPILER_H_
#define COMPILER_H_

#if defined(__GNUC__)
	#define EXPECT(expr, val)  __builtin_expect(expr, val)
	#define LIKELY(expr)       EXPECT((expr), 1)
	#define UNLIKELY(expr)     EXPECT((expr), 0)
#else
	#define EXPECT(expr, val)  (expr)
	#define LIKELY(expr)       (expr)
	#define UNLIKELY(expr)     (expr)
#endif

#endif
