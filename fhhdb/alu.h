#ifndef ALU_H
#define ALU_H
/*
    虚拟CPU的计算元件。
*/
#include "vardefs.h"
class Alu
{
public:
    Alu();
    U32 calculate(U32 operand,U32 a,U32 b);
private:
    U8 zf;
    U8 of;
};

#endif // ALU_H
