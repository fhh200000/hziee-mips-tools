#ifndef UTILITY_H
#define UTILITY_H
#include "vardefs.h"
//MIPS可能会有大端CPU平台。
//在这里，我们通过X86 CPU的BSWAP指令交换数据。
//由于MIPS32的指令长度为固定的4字节，所以我们直接交换U32的字节序即可。
U32 fix_byte_order(U32 in);

//数据结构——顺序表的定义。
typedef struct {
    unsigned long long length;
    unsigned long long used;
    void** data;
} seq_list;
#define SEQ_LIST_DEFAULT_LENGTH 30
void seq_list_init(seq_list* list);
void seq_list_append(seq_list* list,void* data);
void seq_list_delete(seq_list* list);
#endif