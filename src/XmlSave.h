#ifndef XMLSAVE_H
#define XMLSAVE_H

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QSize>
// #include "ModeInfo.h"

// class PictureContent;
// class TextContent;
// class AbstractContent;
// class Desk;
class Slide;

class XmlSave : public QObject
{
	Q_OBJECT
public:
	XmlSave(const QString &);
	~XmlSave();
// 	void saveContent(const Desk *);
 	void saveSlide(Slide *);
// 	void saveProject(int, const ModeInfo&);

private :
	QDomDocument doc;
	QDomElement m_rootElement;
	QDomElement m_contentElements;
	QFile file;
	QTextStream out;
};

#endif

