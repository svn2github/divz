#ifndef DIRECTORWINDOW_H
#define DIRECTORWINDOW_H

#include <QMainWindow>

namespace Ui {
    class DirectorWindow;
}

class DirectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DirectorWindow(QWidget *parent = 0);
    ~DirectorWindow();

private:
    Ui::DirectorWindow *ui;
};

#endif // DIRECTORWINDOW_H
