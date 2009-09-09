#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "model/Slide.h"
class MainWindow : public QWidget
{
    Q_OBJECT
    public:
        MainWindow(QWidget * parent = 0);
        ~MainWindow();
    private:
    	Slide *m_slide;

};


#endif
