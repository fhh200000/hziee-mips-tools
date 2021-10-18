/*
    从汇编器抄来的代码。
    反汇编器不需要两遍扫描以及自动机分析，所以代码实现较为简单。
*/
#ifndef UTILITY_H
#define UTILITY_H
#include "vardefs.h"
//MIPS可能会有大端CPU平台。
//在这里，我们通过X86 CPU的BSWAP指令交换数据。
//由于MIPS32的指令长度为固定的4字节，所以我们直接交换U32的字节序即可。
#include <stdlib.h>
#ifdef _MSC_VER
    #define bswap_64(x) _byteswap_uint64(x)
    #define bswap_32(x) _byteswap_ulong(x)
#elif defined(__APPLE__)
    // Mac OS X / Darwin features
    #include <libkern/OSByteOrder.h>
    #define bswap_64(x) OSSwapInt64(x)
    #define bswap_32(x) OSSwapInt32(x)
#elif defined(__sun) || defined(sun)
    #include <sys/byteorder.h>
    #define bswap_64(x) BSWAP_64(x)
    #define bswap_32(x) BSWAP_32(x)
#elif defined(__FreeBSD__)
#include <sys/endian.h>
    #define bswap_64(x) bswap64(x)
    #define bswap_32(x) bswap32(x)
#elif defined(__OpenBSD__)
    #include <sys/types.h>
    #define bswap_64(x) swap64(x)
    #define bswap_32(x) swap32(x)
#elif defined(__NetBSD__)
    #include <sys/types.h>
    #include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_64)
    #define bswap_64(x) bswap64(x)
    #define bswap_32(x) bswap32(x)
#endif
#else
    #define bswap_64(x) __builtin_bswap64(x)
    #define bswap_32(x) __builtin_bswap32(x)
#endif
#endif
