#ifndef VARDEF_H
#define VARDEF_H
typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef long long I64;
typedef int I32;
typedef short I16;
typedef char I8;

typedef U8 byte;

#ifndef NULL
#define NULL 0
#endif
#define nullptr (void*) 0

//定义MIPS的三种指令格式。
typedef union {
    struct {
        U8 func:6;
        U8 sa:5;
        U8 rd:5;
        U8 rt:5;
        U8 rs:5;
        U8 op:6;
    } split;
    U32 raw;
} R_instruction;
typedef union {
    struct {
        U16 immediate;
        U8 rt:5;
        U8 rs:5;
        U8 op:6;
    } split;
    U32 raw;
} I_instruction;
typedef union {
    struct {
        U32 address:26;
        U8 op:6;
    } split;
    U32 raw;
} J_instruction;
//定义OP_MASK
#define OP_MASK 0xFC000000
//定义FUNC_MASK
#define FUNC_MASK 0x0000003F

#define ACCEPTED 0x8000000000000000
#define REJECTED 0xFFFFFFFFFFFFFFFF
#endif