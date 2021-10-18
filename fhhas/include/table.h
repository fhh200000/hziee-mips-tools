#ifndef TABLE_H
#define TABLE_H
#include "automata.h"
U8 accepted = 0;

//由于MIPS的指令集比较特殊（定长指令），此处采用掩码较为简单。
//以下给出了不同指令的掩码。
#define ACCEPTED 0x8000000000000000
#define REJECTED 0xFFFFFFFFFFFFFFFF
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

//应对JR的Hotfix.
#define JR 0x0000008000000000

#define SLLV_MASK    (NOP_MASK|0b000100|R_INST_MASK|PARAMETER_3|ACCEPTED|REVERSE_OP)
#define  JR_MASK     (NOP_MASK|0b001000|R_INST_MASK|PARAMETER_1|ACCEPTED|JR)
#define ADD_MASK     (NOP_MASK|0b100000|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SUB_MASK     (NOP_MASK|0b100010|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define AND_MASK     (NOP_MASK|0b100100|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define  OR_MASK     (NOP_MASK|0b100101|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define XOR_MASK     (NOP_MASK|0b100110|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define NOR_MASK     (NOP_MASK|0b100111|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SLTU_MASK    (NOP_MASK|0b101011|R_INST_MASK|PARAMETER_3|ACCEPTED)
#define SYSCALL_MASK (NOP_MASK|0b001100|R_INST_MASK|PARAMETER_0|ACCEPTED)
#define MUL_MASK     (NOP_MASK|0b00010011000|R_INST_MASK|PARAMETER_3|ACCEPTED)

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

