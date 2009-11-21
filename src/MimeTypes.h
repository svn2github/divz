#ifndef MimeTypes_H
#define MimeTypes_H

#include <QFileInfo>
#include <QString>
#include <QHash>

class MimeTypes
{
public:
	static QString mimeType(const QString& file);
	static QString mimeType(const QFileInfo& file);
	
protected:
	static void initMap();
	
	static QHash<QString,QString> hash;
	static bool init;
};

#endif
