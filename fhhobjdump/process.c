#include <process.h>
#include <table.h>
#include <stdio.h>
char buf[150];
U64 operand_buf[3]; //MIPS32最多有3个参数。
U8 operand_count = 0;
U8 instruction_type = 0;
U64 process(U64 value)
{
    static U32 mask=0;
    if(value&OP_MASK) {         //I/J Commands
        mask = OP_MASK;     
    }
    else {                      //R Commands
        mask = FUNC_MASK;  
    }
    for(int i=0;i<(sizeof(instructions)/sizeof(U64));i++) {
        if((value&mask) == (instructions[i]&mask)) {
            instruction_type = (instructions[i]&0x300000000)>>32;
            operand_count = (instructions[i]&0xC00000000)>>34;
            if(instruction_type==1) { //R Type
                operand_buf[0] = ((value&0x03E00000)>>21);
                operand_buf[1] = ((value&0x001F0000)>>16);
                operand_buf[2] = ((value&0x0000F800)>>11);
            }

            if(instruction_type==2) { //I Type
                operand_buf[0] = ((value&0x03E00000)>>21);
                operand_buf[1] = ((value&0x001F0000)>>16);
                operand_buf[2] = ((value&0x0000FFFF));
            }

            if(instruction_type==3) { //J Type
                operand_buf[0] = ((value&0x03FFFFFF));
                operand_buf[1] = 0x00;
                operand_buf[2] = 0x00;
            }
            ins_sprintf(instruction_formats[i]);
            printf("0x%08x\t%s\n",(U32)value,buf);
            return ACCEPTED;
        } 

    }
    fprintf(stderr,"No valid instruction found.Stop.\n");
    return REJECTED;
}
/*
    按照给定格式进行字符串格式化。
    顺序按照从左到右。
    R：rs,rt,rd
    I：rs,rt,imm16
    J：imm26
    r:寄存器，i:立即数
    e.g.
    add %r3,%r1,%r2
    beq %r1,%r2,%i3
    jal %i1
*/

static U64 reg_name_sprintf(char* entry,U64 val) {
    U64 count=0;
    char* reg_name = registers[val];
    while(*reg_name) {
        *entry = *reg_name;
        entry++;
        reg_name++;
        count++;
    }
    return count;
}

static U64 immediate_sprintf(char* entry,U64 val) {
    U64 count=0;
    *entry = '0';
    entry++;
    count++;
    *entry = 'x';
    entry++;
    count++;
    U32 mask = 0xF0000000;
    U32 shift = 28;
    U32 has_something=0;
    for(int i=0;i<8;i++) {
        *entry = (val & mask)>>shift;
        has_something += *entry;
        if(has_something) {
            if(*entry<0xA) {
                *entry+=0x30;
            }
            else {
                *entry+=0x57;
            }
            entry++;
            count++;            
        }
        mask >>=4;
        shift -=4;
    }
    //如果到最后还没有数字，那么手动加个0.
    if(!has_something) {
        *entry = '0';
        entry++;
        count++;  
    }
    return count;
}

U64 ins_sprintf(char* format)
{
    char* format_pointer = format;
    char* buffer_pointer = buf;
    while(*format_pointer) {
        if(*format_pointer=='%') {
            //printf("Got parameter:%c type, no.%d\n",*(format_pointer+1),(*(format_pointer+2)-0x30));
            if(*(format_pointer+1)=='r') { //寄存器名称。
                buffer_pointer += reg_name_sprintf(buffer_pointer,operand_buf[(*(format_pointer+2)-0x31)]);
            }
            else {  //立即数。
                buffer_pointer += immediate_sprintf(buffer_pointer,operand_buf[(*(format_pointer+2)-0x31)]);
            }
            format_pointer += 3;
        }
        else {
            *buffer_pointer = *format_pointer; 
            format_pointer++;
            buffer_pointer++;
        }
    }
    *buffer_pointer = 0;
    return ACCEPTED;
}