#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
/*
    项目整合：fhhobjdump。
    把二进制代码翻译为汇编代码。
*/
#include "vardefs.h"
#include "disassembler_watcher.h"

class Disassembler
{
public:
    Disassembler(DisassemblerWatcher* watcher);
    void disassemble(U32 in);
private:
    DisassemblerWatcher* watcher;
    char buff[150];
    U64 operand_buf[3]; //MIPS32最多有3个参数。
    U8 operand_count=0;
    U8 instruction_type=0;
    U64 process(U64 value);
    U64 ins_sprintf(const char* format);
};

#endif // DISASSEMBLER_H
