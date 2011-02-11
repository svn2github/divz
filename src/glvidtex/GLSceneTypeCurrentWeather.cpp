#include "GLSceneTypeCurrentWeather.h"

GLSceneTypeCurrentWeather::GLSceneTypeCurrentWeather(QObject *parent)
	: GLSceneType(parent)
{
	m_fieldInfoList 
		<< FieldInfo("conditions", 
			"Weather Conditions", 
			"Text string describing the current weather conditions, such as 'Partly Cloudy.'", 
			"Text", 
			true)
			
		<< FieldInfo("temp", 
			"Current Temperature", 
			"A short numerical text giving the current temperature in farenheit, for example: 9*F", 
			"Text", 
			true)
			
		<< FieldInfo("winds", 
			"Wind Conditions", 
			"A short string giving the current wind conditions, such as 'Wind: SW at 9 mph'", 
			"Text", 
			true)
			
		<< FieldInfo("icon", 
			"Weather Icon", 
			"A scalable vector graphic (SVG) icon representing the current weather conditions.", 
			"Svg", 
			true)
		
		<< FieldInfo("location", 
			"Location", 
			"The normalized location description given by the server, such as 'Chicago, IL'", 
			"Text", 
			false)
		;
		
	m_paramInfoList
		<< ParameterInfo("location",
			"Location",
			"Can be a ZIP code or a location name (Chicago, IL)",
			QVariant::String,
			true,
			SLOT(setLocation(const QString&)))
			
		<< ParameterInfo("updateTime",
			"Update Time",
			"Time in seconds to wait between updates",
			QVariant::Int,
			true,
			SLOT(setUpdateTime(int)));
			
	connect(&m_reloadTimer, SIGNAL(timeout()), this, SLOT(reloadData()));
	m_reloadTimer.setInterval(1 * 60 * 1000); // every 1 minute
			
}

void GLSceneTypeCurrentWeather::setLiveStatus(bool flag)
{
	GLSceneType::setLiveStatus(flag);
	
	if(flag)
	{
		m_reloadTimer.start();
		applyFieldData();
	}
	else
	{
		m_reloadTimer.stop();
	}
}

void GLSceneTypeCurrentWeather::setParam(QString param, QVariant value)
{
	GLSceneType::setParam(param, value);
	
	if(param == "location")
		reloadData();
	else
	if(param == "updateTime")
		m_reloadTimer.setInterval(value.toInt() * 1000); 
}

void GLSceneTypeCurrentWeather::reloadData()
{
	requestData(location());
}

