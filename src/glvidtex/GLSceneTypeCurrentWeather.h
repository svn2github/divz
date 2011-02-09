#ifndef GLSceneTypeCurrentWeather_H
#define GLSceneTypeCurrentWeather_H

#include "GLSceneGroupType.h"

#include <QNetworkReply>

/** \class GLSceneTypeCurrentWeather
	A basic 'current weather conditions' scene type.
	
	Parameters:
		- Location - Can be a ZIP code or a location name (Chicago, IL)
	 
	Fields Required:
		- Location
			- Text
			- Example: Chicago, IL
			- This will be the 'normalized' location returned by the server, not the raw location given as a parameter.
			- Optional
		- Condition
			- Text
			- Example: Partly Cloudy
		- Temp
			- Text
			- Example: 9*F
		- Icon
			- Image
		- Wind
			- Text
			- Example: Wind: SW at 9 mph
			- Optional
*/			


class GLSceneTypeCurrentWeather : public GLSceneType
{
	Q_OBJECT
public:
	GLSceneTypeCurrentWeather(QObject *parent=0);
	virtual ~GLSceneTypeCurrentWeather() {};
	
	virtual QString id()		{ return "e0f44600-aa86-4332-a35f-efeb0088db4a"; }
	virtual QString title()		{ return "Current Weather Conditions"; }
	virtual QString description()	{ return "Displays the current weather conditions (wind, temperature, cloud cover) for a given location."; }
	
	QString location() { return m_params["location"].toString(); }
	
public slots:
	virtual void setLiveStatus (bool flag=true);
	
	/** Overridden to intercept changes to the 'location' parameter. */
	virtual void setParam(QString param, QVariant value);
	
	/** Set the current location to \a local.
		\a local may be any valid location descriptor accepted by Google, 
		such as a zip/postal code, a city/state pair such as "Chicago, IL", etc. */
	void setLocation(const QString& local) { setParam("location", local); }
		
	/** Reload the weather data for the current 'location' parameter. */
	void reloadData();
	
private:
	void requestData(const QString &location);
	void handleNetworkData(QNetworkReply *networkReply);
	void parseData(const QString &data);
	QString extractIcon(const QString &data);
	
private:
	QHash<QString, QString> m_icons;
	
	QTimer m_reloadTimer;
};

#endif
