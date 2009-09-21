#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "SlideGroup.h"

#include <QList>

class Document : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(QString docTitle READ docTitle WRITE setDocTitle);
	Q_PROPERTY(QString filename READ filename WRITE setFilename);
	
	
public:
	Document(const QString & filename = "");
	~Document();
	
	ITEM_PROPDEF(DocTitle,	QString,	docTitle);
	ITEM_PROPDEF(Filename,	QString,	filename);

	void addGroup(SlideGroup *);
	QList<SlideGroup *> groupList();
	
	void removeGroup(SlideGroup *);
	
	void load(const QString & filename);
	void save(const QString & filename = "");
	
	
private:
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
	
private:
	QList<SlideGroup *> m_groups;
	
	QString m_docTitle;
	QString m_filename;
};

#endif
