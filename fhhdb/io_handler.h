#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H
#include "vardefs.h"

class IOHandler {
    public:
    //由于fhh只需要实现SPIM的整数Syscall，所以只需要返回一个数据就可以！
    virtual U32 on_io_request_sent(U32 v0,U32 a0,U32 a1,U32 a2)=0;
};
#endif // SYSCALL_HANDLER_H
