#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "register_watcher.h"
#include "disassembler_watcher.h"
#include "vmcpu.h"
#include "disassembler.h"
#include "io_handler.h"
#include <QLineEdit>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow,public RegisterWatcher,public DisassemblerWatcher,public IOHandler
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_register_changed(int no, U32 val);
    void on_debugging_started();
    void on_debugging_stopped();
    void on_disassemble_finished(QString result);
    U32 on_io_request_sent(U32 v0,U32 a0,U32 a1,U32 a2);
private slots:
    void on_actionExit_triggered();

    void on_nextButton_clicked();

    void on_startButton_clicked();

    void on_resetButton_clicked();

    void on_actionOpenRam_triggered();

    void on_actionOpenRom_triggered();

    void on_showRamButton_clicked();

    void on_actionQt_triggered();

private:
    Ui::MainWindow *ui;
    VmCpu *cpu;
    Disassembler *disassembler;
    U64 address_width; //决定在显示地址的时候要有几个0
    U64 current_offset;
    QLineEdit* reg_line_edits[36];
    U8 is_debug_running=0;
    U64 previous_pc=0;
};
#endif // MAINWINDOW_H
