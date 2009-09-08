#ifndef XMLREAD_H
#define XMLREAD_H

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QList>
// #include "ModeInfo.h"

class Slide;

class XmlRead : public QObject
{
	Q_OBJECT
public:
	XmlRead(const QString & filePath);
// 	void readProject(MainWindow * mainWindow);
// 	void readDesk(Desk * desk);
// 	void readContent(Desk * desk);
	void readSlide(Slide *slide);
	

private :
// 	QDomElement m_projectElement;
// 	QDomElement m_deskElement;
	QDomElement m_contentElement;

// Q_SIGNALS:
// 	void changeMode(int);
// 	void setModeInfo(ModeInfo);
};

#endif

