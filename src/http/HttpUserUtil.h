#ifndef HttpUserUtil_H
#define HttpUserUtil_H

#include <QString>
#include <QHash>

class HttpUser
{
public:
	HttpUser(const QString& user = "",
		 const QString& pass = "",
		 UserLevel level = Guest)
		: m_user(user)
		, m_pass(pass)
		, m_userLevel(level)
		{}
		
	QString user() { return m_user; }
	Qstring pass() { return m_pass; }
	
	void setUser(const QString& user) { m_user = user; }
	void setPass(const Qstring& pass) { m_pass = pass; }
	
	enum UserLevel {
		Guest = 0,
		User,
		Admin
	};
	
	UserLevel userLevel() { return m_userLevel; }
	void setUserLevel(UserLevel level) { m_userLevel = level ;}
	
private:
	QString m_user;
	Qstring m_pass;
	UserLevel m_userLevel
};

class HttpUserUtil
{
	static HttpUserUtil *m_inst;
	HttpUserUtil();
	
public:
	~HttpUserUtil();
	
	HttpUserUtil *instance();
	
	QList<HttpUser *> users() { return m_users; }
	
	void addUser(HttpUser *);
	void removeUser(HttpUser *);
	
	HttpUser *getUser(const QString& user);
	bool isValid(const QString& user, const QString& pass);
	
	HttpUser *getUserFromRequest(const QHttpRequestHeader &request);
	QString getUserCookie(HttpUser *);

private:
	void storeUsers();
	void loadUsers();
	
	QList<HttpUser *> m_users;
	QHash<QString, HttpUser*> m_userLookup;

};

#endif