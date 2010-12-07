#include "DirectorWindow.h"
#include "ui_DirectorWindow.h"

DirectorWindow::DirectorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DirectorWindow)
{
    ui->setupUi(this);

}

DirectorWindow::~DirectorWindow()
{
    delete ui;
}
