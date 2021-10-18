/*
    从汇编器抄来的代码。
    反汇编器不需要两遍扫描以及自动机分析，所以代码实现较为简单。
*/
#ifndef TABLE_H
#define TABLE_H
#include "vardefs.h"

//由于MIPS的指令集比较特殊（定长指令），此处采用掩码较为简单。
//以下给出了不同指令的掩码。

#define NOP_MASK 0x0000000000000000
#define R_INST_MASK 0x0000000100000000
#define I_INST_MASK 0x0000000200000000
#define J_INST_MASK 0x0000000300000000

#define PARAMETER_0 0x0000000000000000
#define PARAMETER_1 0x0000000400000000
#define PARAMETER_2 0x0000000800000000
#define PARAMETER_3 0x0000000C00000000

//有些指令的rs和rt反向。Make them happy.
//同时，rw和sw的神奇的imm16(reg)方式也可以用这个标记make them happy.
#define REVERSE_OP  0x0000001000000000

//跳转指令分为相对跳转和绝对跳转。
#define JUMP_RELATIVE 0x0000002000000000
#define JUMP_ABSOLUTE 0x0000004000000000

#define SLLV_MASK    (NOP_MASK|0b000100|R_INST_MASK|PARAMETER_3|ACCEPTED|REVERSE_OP)
#define  JR_MASK     (NOP_MASK|0b001000|R_INST_MASK|PARAMETER_1|ACCEPTED)
#define ADD_MASK     (NOP_MASK|0b100000|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SUB_MASK     (NOP_MASK|0b100010|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define AND_MASK     (NOP_MASK|0b100100|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define  OR_MASK     (NOP_MASK|0b100101|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define XOR_MASK     (NOP_MASK|0b100110|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define NOR_MASK     (NOP_MASK|0b100111|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SLTU_MASK    (NOP_MASK|0b101011|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SYSCALL_MASK (NOP_MASK|0b001100|R_INST_MASK|PARAMETER_0|ACCEPTED)

#define BEQ_MASK   (NOP_MASK| (0x10000000) |I_INST_MASK|PARAMETER_3|JUMP_RELATIVE|ACCEPTED)
#define BNE_MASK   (NOP_MASK|(0b000101<<26)|I_INST_MASK|PARAMETER_3|JUMP_RELATIVE|ACCEPTED)
#define ADDI_MASK  (NOP_MASK|(0b001000<<26)|I_INST_MASK|PARAMETER_3|ACCEPTED)
#define SLTIU_MASK (NOP_MASK|(0b001011<<26)|I_INST_MASK|PARAMETER_3|ACCEPTED)
#define ANDI_MASK  (NOP_MASK|(0b001100<<26)|I_INST_MASK|PARAMETER_3|ACCEPTED)
#define XORI_MASK  (NOP_MASK|(0b001110<<26)|I_INST_MASK|PARAMETER_3|ACCEPTED)
#define LW_MASK    (NOP_MASK| (0x8C000000) |I_INST_MASK|PARAMETER_3|ACCEPTED|REVERSE_OP)
#define SW_MASK    (NOP_MASK| (0xAC000000) |I_INST_MASK|PARAMETER_3|ACCEPTED|REVERSE_OP)

#define J_MASK    (NOP_MASK| (0x08000000) |J_INST_MASK|PARAMETER_1|JUMP_ABSOLUTE|ACCEPTED)
#define JAL_MASK  (NOP_MASK|(0b000011<<26)|J_INST_MASK|PARAMETER_1|JUMP_ABSOLUTE|ACCEPTED)
//以上。

//枚举所有指令。
U64 
#ifdef __GNUC__
__attribute__((weak)) //垃圾MSVC连weak都不支持#(喷血)
#endif
instructions[] = {
    SLLV_MASK, JR_MASK,   ADD_MASK, SUB_MASK, AND_MASK,  OR_MASK,    XOR_MASK,
    NOR_MASK,  SLTU_MASK, BEQ_MASK, BNE_MASK, ADDI_MASK, SLTIU_MASK, ANDI_MASK,
    XORI_MASK, LW_MASK,   SW_MASK,  J_MASK,   JAL_MASK, SYSCALL_MASK
};

//枚举所有指令格式化字符串。
//顺序按照从左到右。
//R：rs,rt,rd
//I：rs,rt,imm16
//J：imm26
//e.g.
//add %r3,%r1,%r2
//beq %r1,%r2,%i3
//jal %i1
char*
#ifdef __GNUC__
__attribute__((weak)) //垃圾MSVC连weak都不支持#(喷血)
#endif
instruction_formats[] = {
    "sllv %r3,%r2,%r1",
    "jr %r1",
    "add %r3,%r1,%r2",
    "sub %r3,%r1,%r2",
    "and %r3,%r1,%r2",
    "or %r3,%r1,%r2",
    "xor %r3,%r1,%r2",
    "nor %r3,%r1,%r2",
    "sltu %r3,%r1,%r2",
    "beq %r1,%r2,%i3",
    "bne %r1,%r2,%i3",
    "addi %r2,%r1,%i3",
    "sltiu %r2,%r1,%i3",
    "andi %r2,%r1,%i3",
    "xori %r2,%r1,%i3",
    "lw %r2,%i3(%r1)",
    "sw %r2,%i3(%r1)",
    "j %i1",
    "jal %i1",
    "syscall"
};
//枚举所有寄存器名称。
char*
#ifdef __GNUC__
__attribute__((weak)) //垃圾MSVC连weak都不支持#(喷血)
#endif
registers[] = {
    "$zero",
    "$at",
    "$v0",
    "$v1",
    "$a0",
    "$a1",
    "$a2",
    "$a3",
    "$t0",
    "$t1",
    "$t2",
    "$t3",
    "$t4",
    "$t5",
    "$t6",
    "$t7",
    "$s0",
    "$s1",
    "$s2",
    "$s3",
    "$s4",
    "$s5",
    "$s6",
    "$s7",
    "$t8",
    "$t9",
    "$k0",
    "$k1",
    "$gp",
    "$sp",
    "$s8",
    "$ra",
};
#endif
