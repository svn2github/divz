#include "MimeTypes.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QFileInfo>

// Standard unix format mime.types file
#define DATABASE_FILE ":/data/mime.types"

QHash<QString,QString> MimeTypes::hash;
bool MimeTypes::init = false;

QString MimeTypes::mimeType(const QString& file) { return mimeType(QFileInfo(file)); }
QString MimeTypes::mimeType(const QFileInfo& info)
{
	if(!init)
		initMap();
		
	QString suffix = info.suffix().toLower();
	return  hash.contains(suffix) ?
		hash.value(suffix) :
		"";
}
	
// protected:
void MimeTypes::initMap()
{
	QFile file(DATABASE_FILE);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	
	QRegExp rx("\\s+");
	QTextStream in(&file);
	while (!in.atEnd()) 
	{
		QString line = in.readLine();
		if(line.startsWith("#") || 
		   line.isEmpty())
		   continue;
		
		QStringList parts = line.split(rx);
		if(parts.isEmpty())
			return;
		
		QString mime = parts.takeFirst();
		
		while(!parts.isEmpty())
		{
			QString ext = parts.takeFirst().toLower();
			hash[ext] = mime;
			qDebug("[DEBUG] mime=%s, ext=%s\n", qPrintable(mime), qPrintable(ext));
		}
	}
	
	init = true;
}
