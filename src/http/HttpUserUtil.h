#ifndef HttpUserUtil_H
#define HttpUserUtil_H

#include <QString>
#include <QHash>
#include <QHttpRequestHeader>

class HttpUser
{
public:
	enum UserLevel {
		Guest = 0,
		User,
		Admin
	};
	
	HttpUser(const QString& user = "",
		 const QString& pass = "",
		 UserLevel level = Guest)
		: m_user(user)
		, m_pass(pass)
		, m_level(level)
		{}
		
	QString user() { return m_user; }
	QString pass() { return m_pass; }
	
	void setUser(const QString& user) { m_user = user; }
	void setPass(const QString& pass) { m_pass = pass; }
	
	UserLevel level() { return m_level; }
	void setLevel(UserLevel level) { m_level = level ;}
	
private:
	QString m_user;
	QString m_pass;
	UserLevel m_level;
};

class HttpUserUtil
{
	static HttpUserUtil *m_inst;
	HttpUserUtil();
	
public:
	~HttpUserUtil();
	
	static HttpUserUtil *instance();
	
	QList<HttpUser *> users() { return m_users; }
	
	void addUser(HttpUser *);
	void removeUser(HttpUser *);
	
	HttpUser *getUser(const QString& user);
	bool isValid(const QString& user, const QString& pass);
	
	void setCurrentUser(HttpUser *);
	HttpUser *currentUser() { return m_currentUser; }
	
private:
	void storeUsers();
	void loadUsers();
	
	QList<HttpUser *> m_users;
	QHash<QString, HttpUser*> m_userLookup;
	
	HttpUser *m_currentUser;

};

#endif
