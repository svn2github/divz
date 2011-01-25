#ifndef PlayerCommandLineInterface_H
#define PlayerCommandLineInterface_H

#include <QtGui>

class PlayerConnection;
class GLDrawable;

class PlayerCommandLineInterface : public QObject
{
	Q_OBJECT
	
public:
	PlayerCommandLineInterface();
	~PlayerCommandLineInterface();	
	
	void printHelp();
	void processCmdLine();
	void startConnection();
	
private slots:
	void loginSuccess();
	void loginFailure();
	void playerError(const QString&);
	void propQueryResponse(GLDrawable *drawable, QString propertyName, const QVariant& value);

private:
	void quit();
	
	PlayerConnection *m_con;
	QStringList m_rawArgs;
};

#endif
