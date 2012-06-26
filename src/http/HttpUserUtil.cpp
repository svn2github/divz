#include "HttpUserUtil.h"

#define HTTP_USER_STORE "httpusers.ini"

HttpUserUtil *HttpUserUtil::m_inst = 0;

HttpUserUtil *HttpUserUtil::instance()
{
	if(!m_inst)
		m_inst = new HttpUserUtil();
		
	return m_inst;
}

HttpUserUtil::HttpUserUtil()
{
	loadUsers();
}

HttpUserUtil::~HttpUserUtil()
{
	storeUsers();
}

void HttpUserUtil::loadUsers()
{
	QSettings settings(HTTP_USER_STORE, QSettings::IniFormat);
	
	int numUsers = settings.beginReadArray("users");
	
	for(int i=0; i<numUsers; i++)
	{
		settings.setArrayIndex(i);
		
		HttpUser *user = new HttpUser();
		user->setUser(settings.value("user").toString());
		user->setPass(settings.value("pass").toString());
		user->setUserLevel((HttpUser::UserLevel)settings.value("level").toInt());
		addUser(user);
	}
	
	settings.endArray();
}

void HttpUserUtil::storeUsers()
{
	QSettings settings(HTTP_USER_STORE, QSettings::IniFormat);
	
	settings.beginWriteArray("users");
	foreach(HttpUser *user, users())
	{
		settings.setArrayIndex(i);
		settings.setValue("user", user->user());
		settings.setValue("pass", user->pass());
		settings.setValue("level", (int)user->userLevel());
	}
	settings.endArray();
}

void HttpUserUtil::addUser(HttpUser *user)
{
	if(!user)
		return;
		
	if(!m_users.contains(user))
	{
		m_users << user;
		m_userLookup[user->user()] = user;
	}
}

void HttpUserUtil::removeUser(HttpUser *user)
{
	m_users.removeAll(user);
	if(user)
		m_userLookup.remove(user->user());
}

HttpUser *HttpUserUtil::getUser(const QString& user)
{
	if(!m_userLookup.contains(user))
		return 0;
		
	return m_userLookup.value(user);
}

bool HttpUserUtil::isValid(const QString& user, const QString& pass)
{
	HttpUser *data = getUser(user);
	
	if(!data)
		return false;
	
	return data->pass() == pass;
}

HttpUser *HttpUserUtil::getUserFromRequest(const QHttpRequestHeader &request)
{

}

QString HttpUserUtil::getUserCookie(HttpUser *data)
{

}
