#include "alu.h"

Alu::Alu()
{

}

U32 Alu::calculate(U32 operand,U32 a,U32 b)
{
    U64 result=0;
    switch(operand) {
        case 0:{        //and
            result = a&b;
            break;
        }
        case 1:{        //or
            result = a|b;
            break;
        }
        case 2:{        //xor
            result = a^b;
            break;
        }
        case 3:{        //nor
            result = ~(a|b);
            break;
        }
        case 4:{        //add
            result = a+b;
            if(result&0xFFFFFFFF00000000) {
                of = 1;
            }
            break;
        }
        case 5:{        //sub
            result = a-b;
            break;
        }
        case 6:{        //slt
            result = a<b?1:0;
            break;
        }
        case 7:{        //sll
            result = b<<a;
            break;
        }
        case 8:{        //mul
            result = a*b;
            break;
        }
    }
    return result&0xFFFFFFFF;
}
