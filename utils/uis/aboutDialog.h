#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>
#include "ui_aboutDialog.h"

class AboutDialog : public QWidget
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialogClass ui;
};

#endif // ABOUTDIALOG_H
