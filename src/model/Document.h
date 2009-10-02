#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "SlideGroup.h"

#include <QList>

class Document : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(QString docTitle READ docTitle WRITE setDocTitle);
	Q_PROPERTY(QString filename READ filename WRITE setFilename);
	Q_PROPERTY(double aspectRatio READ aspectRatio WRITE setAspectRatio);
	
	
public:
	Document(const QString & filename = "");
	~Document();
	
	ITEM_PROPDEF(DocTitle,		QString,	docTitle);
	ITEM_PROPDEF(Filename,		QString,	filename);
	ITEM_PROPDEF(AspectRatio,	double,		aspectRatio);

	void addGroup(SlideGroup *);
	QList<SlideGroup *> groupList();
	int numGroups() { return m_groups.size(); }
	SlideGroup * at(int sortedIdx);
	
	void removeGroup(SlideGroup *);
	
	void load(const QString & filename);
	void save(const QString & filename = "");

	void setAspect16x9() { setAspectRatio(16/9); }
	void setAspect4x3()  { setAspectRatio(4/3);  }

signals:
	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private slots:
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private:
	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	
private:
	QList<SlideGroup *> m_groups;
	
	QString m_docTitle;
	QString m_filename;
	double m_aspectRatio;
};

#endif
