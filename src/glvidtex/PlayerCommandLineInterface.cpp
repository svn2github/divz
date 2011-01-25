#include "PlayerCommandLineInterface.h"
#include "PlayerConnection.h"
#include "GLDrawable.h"
#include <QApplication>

PlayerCommandLineInterface::PlayerCommandLineInterface()
{
	m_rawArgs = qApp->arguments();
	
	// first arg is the binary name
	m_rawArgs.takeFirst();
	
	startConnection();	
}

PlayerCommandLineInterface::~PlayerCommandLineInterface()
{
}

void PlayerCommandLineInterface::startConnection()
{
	if(m_rawArgs.isEmpty())
	{
		qDebug() << "PlayerCommandLineInterface::startConnection: No connection info given on command line!";
		printHelp();
		quit();
	}
	
	QString rawCon = m_rawArgs.takeFirst();
	
	QRegExp rx("(?:(\\w+):(\\w+)@)?(\\w+)",Qt::CaseInsensitive);
	rx.indexIn(rawCon);
	
	QString user = rx.cap(1);
	QString pass = rx.cap(2);
	QString host = rx.cap(3);
	
	if(user.isEmpty())
		user = "player";
	if(pass.isEmpty())
		pass = "player";
	if(host.isEmpty() || host == "-")
		host = "localhost";
	
	PlayerConnection *con = new PlayerConnection();
	connect(con, SIGNAL(loginSuccess()), this, SLOT(loginSuccess()));
	connect(con, SIGNAL(loginFailure()), this, SLOT(loginFailure()));
	connect(con, SIGNAL(playerError(QString)), this, SLOT(playerError(QString)));
	
	con->setUser(user);
	con->setPass(pass);
	con->setHost(host);
	con->setAutoReconnect(false);
	
	qDebug() << "PlayerCommandLineInterface::startConnection: Connecting to "<<host<<" with creds:"<<user<<":"<<pass<<", raw:"<<rawCon;
	
	con->connectPlayer(false); // false = dont send defaults
}

void PlayerCommandLineInterface::playerError(const QString& error)
{
	qDebug() << "PlayerCommandLineInterface::playerError: "<<error;
	quit();
}

void PlayerCommandLineInterface::loginSuccess()
{
	qDebug() << "PlayerCommandLineInterface::loginSuccess: Logged in!";
}

void PlayerCommandLineInterface::loginFailure()
{
	qDebug() << "PlayerCommandLineInterface::loginFailure: Failed login";
	quit();
}

void PlayerCommandLineInterface::printHelp()
{
	/// TODO
}

void PlayerCommandLineInterface::processCmdLine()
{
	/// TODO
}

void PlayerCommandLineInterface::propQueryResponse(GLDrawable *drawable, QString propertyName, const QVariant& value)
{
	/// TODO
}

void PlayerCommandLineInterface::quit()
{
	qApp->quit();
	exit(-1);
}
