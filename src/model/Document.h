#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "model/SlideGroup.h"

#include <QList>
#include <QObject>


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
	SlideGroup * groupById(int groupId);
	
	void removeGroup(SlideGroup *);
	
	void load(const QString & filename);
	void save(const QString & filename = "");


	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	
	
	void setAspect16x9() { setAspectRatio(16/9); }
	void setAspect4x3()  { setAspectRatio(4/3);  }
	
signals:
	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void aspectRatioChanged(double);

private slots:
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private:
	QList<SlideGroup *> m_groups;
	
	QString m_docTitle;
	QString m_filename;
	double m_aspectRatio;
};

#include <QThread>
class DocumentSaveThread : public QThread
{
	Q_OBJECT
public:
	DocumentSaveThread(Document*doc, const QString &file="")  
		: m_doc(doc)
		, m_file(file)
		
	{
		connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	}
	
	virtual ~DocumentSaveThread() {}
	
	void run()
	{
		if(!m_doc)
			return;
		if(m_file.isEmpty() && 
		   m_doc->filename().isEmpty())
			return;
		m_doc->save(m_file);
	}

private:	
	Document *m_doc;
	QString m_file;
};

#endif
