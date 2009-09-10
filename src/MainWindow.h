#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model/Slide.h"
class MyGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        MainWindow(QWidget * parent = 0);
        ~MainWindow();
        
    private slots:
    	void newTextItem();
    	
    private:
    	Slide *m_slide;
    	MyGraphicsScene *m_scene;

};


#endif
