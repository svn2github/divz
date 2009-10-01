#ifndef OUTPUT_H
#define OUTPUT_H

#include <QRect>
#include <QByteArray>
#include <QObject>
#include <QVariant>

class Output : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool isSystem READ isSystem WRITE setIsSystem);
	Q_PROPERTY(bool isEnabled READ isEnabled WRITE setIsEnabled);
	Q_PROPERTY(QString name READ name WRITE setName);
	Q_PROPERTY(OutputType outputType READ outputType WRITE setOutputType);
	Q_PROPERTY(int screenNum READ screenNum WRITE setScreenNum);
	Q_PROPERTY(QRect customRect READ customRect WRITE setCustomRect);
	Q_PROPERTY(NetworkRole networkRole READ networkRole WRITE setNetworkRole);
	Q_PROPERTY(QString host READ host WRITE setHost);
	Q_PROPERTY(int port READ port WRITE setPort);
	Q_PROPERTY(bool allowMultiple READ allowMultiple WRITE setAllowMultiple);
	Q_PROPERTY(QString tags READ tags WRITE setTags);
public:
	Output();

	bool isSystem() { return m_isSystem; }
	void setIsSystem(bool);

	bool isEnabled() { return m_isEnabled; }
	void setIsEnabled(bool);

	QString name() { return m_name; }
	void setName(QString);

	typedef enum OutputType { Screen, Custom, Network };
	OutputType outputType() { return m_outputType; }
	void setOutputType(OutputType);

	int screenNum() { return m_screenNum; }
	void setScreenNum(int);

	QRect customRect() { return m_customRect; }
	void setCustomRect(QRect);

	typedef enum NetworkRole { Client, Server };
	NetworkRole networkRole() { return m_networkRole; }
	void setNetworkRole(NetworkRole);

	QString host() { return m_host; }
	void setHost(const QString&);

	int port() { return m_port; }
	void setPort(int);

	bool allowMultiple() { return m_allowMultiple; }
	void setAllowMultiple(bool);
	
	QString tags() { return m_tags; }
	void setTags(QString);

	QByteArray toByteArray();
	void fromByteArray(QByteArray);

private:
	bool m_isSystem;
	bool m_isEnabled;
	QString m_name;
	OutputType m_outputType;
	int m_screenNum;
	QRect m_customRect;
	NetworkRole m_networkRole;
	QString m_host;
	int m_port;
	bool m_allowMultiple;
	QString m_tags;

};


#endif
