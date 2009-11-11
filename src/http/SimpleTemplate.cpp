#include "SimpleTemplate.h"

#include <QTextStream>

#include <QFile>
#include <QFileInfo>

SimpleTemplate::SimpleTemplate(const QString& file, bool isRawTemplateData)
{
	if(isRawTemplateData)
	{
		m_data = file;
	}
	else
	{
		QFile fileHandle(file);
		if(!fileHandle.open(QIODevice::ReadOnly))
		{
			qWarning("SimpleTemplate: Unable to open '%s' for reading", qPrintable(file));
		}
		else
		{
			QTextStream os(&fileHandle);
			m_data = os.readAll();
			fileHandle.close();
		}
	}
}

void SimpleTemplate::param(const QString &param, const QString &value)
{
	m_params[param] = value;
}

QString SimpleTemplate::toString()
{
	QString data = m_data;
	foreach(QString key, m_params.keys())
		data.replace(QString("%%%1%%").arg(key), m_params.value(key));
	return data;
}