transrule instruction_rule[] = 
{
    {0 ,'a',1},
    {0 ,'b',4},
    {0 ,'j',J_MASK},    //j
    {0 ,'l',8},
    {0 ,'m',28},
    {0 ,'n',18},
    {0 ,'o',9},
    {0 ,'s',10},
    {0 ,'x',16},
    {1 ,'d',2},
    {1 ,'n',3},
    {2 ,'d',ADD_MASK},  //add
    {3 ,'d',AND_MASK},  //and
    {4 ,'e',5},
    {4 ,'n',6},
    {5 ,'q',BEQ_MASK},  //beq
    {6 ,'e',BNE_MASK},  //bne
    {7 ,'l',JAL_MASK},  //jal
    {8 ,'w',LW_MASK},   //lw
    {9 ,'r',OR_MASK},   //or
    {10,'l',11},
    {10,'u',15},
    {10,'w',SW_MASK},
    {10,'y',23},
    {11,'l',12},
    {11,'t',13},
    {11,'w',LW_MASK},   //lw
    {12,'v',SLLV_MASK}, //sllv
    {13,'i',22},
    {13,'u',SLTU_MASK}, //sltu
    {15,'b',SUB_MASK},  //sub
    {16,'o',17},
    {17,'r',XOR_MASK},  //xor
    {18,'o',19},
    {19,'r',NOR_MASK},  //nor
    {22,'u',SLTIU_MASK},//sltiu
    {23,'s',24},
    {24,'c',25},
    {25,'a',26},
    {26,'l',27},
    {27,'l',SYSCALL_MASK},//syscall
    {28,'u',29},
    {29,'l',MUL_MASK},//mul
    //存在移进/归约冲突，手动解决。
    {ADD_MASK,'i',ADDI_MASK}, //addi
    {AND_MASK,'i',ANDI_MASK}, //andi
    {XOR_MASK,'i',XORI_MASK}, //xori
    {J_MASK  ,'a',7},
    {J_MASK  ,'r',JR_MASK},   //jr
};
transrule reg_code_rule[] = 
{
    {0 ,'$',1},
    {1 ,'0',1|ACCEPTED},
    {1 ,'1',1|ACCEPTED},
    {1 ,'2',1|ACCEPTED},
    {1 ,'3',1|ACCEPTED},
    {1 ,'4',1|ACCEPTED},
    {1 ,'5',1|ACCEPTED},
    {1 ,'6',1|ACCEPTED},
    {1 ,'7',1|ACCEPTED},
    {1 ,'8',1|ACCEPTED},
    {1 ,'9',1|ACCEPTED},
    {1|ACCEPTED,'0',1|ACCEPTED},
    {1|ACCEPTED,'1',1|ACCEPTED},
    {1|ACCEPTED,'2',1|ACCEPTED},
    {1|ACCEPTED,'3',1|ACCEPTED},
    {1|ACCEPTED,'4',1|ACCEPTED},
    {1|ACCEPTED,'5',1|ACCEPTED},
    {1|ACCEPTED,'6',1|ACCEPTED},
    {1|ACCEPTED,'7',1|ACCEPTED},
    {1|ACCEPTED,'8',1|ACCEPTED},
    {1|ACCEPTED,'9',1|ACCEPTED},
};
transrule reg_name_rule[] = 
{
    {0 ,'$',33},
    {33,'a',34},
    {33,'f',35},
    {33,'g',36},
    {33,'k',37},
    {33,'r',38},
    {33,'s',39},
    {33,'t',40},
    {33,'v',41},
    {33,'z',42},
    {34,'t',1 |ACCEPTED}, //$1 ,at
    {34,'0',4 |ACCEPTED}, //$4 ,a0
    {34,'1',5 |ACCEPTED}, //$5 ,a1
    {34,'2',6 |ACCEPTED}, //$6 ,a2
    {34,'3',7 |ACCEPTED}, //$7 ,a3
    {35,'p',30|ACCEPTED}, //$30,fp
    {36,'p',28|ACCEPTED}, //$28,gp
    {37,'0',26|ACCEPTED}, //$26,k0
    {37,'1',27|ACCEPTED}, //$27,k1
    {38,'a',31|ACCEPTED}, //$31,ra
    {39,'0',16|ACCEPTED}, //$16,s0
    {39,'1',17|ACCEPTED}, //$17,s1
    {39,'2',18|ACCEPTED}, //$18,s2
    {39,'3',19|ACCEPTED}, //$19,s3
    {39,'4',20|ACCEPTED}, //$20,s4
    {39,'5',21|ACCEPTED}, //$21,s5
    {39,'6',22|ACCEPTED}, //$22,s6
    {39,'7',23|ACCEPTED}, //$23,s7
    {39,'8',30|ACCEPTED}, //$30,s7
    {39,'p',29|ACCEPTED}, //$29,sp
    {40,'0',8 |ACCEPTED}, //$8 ,t0
    {40,'1',9 |ACCEPTED}, //$9 ,t1
    {40,'2',10|ACCEPTED}, //$10,t2
    {40,'3',11|ACCEPTED}, //$11,t3
    {40,'4',12|ACCEPTED}, //$12,t4
    {40,'5',13|ACCEPTED}, //$13,t5
    {40,'6',14|ACCEPTED}, //$14,t6
    {40,'7',15|ACCEPTED}, //$15,t7
    {40,'8',24|ACCEPTED}, //$24,t8
    {40,'9',25|ACCEPTED}, //$25,t9
    {41,'0',2 |ACCEPTED}, //$2 ,v0
    {41,'1',3 |ACCEPTED}, //$3 ,v1
    {42,'e',43},
    {43,'r',44},
    {44,'o',0 |ACCEPTED}, //$0 ,zero
}; 

