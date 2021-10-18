#ifndef VMCPU_H
#define VMCPU_H

/*
    虚拟CPU的入口。
*/

#include "register_watcher.h"
#include "alu.h"
#include "io_handler.h"
class VmCpu
{
private:
    U32 registers[32];
    U32 pc;
    U32 hi;
    U32 lo;
    U32 val;
    U32* ram=nullptr; //lw&sw指令用。
    U32* rom=nullptr; //程序存储使用。
    U64 ram_length=0;
    U64 rom_length=0;
    RegisterWatcher* watcher;
    IOHandler* handler;
    Alu *alu;
public:
    VmCpu(RegisterWatcher* watcher,IOHandler* handler);
    ~VmCpu();
    void process();
    void reset();
    void halt();
    void init_ram(U32* val,U64 length);
    void init_rom(U32* val,U64 length);
    U32* get_ram(U64* length); //长度信息通过指针传回。

};

#endif // VMCPU_H
