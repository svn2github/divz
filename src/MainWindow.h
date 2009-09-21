#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "model/Document.h"
class MyGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        MainWindow(QWidget * parent = 0);
        ~MainWindow();
        
    private slots:
    	void newTextItem();
        void newBoxItem();
        void newVideoItem();
        
    	
    private:
    	//Slide *m_slide;
    	Document m_doc;
    	MyGraphicsScene *m_scene;

};


#endif