transrule pure_digit_rule[] = {
    {0,'-',0},
    {0,'0',0|ACCEPTED},
    {0|ACCEPTED,'0',0|ACCEPTED},
    {0,'1',3|ACCEPTED},
    {0,'2',3|ACCEPTED},
    {0,'3',3|ACCEPTED},
    {0,'4',3|ACCEPTED},
    {0,'5',3|ACCEPTED},
    {0,'6',3|ACCEPTED},
    {0,'7',3|ACCEPTED},
    {0,'8',3|ACCEPTED},
    {0,'9',3|ACCEPTED},
    {1,'0',2|ACCEPTED},
    {1,'1',2|ACCEPTED},
    {1,'2',2|ACCEPTED},
    {1,'3',2|ACCEPTED},
    {1,'4',2|ACCEPTED},
    {1,'5',2|ACCEPTED},
    {1,'6',2|ACCEPTED},
    {1,'7',2|ACCEPTED},
    {1,'8',2|ACCEPTED},
    {1,'9',2|ACCEPTED},
    {1,'a',2|ACCEPTED},
    {1,'b',2|ACCEPTED},
    {1,'c',2|ACCEPTED},
    {1,'d',2|ACCEPTED},
    {1,'e',2|ACCEPTED},
    {1,'f',2|ACCEPTED},
    {0|ACCEPTED,'x',1},
    {2|ACCEPTED,'0',2|ACCEPTED},
    {2|ACCEPTED,'1',2|ACCEPTED},
    {2|ACCEPTED,'2',2|ACCEPTED},
    {2|ACCEPTED,'3',2|ACCEPTED},
    {2|ACCEPTED,'4',2|ACCEPTED},
    {2|ACCEPTED,'5',2|ACCEPTED},
    {2|ACCEPTED,'6',2|ACCEPTED},
    {2|ACCEPTED,'7',2|ACCEPTED},
    {2|ACCEPTED,'8',2|ACCEPTED},
    {2|ACCEPTED,'9',2|ACCEPTED},
    {2|ACCEPTED,'a',2|ACCEPTED},
    {2|ACCEPTED,'b',2|ACCEPTED},
    {2|ACCEPTED,'c',2|ACCEPTED},
    {2|ACCEPTED,'d',2|ACCEPTED},
    {2|ACCEPTED,'e',2|ACCEPTED},
    {2|ACCEPTED,'f',2|ACCEPTED},
    {3|ACCEPTED,'0',3|ACCEPTED},
    {3|ACCEPTED,'1',3|ACCEPTED},
    {3|ACCEPTED,'2',3|ACCEPTED},
    {3|ACCEPTED,'3',3|ACCEPTED},
    {3|ACCEPTED,'4',3|ACCEPTED},
    {3|ACCEPTED,'5',3|ACCEPTED},
    {3|ACCEPTED,'6',3|ACCEPTED},
    {3|ACCEPTED,'7',3|ACCEPTED},
    {3|ACCEPTED,'8',3|ACCEPTED},
    {3|ACCEPTED,'9',3|ACCEPTED},
};
transrule label_rule[] = {
    {0,'a',0},
    {0,'b',0},
    {0,'c',0},
    {0,'d',0},
    {0,'e',0},
    {0,'f',0},
    {0,'g',0},
    {0,'h',0},
    {0,'i',0},
    {0,'j',0},
    {0,'k',0},
    {0,'l',0},
    {0,'m',0},
    {0,'n',0},
    {0,'o',0},
    {0,'p',0},
    {0,'q',0},
    {0,'r',0},
    {0,'s',0},
    {0,'t',0},
    {0,'u',0},
    {0,'v',0},
    {0,'w',0},
    {0,'x',0},
    {0,'y',0},
    {0,'z',0},
    {0,'0',0},
    {0,'1',0},
    {0,'2',0},
    {0,'3',0},
    {0,'4',0},
    {0,'5',0},
    {0,'6',0},
    {0,'7',0},
    {0,'8',0},
    {0,'9',0},
    {0,'_',0},
    {0,':',1|ACCEPTED},
};
transrule const_val_rule[] = {
    {0,'a',1|ACCEPTED},
    {0,'b',1|ACCEPTED},
    {0,'c',1|ACCEPTED},
    {0,'d',1|ACCEPTED},
    {0,'e',1|ACCEPTED},
    {0,'f',1|ACCEPTED},
    {0,'g',1|ACCEPTED},
    {0,'h',1|ACCEPTED},
    {0,'i',1|ACCEPTED},
    {0,'j',1|ACCEPTED},
    {0,'k',1|ACCEPTED},
    {0,'l',1|ACCEPTED},
    {0,'m',1|ACCEPTED},
    {0,'n',1|ACCEPTED},
    {0,'o',1|ACCEPTED},
    {0,'p',1|ACCEPTED},
    {0,'q',1|ACCEPTED},
    {0,'r',1|ACCEPTED},
    {0,'s',1|ACCEPTED},
    {0,'t',1|ACCEPTED},
    {0,'u',1|ACCEPTED},
    {0,'v',1|ACCEPTED},
    {0,'w',1|ACCEPTED},
    {0,'x',1|ACCEPTED},
    {0,'y',1|ACCEPTED},
    {0,'z',1|ACCEPTED},
    {0,'_',1|ACCEPTED},
    {1|ACCEPTED,'a',1|ACCEPTED},
    {1|ACCEPTED,'b',1|ACCEPTED},
    {1|ACCEPTED,'c',1|ACCEPTED},
    {1|ACCEPTED,'d',1|ACCEPTED},
    {1|ACCEPTED,'e',1|ACCEPTED},
    {1|ACCEPTED,'f',1|ACCEPTED},
    {1|ACCEPTED,'g',1|ACCEPTED},
    {1|ACCEPTED,'h',1|ACCEPTED},
    {1|ACCEPTED,'i',1|ACCEPTED},
    {1|ACCEPTED,'j',1|ACCEPTED},
    {1|ACCEPTED,'k',1|ACCEPTED},
    {1|ACCEPTED,'l',1|ACCEPTED},
    {1|ACCEPTED,'m',1|ACCEPTED},
    {1|ACCEPTED,'n',1|ACCEPTED},
    {1|ACCEPTED,'o',1|ACCEPTED},
    {1|ACCEPTED,'p',1|ACCEPTED},
    {1|ACCEPTED,'q',1|ACCEPTED},
    {1|ACCEPTED,'r',1|ACCEPTED},
    {1|ACCEPTED,'s',1|ACCEPTED},
    {1|ACCEPTED,'t',1|ACCEPTED},
    {1|ACCEPTED,'u',1|ACCEPTED},
    {1|ACCEPTED,'v',1|ACCEPTED},
    {1|ACCEPTED,'w',1|ACCEPTED},
    {1|ACCEPTED,'x',1|ACCEPTED},
    {1|ACCEPTED,'y',1|ACCEPTED},
    {1|ACCEPTED,'z',1|ACCEPTED},
    {1|ACCEPTED,'0',1|ACCEPTED},
    {1|ACCEPTED,'1',1|ACCEPTED},
    {1|ACCEPTED,'2',1|ACCEPTED},
    {1|ACCEPTED,'3',1|ACCEPTED},
    {1|ACCEPTED,'4',1|ACCEPTED},
    {1|ACCEPTED,'5',1|ACCEPTED},
    {1|ACCEPTED,'6',1|ACCEPTED},
    {1|ACCEPTED,'7',1|ACCEPTED},
    {1|ACCEPTED,'8',1|ACCEPTED},
    {1|ACCEPTED,'9',1|ACCEPTED},
    {1|ACCEPTED,'_',1|ACCEPTED},
};

status is_instruction(char* begin,int length)
{
    return naive_dfa(instruction_rule,(sizeof(instruction_rule)/sizeof(transrule)),length,begin);
}
status is_reg_name(char* begin,int length)
{
    return naive_dfa(reg_name_rule,(sizeof(reg_name_rule)/sizeof(transrule)),length,begin);
}
status is_reg_code(char* begin,int length)
{
    return naive_dfa(reg_code_rule,(sizeof(reg_code_rule)/sizeof(transrule)),length,begin);
}
status is_pure_digit(char* begin,int length)
{
    return naive_dfa(pure_digit_rule,(sizeof(pure_digit_rule)/sizeof(transrule)),length,begin);
}
status is_label(char* begin,int length)
{
    return naive_dfa(label_rule,(sizeof(label_rule)/sizeof(transrule)),length,begin);
}
status is_const_val(char* begin,int length)
{
    return naive_dfa(const_val_rule,(sizeof(const_val_rule)/sizeof(transrule)),length,begin);
}
status (*check_type[])(char* begin,int length) = {
    is_instruction,
    is_reg_name,
    is_reg_code,
    is_pure_digit,
    is_label,
    is_const_val,
};
U32 check_type_length = sizeof(check_type)/sizeof(void*);

#endif
