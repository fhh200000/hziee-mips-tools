#ifndef REGISTER_WATCHER_H
#define REGISTER_WATCHER_H
/*
    与Java的接口类似。
    通过定义一个抽象的register_watcher的方法监控寄存器。
    当寄存器改变的时候，调用回调接口完成改写。
*/
#include "vardefs.h"
class RegisterWatcher {
public:
    virtual void on_register_changed(int no,U32 val)=0;
    virtual void on_debugging_started()=0;
    virtual void on_debugging_stopped()=0;
};
#endif // REGISTER_WATCHER_H
