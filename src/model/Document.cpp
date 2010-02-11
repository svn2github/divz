#include "Document.h"
#include "SlideGroup.h"

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

#include "AppSettings.h"

#include "songdb/SongSlideGroup.h"
#include "ppt/PPTSlideGroup.h"

#include <QProgressDialog>
#include <QApplication>

#include <QMutex>
#include <QMutexLocker>
QMutex document_saveMutex;

Document::Document(const QString & s) : m_docTitle(""), m_filename(""), m_aspectRatio(4/3)
{
	double ar = AppSettings::liveAspectRatio();
	if(ar > -1)
		m_aspectRatio = ar;

		
	if(!s.isEmpty())
		load(s);
}

Document::~Document() 
{
	qDeleteAll(m_groups);
}

QList<SlideGroup *> Document::groupList() { return m_groups; }

SlideGroup * Document::at(int x) { return m_groups.at(x); }

SlideGroup * Document::groupById(int id)
{
	foreach(SlideGroup *group, m_groups)
		if(group && group->groupId() == id)
			return group;
	return 0;
}

void Document::setAspectRatio(double f) 
{ 
	if(f<0.1)
		f = 1.0;
	m_aspectRatio = f;
	emit aspectRatioChanged(f);
}

void Document::addGroup(SlideGroup *g)
{
	assert(g != NULL);
	m_groups.append(g);
	if(g->groupNumber()<0)
		g->setGroupNumber(m_groups.size());
	emit slideGroupChanged(g, "add", 0, "", 0, "", "", QVariant());
	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

}

void Document::removeGroup(SlideGroup *g)
{
	assert(g != NULL);
	disconnect(g,0,this,0);
	m_groups.removeAll(g);
	emit slideGroupChanged(g, "remove", 0, "", 0, "", "", QVariant());

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

	QProgressDialog progress(QString(tr("Loading %1...")).arg(m_filename),"",0,0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(0); // hide cancel button
	progress.show();
	QApplication::processEvents();
	
	QFileInfo inf(m_filename);
	QString ext = inf.suffix();
	if(ext == "xml" || ext == "dvx" || ext == "dvizx")
	{

		
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
		return;
	}
	else
	//if(ext == "dvz" || ext == "dviz")
	{
		QByteArray array = file.readAll();
		
		QDataStream stream(&array, QIODevice::ReadOnly);
		QVariantMap map;
		stream >> map;
		
		setDocTitle(map["title"].toString());
		setAspectRatio(map["aspect"].toDouble());
		
		m_groups.clear();


		QVariantList items = map["groups"].toList();

		progress.setMaximum(items.size());
		int count = 0;
		foreach(QVariant var, items)
		{
			progress.setValue(count ++);
			QApplication::processEvents();

			QByteArray ba = var.toByteArray();
			SlideGroup * group = SlideGroup::fromByteArray(ba,this);
			qDebug() << "Load Group: nbr:"<<group->groupNumber()<<", name:"<<group->assumedName();
			addGroup(group);
		}
	}
	
	file.close();
	
}

bool Document::fromXml(QDomElement & pe)
{
	qDeleteAll(m_groups);
	m_groups.clear();

	setDocTitle(pe.attribute("title"));
	setAspectRatio(pe.attribute("aspect").toDouble());
	
	//qDebug() << "Document::fromXml: title:"<<docTitle();
	//qDebug() << "Document::fromXml: Loading...";
	// for each slide
	for (QDomElement element = pe.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) 
	{
		SlideGroup *g = 0;
		 
		if (element.tagName() == "song")
		{
			//qDebug("Document::fromXml: Group type: Song");
			g = new SongSlideGroup();
		}
		else
		if (element.tagName() == "powerpoint")
		{
			g = new PPTSlideGroup();
		}
		else
		{
			//qDebug("Document::fromXml: Group type: Generic");
			g = new SlideGroup();
		}
		addGroup(g);
		
		//qDebug("Document::fromXml: Converting group from xml...");
		// restore the item, and delete it if something goes wrong
		if (!g->fromXml(element)) 
		{
			qDebug("Document::fromXml: group fromXml failed, removing");
 			removeGroup(g);
			delete g;
			continue;
		}
		
		if(g->groupNumber()<0)
			g->setGroupNumber(m_groups.size());
			
		//qDebug() << "Document::fromXml: Done loading group, loaded:"<<g->numSlides()<<"slides, title:"<<g->groupTitle();
		
	}
	
	//qDebug() << "Document::fromXml: Done Loading, loaded:"<<numGroups()<<"groups";
	return true;
}

void Document::save(const QString & filename)
{
	QMutexLocker lock(&document_saveMutex);
	
	QString tmp = filename;
	if(tmp.isEmpty())
		tmp = m_filename;
	else
		m_filename = tmp;
		
	QFile file(tmp);

	QSettings settings;
	int maxBackups = settings.value("max-backups","10").toInt();
	int counter = settings.value(QString("filecounts/%1").arg(tmp),"1").toInt();

	//qDebug() << "MaxBackups: "<<maxBackups;

	for(int idx = counter - maxBackups; idx > 0; idx--)
	{
		QString backup = QString("%1.%2").arg(tmp).arg(idx);
		QFile backupFile(backup);
		if(backupFile.exists())
		{
			//qDebug() << "Removing backup "<<backup;
			backupFile.remove();
		}
		else
			break;
	}

	QString backup = QString("%1.%2").arg(tmp).arg(counter);
	file.copy(backup);
	//qDebug() << "Copied "<<tmp<<" to "<<backup;

	counter ++;
	settings.setValue(QString("filecounts/%1").arg(tmp),counter);
	
	// Open file
	if (!file.open(QIODevice::WriteOnly))
	{
		//QMessageBox::warning(0, tr("File Error"), tr("Error saving to the file '%1'").arg(tmp));
		//throw 0;
		return;
	}
	
	
	QFileInfo inf(tmp);
	QString ext = inf.suffix();
	if(ext == "xml" || ext == "dvx" || ext == "dvizx")
	{
		QDomDocument doc;
		QTextStream out;
		
		//qDebug() << "Document::save: Writing to "<<tmp;
		
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
	}
	else
	//if(ext == "dvz" || ext == "dviz")
	{
		QByteArray array;
		QDataStream stream(&array, QIODevice::WriteOnly);
		QVariantMap map;
		
		QVariantList list;
		foreach (SlideGroup * group, m_groups)
			list << group->toByteArray();
		
		map["groups"] = list;
		map["title"] = docTitle();
		map["aspect"] = aspectRatio();
		
		//qDebug() << "SlideGroup::toByteArray(): "<<map;
		stream << map;
		
		file.write(array);
	}
	
	file.close();
	
	//qDebug() << "Document::save: Done writing "<<tmp;
}

void Document::toXml(QDomElement & pe) const
{
	QDomDocument doc = pe.ownerDocument();
	
	pe.setAttribute("title",docTitle());
	pe.setAttribute("aspect",aspectRatio());
	
	foreach (SlideGroup * g, m_groups) 
	{
		QDomElement element = doc.createElement("group");
		pe.appendChild(element);
		g->toXml(element);	
	}
}
