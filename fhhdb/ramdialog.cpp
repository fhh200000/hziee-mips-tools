#include "ramdialog.h"
#include "ui_ramdialog.h"

RamDialog::RamDialog(QWidget *parent,U32* ram,U64 ram_length) :
    QDialog(parent),
    ui(new Ui::RamDialog),
    ram(ram),
    ram_length(ram_length)
{
    ui->setupUi(this);
    ui->mem_count->setText(ui->mem_count->text().arg(ram_length));
    if(ram_length) {
        for(U64 i=0;i<ram_length;i+=4) {
            ui->mem_list->addItem(QString::asprintf("0x%04llx    0x%08x",i,ram[i/4]));
        }
    }
}

RamDialog::~RamDialog()
{
    delete ui;
}
void RamDialog::resizeEvent(QResizeEvent *event)
{
    ui->mem_list->resize(width()-40,height()-60);
    (void)event;
}