void GLSceneTypeCurrentWeather::requestData(const QString &location) 
{
	QUrl url("http://www.google.com/ig/api");
	url.addEncodedQueryItem("hl", "en");
	url.addEncodedQueryItem("weather", QUrl::toPercentEncoding(location));
	
	qDebug() << "GLSceneTypeCurrentWeather::requestData("<<location<<"): url:"<<url;

	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(handleNetworkData(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void GLSceneTypeCurrentWeather::handleNetworkData(QNetworkReply *networkReply) 
{
	QUrl url = networkReply->url();
	if (!networkReply->error())
		parseData(QString::fromUtf8(networkReply->readAll()));
	networkReply->deleteLater();
	networkReply->manager()->deleteLater();
}

QString GLSceneTypeCurrentWeather::extractIcon(const QString &data) 
{
	if (m_icons.isEmpty()) 
	{
		m_icons["mostly_cloudy"]    = "weather-few-clouds";
		m_icons["cloudy"]           = "weather-overcast";
		m_icons["mostly_sunny"]     = "weather-sunny-very-few-clouds";
		m_icons["partly_cloudy"]    = "weather-sunny-very-few-clouds";
		m_icons["sunny"]            = "weather-sunny";
		m_icons["flurries"]         = "weather-snow";
		m_icons["fog"]              = "weather-fog";
		m_icons["haze"]             = "weather-haze";
		m_icons["icy"]              = "weather-icy";
		m_icons["sleet"]            = "weather-sleet";
		m_icons["chance_of_sleet"]  = "weather-sleet";
		m_icons["snow"]             = "weather-snow";
		m_icons["chance_of_snow"]   = "weather-snow";
		m_icons["mist"]             = "weather-showers";
		m_icons["rain"]             = "weather-showers";
		m_icons["chance_of_rain"]   = "weather-showers";
		m_icons["storm"]            = "weather-storm";
		m_icons["chance_of_storm"]  = "weather-storm";
		m_icons["thunderstorm"]     = "weather-thundershower";
		m_icons["chance_of_tstorm"] = "weather-thundershower";
		
		m_icons["night:sunny"]            = "weather-night-waxingcrescent-clear";
		m_icons["night:mostly_cloudy"]    = "weather-night-waxingcrescent-partially-cloudy";
		m_icons["night:cloudy"]           = "weather-night-waxingcrescent-cloudy";
		m_icons["night:mostly_sunny"]     = "weather-night-waxingcrescent-partially-cloudy";
		m_icons["night:partly_cloudy"]    = "weather-night-waxingcrescent-partially-cloudy";
		m_icons["night:flurries"]         = "weather-night-waxingcrescent-snow";
		m_icons["night:fog"]              = "weather-night-foggy";
// 		m_icons["haze"]             = "weather-haze";
// 		m_icons["icy"]              = "weather-icy";
// 		m_icons["sleet"]            = "weather-sleet";
// 		m_icons["chance_of_sleet"]  = "weather-sleet";
		m_icons["night:snow"]             = "weather-night-waxingcrescent-snow";
		m_icons["night:chance_of_snow"]   = "weather-night-waxingcrescent-snow";
// 		m_icons["mist"]             = "weather-showers";
// 		m_icons["rain"]             = "weather-showers";
// 		m_icons["chance_of_rain"]   = "weather-showers";
		m_icons["night:storm"]            = "weather-night-waxingcrescent-rain";
		m_icons["night:chance_of_storm"]  = "weather-night-waxingcrescent-rain";
		m_icons["night:thunderstorm"]     = "weather-night-waxingcrescent-thunderstorms";
		m_icons["night:chance_of_tstorm"] = "weather-night-waxingcrescent-thunderstorms";
	}
	QRegExp regex("([\\w]+).gif$");
	if (regex.indexIn(data) != -1) 
	{
		QString i = regex.cap();
		i = i.left(i.length() - 4);
		
		QTime time = QTime::currentTime();
		
		// The *right* way todo this in the future would be to check the LOCAL sunrise/sunset times
		// and then compare the current min/hour to the local sunrise/sunset!
		// For now, this is Good Enough.
		bool isNight = time.hour() <= 6 ||
		               time.hour() >= 6 + 12;
		
		QString name;
		if(isNight)
			name = m_icons.value("night:" + i);
		
		if(!isNight || name.isEmpty())
			name = m_icons.value(i);
		
		if (!name.isEmpty()) 
		{
			name.prepend("images/icons/");
			name.append(".svg");
			return name;
		}
	}
	return QString();
}

// static QString toCelcius(QString t, QString unit) 
// {
// 	bool ok = false;
// 	int degree = t.toInt(&ok);
// 	if (!ok)
// 		return QString();
// 	if (unit != "SI")
// 		degree = ((degree - 32) * 5 + 8)/ 9;
// 	return QString::number(degree) + QChar(176);
// }


#define GET_DATA_ATTR xml.attributes().value("data").toString()

void GLSceneTypeCurrentWeather::parseData(const QString &data) 
{
	qDebug() << "GLSceneTypeCurrentWeather::parseData()";
	QString unitSystem;

	QXmlStreamReader xml(data);
	while (!xml.atEnd()) 
	{
		xml.readNext();
		if (xml.tokenType() == QXmlStreamReader::StartElement) 
		{
			if (xml.name() == "city") 
			{
				QString city = GET_DATA_ATTR;
				setField("location", city); 
			}
			if (xml.name() == "unit_system")
				unitSystem = xml.attributes().value("data").toString();
			
			// Parse current weather conditions
			if (xml.name() == "current_conditions") 
			{
				while (!xml.atEnd()) 
				{
					xml.readNext();
					if (xml.name() == "current_conditions")
						break;
						
					if (xml.tokenType() == QXmlStreamReader::StartElement) 
					{
						if (xml.name() == "condition") 
						{
							setField("conditions", GET_DATA_ATTR);
						}
						if (xml.name() == "icon") 
						{
							QString name = extractIcon(GET_DATA_ATTR);
							if (!name.isEmpty()) 
								setField("icon", name);
						}
						if (xml.name() == "temp_f") 
						{
							QString s = GET_DATA_ATTR + QChar(176);
							setField("temp", s);
						}
						if (xml.name() == "wind_condition") 
						{
							setField("winds", GET_DATA_ATTR);
						}
					}
				}
			}
// 			// Parse and collect the forecast conditions
// 			if (xml.name() == "forecast_conditions") 
// 			{
// 				QGraphicsTextItem *dayItem  = 0;
// 				QGraphicsSvgItem *statusItem = 0;
// 				QString lowT, highT;
// 				while (!xml.atEnd()) 
// 				{
// 					xml.readNext();
// 					if (xml.name() == "forecast_conditions") 
// 					{
// 						if (dayItem && 
// 						    statusItem &&
// 						    !lowT.isEmpty() && 
// 						    !highT.isEmpty()) 
// 						{
// 							m_dayItems << dayItem;
// 							m_conditionItems << statusItem;
// 							
// 							QString txt = highT + '/' + lowT;
// 							QGraphicsTextItem* rangeItem;
// 							rangeItem = m_scene.addText(txt);
// 							rangeItem->setDefaultTextColor(textColor);
// 							m_rangeItems << rangeItem;
// 							
// 							QGraphicsRectItem *box;
// 							box = m_scene.addRect(0, 0, 10, 10);
// 							box->setPen(Qt::NoPen);
// 							box->setBrush(Qt::NoBrush);
// 							m_forecastItems << box;
// 							
// 							dayItem->setParentItem(box);
// 							statusItem->setParentItem(box);
// 							rangeItem->setParentItem(box);
// 						} 
// 						else 
// 						{
// 							delete dayItem;
// 							delete statusItem;
// 						}
// 						break;
// 					}
// 					if (xml.tokenType() == QXmlStreamReader::StartElement) 
// 					{
// 						if (xml.name() == "day_of_week") 
// 						{
// 							QString s = GET_DATA_ATTR;
// 							dayItem = m_scene.addText(s.left(3));
// 							dayItem->setDefaultTextColor(textColor);
// 						}
// 						if (xml.name() == "icon") 
// 						{
// 							QString name = extractIcon(GET_DATA_ATTR);
// 							if (!name.isEmpty()) 
// 							{
// 								statusItem = new QGraphicsSvgItem(name);
// 								m_scene.addItem(statusItem);
// 							}
// 						}
// 						if (xml.name() == "low")
// 							lowT = toCelcius(GET_DATA_ATTR, unitSystem);
// 						if (xml.name() == "high")
// 							highT = toCelcius(GET_DATA_ATTR, unitSystem);
// 					}
// 				}
// 			}
		}
	}
}
