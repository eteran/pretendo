
#ifndef COMPILER_H_
#define COMPILER_H_

#define EXPECT(expr, val)  __builtin_expect(expr, val)
#define LIKELY(expr)       EXPECT((expr), 1)
#define UNLIKELY(expr)     EXPECT((expr), 0)

#endif
