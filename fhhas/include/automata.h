#ifndef AUTOMATA_H
#define AUTOMATA_H
//由于MIPS的结构比较奇怪，所以可能需要U64来存储状态了…………
#include "vardefs.h"
typedef U64 status;
#define ACCEPTED 0x8000000000000000
#define REJECTED 0xFFFFFFFFFFFFFFFF
typedef struct {
    U64 curr;
    U8 input;
    U64 next;
} transrule;

status naive_dfa(transrule* instruct,int rulelength,int inputlength,char* in);
status token_store(int type,status result,char* pointer,int length);
U64 process_pass_1(char* in,int length);
U64 process_pass_2(char* in,int length);
extern U64 (*process)(char* in,int length);
#endif