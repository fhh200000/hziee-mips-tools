#include "vmcpu.h"
#include <cstring>

#include <QDebug>
#include "utility.h"
#include "table.h"
VmCpu::VmCpu(RegisterWatcher* watcher,IOHandler* handler):watcher(watcher),handler(handler),alu(new Alu())
{

}
VmCpu::~VmCpu()
{
    if(ram!=nullptr)
        delete[] ram;
    if(rom!=nullptr)
        delete[] rom;
    delete alu;
}
void VmCpu::init_ram(U32* val,U64 length)
{
    if(this->ram!=nullptr) {
        delete[](this->ram);
    }
    this->ram = new U32[length/(sizeof(U32)/sizeof(U8))];
    memcpy(this->ram,val,length);
    ram_length = length;
}
void VmCpu::init_rom(U32* val,U64 length)
{
    if(this->rom!=nullptr) {
        delete[](this->rom);
    }
    this->rom = new U32[length/(sizeof(U32)/sizeof(U8))];
    memcpy(this->rom,val,length);
    rom_length = length;
}
void VmCpu::process()
{
    U32 params[3];
    if(pc>=rom_length) {
        halt();
        return;
    }
    val = bswap_32(rom[pc/4]);
    pc = pc+4;
    //确认指令类型。
    if(val&OP_MASK) {         //I/J Commands
        //拆解指令参数，假设是I型。
        U32 operand =(val&OP_MASK)>>26;
        params[0] = ((val&0x03E00000)>>21);
        params[1] = ((val&0x001F0000)>>16);
        params[2] =  (val&0x0000FFFF);
        //翻译Operand。
        switch (operand) {
            case 0b001000: {
                operand = 0b100; //addi
                //符号位扩展。
                if(params[2]&0x8000) {
                    params[2] |= 0xFFFF0000;
                }
                break;
            }
            case 0b001100: {
                operand = 0b000; //andi
                break;
            }
            case 0b001110: {
                operand = 0b010; //xori
                break;
            }
            case 0b001011: {
                operand = 0b110; //sltiu
                break;
            }
            case 0b100011: { //lw
                //符号位扩展。
                if(params[2]&0x8000) {
                    params[2] |= 0xFFFF0000;
                }
                U32 offset = (registers[params[0]]+params[2]);
                //如果太大，则返回0.
                if(offset<ram_length) {
                    registers[params[1]] = ram[offset/4];
                }
                else {
                    registers[params[1]] = 0;
                }
                watcher->on_register_changed(params[1],registers[params[1]]);
                goto refresh;
            }
            case 0b101011: { //sw
                //符号位扩展。
                if(params[2]&0x8000) {
                    params[2] |= 0xFFFF0000;
                }
                U32 offset = (registers[params[0]]+params[2]);
                //如果太大，则丢弃数据.
                if(offset<ram_length) {
                    ram[offset/4] = registers[params[1]];
                }
                goto refresh;
            }
            case 0b000100:{ //beq
                if(registers[params[0]]==registers[params[1]]) {
                    pc += (params[2]<<2);
                }
                goto refresh;
            }
            case 0b000101:{ //bne
                if(registers[params[0]]!=registers[params[1]]) {
                    pc += (params[2]<<2);
                }
                goto refresh;
            }
            //以下是J类型指令，需要对操作数修改。
            case 0b000010:{ //j
                params[0] = ((params[0]<<21)|(params[1]<<16)|(params[2]));
                pc = (pc&0xF0000000)|(params[0]<<2);
                goto refresh;
            }
            case 0b000011:{ //jal
                params[0] = ((params[0]<<21)|(params[1]<<16)|(params[2]));
                registers[31] = pc;
                pc = (pc&0xF0000000)|(params[0]<<2);
                watcher->on_register_changed(31,registers[31]);
                goto refresh;
            }
        }
        //取寄存器值。
        params[0] = registers[params[0]];
        //交由ALU运算。
        U32 output = alu->calculate(operand,params[0],params[2]);
        //放到目标寄存器里。
        registers[params[1]] = output;
        watcher->on_register_changed(params[1],registers[params[1]]);
    }
    else {                      //R Commands
        //拆解指令参数。
        params[0] = ((val&0x03E00000)>>21);
        params[1] = ((val&0x001F0000)>>16);
        params[2] = ((val&0x0000F800)>>11);
        U32 operand = val&FUNC_MASK;
        //取寄存器值。
        params[0] = registers[params[0]];
        params[1] = registers[params[1]];
        //翻译Operand。
        switch (operand) {
            case 0b100000: {
                operand = 0b100; //add
                break;
            }
            case 0b100010: {
                operand = 0b101; //sub
                break;
            }
            case 0b100100: {
                operand = 0b000; //and
                break;
            }
            case 0b100101: {
                operand = 0b001; //or
                break;
            }
            case 0b100110: {
                operand = 0b010; //xor
                break;
            }
            case 0b100111: {
                operand = 0b011; //nor
                break;
            }
            case 0b101011: {
                operand = 0b110; //sltu
                break;
            }
            case 0b000100: {
                operand = 0b111; //sllv
                break;
            }
            case 0b001000: { //jr
                pc = params[0];
                goto refresh;
            }
            case 0b00010011000: { //mul;
                operand = 0b1000; //注意！扩展了！
                break;
            }
            case 0b001100: { //syscall
                U32 prev_v0 = registers[2];
                registers[2] = handler->on_io_request_sent(registers[2],registers[4],registers[5],registers[6]);

                if(prev_v0==10||prev_v0==17) { //Exit
                    watcher->on_register_changed(32,pc);
                    return;
                }
                if(prev_v0!=registers[2]) {
                     watcher->on_register_changed(2,registers[2]);
                }
                goto refresh;
            }
        }
        //交由ALU运算。
        U32 output = alu->calculate(operand,params[0],params[1]);
        //放到目标寄存器里。
        registers[params[2]] = output;
        watcher->on_register_changed(params[2],registers[params[2]]);
    }
refresh:
    watcher->on_register_changed(32,pc);
    watcher->on_register_changed(35,val);
}
void VmCpu::reset()
{
    for(int i=0;i<32;i++) {
        registers[i]=0;
    }
    pc = 0;
    hi = 0;
    lo = 0;
    val = 0;
    watcher->on_debugging_started();
    for(int i=0;i<36;i++) {
        watcher->on_register_changed(i,0x00);
    }
}
void VmCpu::halt()
{
    pc = 0;
    val = 0;
    watcher->on_debugging_stopped();
}
U32* VmCpu::get_ram(U64* length)
{
    *length = ram_length;
    return ram;
}
