#ifndef PlayerWindow_H
#define PlayerWindow_H

#include <QWidget>

#include "GLWidget.h"

class PlayerWindow : public GLWidget
{
	Q_OBJECT
public:
	PlayerWindow(QWidget *parent=0);
};

#endif
