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
U64 fix_byte_order(U64 in);
#endif