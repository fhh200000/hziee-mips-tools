/*
    从汇编器抄来的代码。
    反汇编器不需要两遍扫描以及自动机分析，所以代码实现较为简单。
*/
#include <utility.h>
#include <stdlib.h>
#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_64(x) OSSwapInt64(x)
#elif defined(__sun) || defined(sun)
#include <sys/byteorder.h>
#define bswap_64(x) BSWAP_64(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
#define bswap_64(x) bswap64(x)
#elif defined(__OpenBSD__)
#include <sys/types.h>
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)
#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_64)
#define bswap_64(x) bswap64(x)
#endif
#else
#define bswap_64(x) __builtin_bswap64(x)

#endif
U64 fix_byte_order(U64 in)
{
    //如果定义了大端，则进行字节交换。
    //如果没定义，什么也不做。
    #ifdef BIG_ENDIAN
    return bswap_64(in);
    #else
    return in;
    #endif
}
