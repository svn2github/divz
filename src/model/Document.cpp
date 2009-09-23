#include "Document.h"

#include <assert.h>

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QList>

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include <QStringList>

#include <QDebug>

Document::Document(const QString & s)  
{
	if(!s.isEmpty())
		load(s);
}

Document::~Document() 
{
	qDeleteAll(m_groups);
}

QList<SlideGroup *> Document::groupList() { return m_groups; }

void Document::addGroup(SlideGroup *g)
{
	assert(g != NULL);
	emit slideGroupChanged(g, "add", 0, "", 0, "", "", QVariant());
	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	m_groups.append(g);
}

void Document::removeGroup(SlideGroup *g)
{
	assert(g != NULL);
	disconnect(g,0,this,0);
	emit slideGroupChanged(g, "remove", 0, "", 0, "", "", QVariant());
	m_groups.removeAll(g);
}


void Document::slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value)
{
	SlideGroup * g = dynamic_cast<SlideGroup *>(sender());
	emit slideGroupChanged(g, "change", slide, slideOperation, item, operation, fieldName, value);
}

void Document::setDocTitle(QString s)  { m_docTitle = s; }
void Document::setFilename(QString s)  { m_filename = s; }

void Document::load(const QString & s)
{
	m_filename = s;
	
	// Load the file
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QMessageBox::critical(0, tr("Loading error"), tr("Unable to load file %1").arg(m_filename));
		throw(0);
		return;
	}
	
	// And create the XML document into memory (with nodes...)
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(&file, false, error)) 
	{
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse file %1. The error was: %2").arg(m_filename, *error));
		file.close();
		throw(0);
		return;
	}
	file.close();
	
	delete error;
	error = 0;
	
	QDomElement root = doc.documentElement(); // The root node
	
	fromXml(root);
}

bool Document::fromXml(QDomElement & pe)
{
	qDeleteAll(m_groups);
	m_groups.clear();

	setDocTitle(pe.attribute("title"));
	
	//qDebug() << "Document::fromXml: title:"<<docTitle();
	// for each slide
	for (QDomElement element = pe.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) 
	{
		SlideGroup *g = new SlideGroup();
		addGroup(g);
		//qDebug("Document::fromXml: Loaded new group");
		
		// restore the item, and delete it if something goes wrong
		if (!g->fromXml(element)) 
		{
			qDebug("Document::fromXml: group fromXml failed, removing");
 			removeGroup(g);
			delete g;
			continue;
		}
		
	}
	
	return true;
}

void Document::save(const QString & filename)
{
	QString tmp = filename;
	if(tmp.isEmpty())
		tmp = m_filename;
		
	QDomDocument doc;
	QFile file;
	QTextStream out;
	
	// Open file
	file.setFileName(tmp);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QMessageBox::warning(0, tr("File Error"), tr("Error saving to the file '%1'").arg(tmp));
		throw 0;
		return;
	}
	
	qDebug() << "Document::save: Writing to "<<tmp;
	
	out.setDevice(&file);
	
	// This element contains all the others.
	QDomElement rootElement = doc.createElement("document");
	
	toXml(rootElement);
	
	// Add the root (and all the sub-nodes) to the document
	doc.appendChild(rootElement);
	
	//Add at the begining : <?xml version="1.0" ?>
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
	doc.insertBefore(noeud,doc.firstChild());
	//save in the file (4 spaces indent)
	doc.save(out, 4);
	file.close();
}

void Document::toXml(QDomElement & pe) const
{
	pe.setAttribute("title",docTitle());
	
	QDomDocument doc = pe.ownerDocument();
	
	foreach (SlideGroup * g, m_groups) 
	{
		QDomElement element = doc.createElement("group");
		pe.appendChild(element);
		g->toXml(element);	
	}
}
