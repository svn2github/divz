#ifndef SimpleTemplate_H
#define SimpleTemplate_H

#include <QHash>
#include <QString>

class SimpleTemplate
{
public:
	SimpleTemplate(const QString& file, bool isRawTemplateData = false);
	
	void param(const QString &param, const QString &value);
	
	QString toString();
	
private:
	QHash<QString,QString> m_params;
	
	QString m_data;
	QString m_file;
};


#endif

