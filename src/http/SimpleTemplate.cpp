#include "SimpleTemplate.h"

#include <QTextStream>
#include <QStringList>

#include <QFile>
#include <QFileInfo>

#include <QDebug>

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

void SimpleTemplate::param(const QString &param, const QVariant &value)
{
	m_params[param] = value;
}

void SimpleTemplate::param(const QString &param, const QVariantList &value)
{
	m_params[param] = value;
}

void SimpleTemplate::params(const QVariantMap &map)
{
	foreach(QString param, map.keys())
		m_params[param] = map.value(param);
}

QString SimpleTemplate::toString()
{
	QString data = m_data;
	foreach(QString key, m_params.keys())
	{
		QString valueKey = QString("%%%1%%").arg(key);
		QString loopKey  = QString("%%loop:%1%%").arg(key);
		QString ifKey    = QString("%%if:%1%%").arg(key);
		
		int ifIdx = data.indexOf(ifKey);
		int loopIdx = data.indexOf(loopKey);
		
		if(data.indexOf(valueKey) > -1)
		{
			data.replace(valueKey, m_params.value(key).toString());
		}
		else
		if(ifIdx > -1)
		{
			QString endIf = QString("%%/if:%1%%").arg(key);
			int endIdx = data.indexOf(endIf);
			//if(endIdx < 0)
			//	endIdx = data.length() -1;
			if(endIdx < -1)
			{
				qDebug() << "SimpleTemplate: Unable to find end of IF block: "<<endIf;
				continue;
			}
				
			ifIdx += ifKey.length();
			QString ifBlock = data.mid(ifIdx,endIdx - ifIdx);
			//qDebug() << "SimpleTemplate: ifBlock="<<ifBlock<<", ifIdx:"<<ifIdx<<", endIdx:"<<endIdx<<", endIf:"<<endIf;
			
			bool flag = m_params.value(key).toBool();
			
			QString elseKey = "%%else%%";
			int elseIdx = ifBlock.indexOf(elseKey);
			
			int blockIdx = ifIdx  - ifKey.length();
			int blockLen = endIdx - ifIdx + ifKey.length() + endIf.length();
			if(elseIdx > -1)
			{
				QString blockPre  = ifBlock.left(elseIdx);
				QString blockPost = ifBlock.right(ifBlock.length() - elseIdx - elseKey.length());
				
				data.replace(blockIdx,blockLen, flag ? blockPre : blockPost);
				
			}
			else
			{
				data.replace(blockIdx,blockLen, flag ? ifBlock : "");
			}
			
		}
		else
		if(loopIdx > -1)
		{
			QString endLoop = QString("%%/loop:%1%%").arg(key);
			int endIdx = data.indexOf(endLoop);
			if(endIdx < -1)
			{
				qDebug() << "SimpleTemplate: Unable to find end of LOOP block: "<<endLoop;
				continue;
			}
				
			loopIdx += loopKey.length();
			QString loopBlock = data.mid(loopIdx,endIdx - loopIdx);
			
			SimpleTemplate blockTmpl(loopBlock,true);
			
			QStringList loopOutput;
			
			QVariantList dataList = m_params.value(key).toList();
			foreach(QVariant mapValue, dataList)
			{
				blockTmpl.m_params = mapValue.toMap();
				loopOutput << blockTmpl.toString();
			}
				
			int blockIdx = loopIdx - loopKey.length();
			int blockLen = endIdx  - loopIdx + loopKey.length() + endLoop.length();
			data.replace(blockIdx,blockLen, loopOutput.join(""));
		}
	}
	return data;
}

