#ifndef DISASSEMBLER_WATCHER_H
#define DISASSEMBLER_WATCHER_H
#include <QString>
class DisassemblerWatcher
{
public:
    virtual void on_disassemble_finished(QString result)=0;
};


#endif // DISASSEMBLER_WATCHER_H
