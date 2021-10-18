#ifndef RAMDIALOG_H
#define RAMDIALOG_H

#include <QDialog>
#include "vardefs.h"
namespace Ui {
class RamDialog;
}

class RamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RamDialog(QWidget *parent,U32* ram,U64 ram_length);
    ~RamDialog();
    void resizeEvent(QResizeEvent *event);
private:
    Ui::RamDialog *ui;
    U32* ram;
    U64 ram_length;
};

#endif // RAMDIALOG_H
