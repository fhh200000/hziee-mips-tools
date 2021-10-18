#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <cmath>
#include "ramdialog.h"
#include <utility.h>
#include <QMessageBox>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      cpu(new VmCpu(this,this)),
      disassembler(new Disassembler(this))
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());
    const QLineEdit* line_edits[]= {
        ui->reg0,
        ui->reg1,
        ui->reg2,
        ui->reg3,
        ui->reg4,
        ui->reg5,
        ui->reg6,
        ui->reg7,
        ui->reg8,
        ui->reg9,
        ui->reg10,
        ui->reg11,
        ui->reg12,
        ui->reg13,
        ui->reg14,
        ui->reg15,
        ui->reg16,
        ui->reg17,
        ui->reg18,
        ui->reg19,
        ui->reg20,
        ui->reg21,
        ui->reg22,
        ui->reg23,
        ui->reg24,
        ui->reg25,
        ui->reg26,
        ui->reg27,
        ui->reg28,
        ui->reg29,
        ui->reg30,
        ui->reg31,
        ui->reg32,
        ui->reg33,
        ui->reg34,
        ui->reg35,
    };
    memcpy(this->reg_line_edits,line_edits,sizeof(line_edits));
    ui->statusbar->showMessage("就绪",-1);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cpu;
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_register_changed(int no, U32 val)
{
    if(is_debug_running) {
        this->reg_line_edits[no]->setText(QString::asprintf("0x%08x",val));
    }
    if(no==32) { //PC
        ui->listWidget->setCurrentRow(previous_pc/4);
        previous_pc = val;
    }
}
void MainWindow::on_debugging_started()
{
    ui->nextButton->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->startButton->setText("停止");
    is_debug_running = 1;
    previous_pc = 0;
    ui->statusbar->showMessage("调试已开始");
}
void MainWindow::on_debugging_stopped()
{
    ui->nextButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    for(int i=0;i<36;i++) {
        reg_line_edits[i]->clear();
    }
    ui->startButton->setText("开始");
    is_debug_running = 0;
    ui->statusbar->showMessage("调试已结束");
}
void MainWindow::on_nextButton_clicked()
{
    //测试。
    cpu->process();
}

void MainWindow::on_actionOpenRom_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开ROM文件",".","ROM文件(*.o *.coe)");
    QFile* file = new QFile(filename);
    QByteArray bytearray;
    U32* instructions;
    I64 instruction_byte_count = 0;
    bool is_coe_file = false;
    if(file->exists()) {
        ui->listWidget->clear();
        file->open(QIODevice::ReadOnly);
        //这里要区分.o和.coe了.
        //取巧，只判断最后一位23333
        if(*(filename.end()-1)=='o') { //.o
            bytearray = file->readAll();
            instructions = reinterpret_cast<U32*>(bytearray.data());
            instruction_byte_count = file->size();
        }
        else { //.coe
            is_coe_file = true;
            QByteArray bytearray;
            file->readLine();//跳过第一行，因为没啥用。
            bytearray = file->readLine();//读入第二行。
            //寻找=。
            U64 begin_addr = 0;
            while(bytearray.at(begin_addr)!='=') {
                begin_addr++;
            }
            bytearray = bytearray.mid(begin_addr+1);
            QList<QByteArray> split = bytearray.split(',');
            //把最后一个数据的分号变为逗号。
            (split.end()-1)->replace(';',' ');
            instructions = new U32[split.size()];
            U64 i=0;
            for(QList<QByteArray>::iterator iter = split.begin();iter!=split.end();++iter) {
                //大小端问题。
                instructions[i] = bswap_32((*iter).toLongLong(nullptr,16));
                i++;
            }
            instruction_byte_count = i*4;
        }
        address_width = (log2(instruction_byte_count-1)/4)+1;
        cpu->init_rom(instructions,instruction_byte_count);
        //反汇编所有指令。
        for (I64 i=0;i<(instruction_byte_count/4);i++) {
            current_offset = i*4;
            disassembler->disassemble(instructions[i]);
        }
    }
    ui->statusbar->showMessage(QString("已加载%1字节到指令ROM").arg(instruction_byte_count),-1);
    file->close();
    if(is_coe_file&&instructions!=nullptr) {
        delete[] instructions;
    }
    delete file;
}

void MainWindow::on_actionOpenRam_triggered()
{
    QFile* file = new QFile(QFileDialog::getOpenFileName(this,"打开RAM文件",".","RAM文件(*.coe)"));
    QByteArray bytearray;
    U32* ram;
    U64 ram_size=0;
    if(file->exists()) {
        file->open(QIODevice::ReadOnly);
        //申请足够大的数据buffer.
        ram = new U32[file->size()];
        file->readLine();//跳过第一行，因为没啥用。
        bytearray = file->readLine();//读入第二行。
        //寻找=。
        U64 begin_addr = 0;
        while(bytearray.at(begin_addr)!='=') {
            begin_addr++;
        }
        bytearray = bytearray.mid(begin_addr+1);
        QList<QByteArray> split = bytearray.split(',');
        //把最后一个数据的分号变为逗号。
        (split.end()-1)->replace(';',' ');
        for(QList<QByteArray>::iterator iter = split.begin();iter!=split.end();++iter) {
            ram[ram_size] = (*iter).toLongLong(nullptr,16);
            ram_size++;
        }
        cpu->init_ram(ram,ram_size*4);
        delete[] ram;
    }
    ui->statusbar->showMessage(QString("已加载%1字节到数据RAM").arg(ram_size*4),-1);
    file->close();
    delete file;
}

void MainWindow::on_disassemble_finished(QString result)
{
    result = QString::asprintf("0x%0*llx   %s",(int)address_width,current_offset,result.toStdString().c_str());
    ui->listWidget->addItem(result);
}

void MainWindow::on_startButton_clicked()
{
    if(is_debug_running) {
        cpu->halt();
    }
    else {
        cpu->reset();
    }
}

void MainWindow::on_resetButton_clicked()
{
    cpu->reset();
}


void MainWindow::on_showRamButton_clicked()
{
    U64 ram_length;
    U32* ram = cpu->get_ram(&ram_length);
    RamDialog* dialog = new RamDialog(this,ram,ram_length);
    dialog->show();
}


void MainWindow::on_actionQt_triggered()
{
    QMessageBox::aboutQt(this);
}

U32 MainWindow::on_io_request_sent(U32 v0,U32 a0,U32 a1,U32 a2)
{
    switch (v0) {
        case 1: { //输出整数
            QMessageBox::information(this,"整数输出",QString("输出整数%1(0x%2)").arg(a0).arg(a0&0xFFFFFFFF,8,16,QChar('0')));
            return v0;
        }
        case 5: { //输入整数
            return QInputDialog::getInt(this,"整数输入","请输入整数");
        }
        case 10: { //退出
            cpu->halt();
            return 0;
        }
    }

    //stub!
    (void)a0;
    (void)a1;
    (void)a2;

    return 0;
}
