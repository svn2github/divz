#include "ImageRecord.h"
//#include "ImageRecordListModel.h"
#include <QDebug>

bool ImageRecord::m_dbIsOpen = false;
QSqlDatabase ImageRecord::m_db;

void ImageRecord::initDatabase()
{
	//QFile file(IMAGEDB_FILE);
	
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(IMAGEDB_FILE);
	bool ok = m_db.open();
 	//qDebug()<<"Ok?"<<ok;
	m_dbIsOpen = true;
	
	QStringList tables = m_db.tables();
	if(!tables.contains(IMAGEDB_TABLE))
	{
		QStringList fieldDefs;
		
		const QMetaObject & metaobject = ImageRecord::staticMetaObject;
		int count = metaobject.propertyCount();
		
		for (int i=0; i<count; ++i)
		{
			QMetaProperty metaproperty = metaobject.property(i);
			QString name = QString(metaproperty.name());
			if(name.startsWith("db_"))
			{
				QStringList defn;
				QString field = name.right(name.length() - 3);
				defn << field << " ";
				
				QVariant::Type type = metaproperty.type();
				if(field == IMAGEDB_PRIKEY)
				{
					defn << "integer primary key";
				}
				else
				if(type == QVariant::Int)
				{
					defn << "int(11)";
				}
				else
				if(type == QVariant::String || type == QVariant::DateTime)
				{
					defn << "varchar(255)";
				}
				else
				{
					qDebug() << "ImageRecord::initDatabase(): Unknown field type "<<(int)type<<" for field "<<field;
				}
				
				fieldDefs << defn.join("");
			}
			//else
			//	qDebug() << "ImageRecord::initDatabase(): Not using property "<<name;
		}
		
		qDebug() << "ImageRecord::initDatabase(): Processed "<<count<<" properties";
		
		QString sql = QString("CREATE TABLE %1 (%2)")
				.arg(IMAGEDB_TABLE)
				.arg(fieldDefs.join(", "));
		
		qDebug() << "ImageRecord::initDatabase(): Create SQL: "<<sql;
		
		QSqlQuery query;
		query.prepare(sql);
		query.exec();
		
		if (query.lastError().isValid())
		{
			qDebug() << "ImageRecord::initDatabase(): Error creating database: "<<query.lastError();
		}
		
	}
	
// 	QSqlQuery query;
// 	//query.exec("SELECT title, number FROM songs where recordid=16");
// 	query.prepare("SELECT * FROM songs where number=?");  
// 	query.addBindValue(203);
// 	query.exec();
// 	if (query.lastError().isValid())
// 	{
// 		qDebug() << "initDatabase(): query.lastError();
// 	}
// 	else
// 	{
// 		while (query.next()) 
// 		{
// 			QString title = query.value(0).toString();
// 			int number = query.value(1).toInt();
// 			if(number == 0)
// 				continue;
// 			//check value(1).isNull() before using
// 			qDebug() << title << number;
// 		}
// 	}
}

ImageRecord * ImageRecord::retrieve(int id)
{
	if(!m_dbIsOpen)
		initDatabase();
	QSqlQuery query;
	query.prepare(QString("SELECT * FROM %1 WHERE recordid=?").arg(IMAGEDB_TABLE)); 
	query.addBindValue(id);
	query.exec();
	if (query.lastError().isValid())
	{
		qDebug() << "ImageRecord::retrieve(): "<<query.lastError();
		return 0;
	}
	else
	{
		if(query.size())
		{
			query.next();
			return ImageRecord::fromQuery(query);
		}
		else
		{
			return 0;
		}
	}
}

QList<ImageRecord*> ImageRecord::search(QString text, bool onlyTitle)
{
	if(!m_dbIsOpen)
		initDatabase();
	QSqlQuery query;
	query.prepare(QString("SELECT * FROM %1 WHERE title LIKE :x").arg(IMAGEDB_TABLE) + (onlyTitle ? "" : " OR tags LIKE :y")); 
	query.bindValue(":x",QString("%%1%").arg(text));
	query.exec();
	if(!onlyTitle)
		query.bindValue(":y",QString("%%1%").arg(text));
	if (query.lastError().isValid())
	{
		qDebug() << "ImageRecord::search():"<<query.lastError();
		return QList<ImageRecord*>();
	}
	else
	{
		if(query.size())
		{
			QList<ImageRecord*> list;
			while(query.isValid())
				list << ImageRecord::fromQuery(query);
			return list;
			
		}
		else
		{
			return QList<ImageRecord*>();
		}
	}
	
}

bool ImageRecord::addRecord(ImageRecord* rec)
{
	if(!rec)
	{
		qDebug("ImageRecord::addRecord(): Cannot add a null record.");
		return false;
	}
	
	if(!m_dbIsOpen)
		initDatabase();
	/*
	QSqlQuery query;
	query.prepare(QString("INSERT INTO %1 ( imageid, file, datestamp, title, batchname, tags, description, location, rating, altsizes, colorkeys, softlevel) "
			              "VALUES (:imageid,:file,:datestamp,:title,:batchname,:tags,:description,:location,:rating,:altsizes,:colorkeys,:softlevel)").arg(IMAGEDB_TABLE));
	
	//query.bindValue(":recordid",	rec->recId());
	query.bindValue(":imageid",	rec->imageId());
	query.bindValue(":file",	rec->file());
	query.bindValue(":datestamp",	rec->datestamp());
	query.bindValue(":title",	rec->title());
	query.bindValue(":batchname",	rec->batchName());
	query.bindValue(":tags",	rec->tags());
	query.bindValue(":description",	rec->description());
	query.bindValue(":location",	rec->location());
	query.bindValue(":rating",	rec->rating());
	query.bindValue(":altsizes",	rec->altSizes());
	query.bindValue(":colorkeys",	rec->colorKeys());
	query.bindValue(":softlevel",	rec->softLevel());*/
	
	QStringList fieldNames;
	QStringList placeholderNames;
	QVariantList valueList;
	
	const QMetaObject & metaobject = ImageRecord::staticMetaObject;
	int count = metaobject.propertyCount();
	
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject.property(i);
		QString name = QString(metaproperty.name());
		QVariant value = rec->property(name.toLatin1());
		if(name.startsWith("db_"))
		{
			QString field = name.right(name.length() - 3);
			if(field == IMAGEDB_PRIKEY)
				continue;
				
			fieldNames << field;
			placeholderNames << QString(":%1").arg(field);
			valueList << value;
			//r.setValue(field, value);
		}
	}
	
	QString sql = QString("INSERT INTO %1 ( %2) VALUES (%3)")
		.arg(IMAGEDB_TABLE)
		.arg(fieldNames.join(", "))
		.arg(placeholderNames.join(","));
		
	qDebug() << "ImageRecord::addRecord(): SQL:"<<sql;
	QSqlQuery query;
	query.prepare(sql);
	for(int i=0; i<valueList.size(); i++)
		query.bindValue(placeholderNames.at(i), valueList.at(i));
	
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "ImageRecord::addRecord(): Error adding Record: "<<query.lastError();
		return false;
	}
	else
	{
		QVariant var = query.lastInsertId();
		if(!var.isValid())
		{
			qDebug() << "ImageRecord::addRecord(): Record insert succeeded, but no lastInsertId() available.";
		}
		else
		{
			rec->m_init = true;
			rec->setRecordId(var.toInt());
			rec->m_init = false;
			qDebug() << "ImageRecord::addRecord(): Inserted recordid "<<rec->recordId();
		}
	}

	//ImageRecordListModel::instance()->addRecord(song);

	return true;
}


QSqlDatabase ImageRecord::db()
{
	if(!m_dbIsOpen)
		initDatabase();
	return m_db;
}

void ImageRecord::deleteRecord(ImageRecord* rec, bool deletePtr)
{
	if(!m_dbIsOpen)
		initDatabase();
		
	qDebug() << "ImageRecord::deleteRecord(): Deleting recordid:"<<rec->recordId();
	
	QSqlQuery query;
	query.prepare(QString("DELETE FROM %1 WHERE recordid=?").arg(IMAGEDB_TABLE)); 
	query.addBindValue(rec->recordId());
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "ImageRecord::deleteRecord():"<<query.lastError();
		return;
	}
	else
	{
		//ImageRecordListModel::instance()->removeRecord(rec);	
		if(deletePtr)
		{
			delete rec;
			rec = 0;
		}
	}
}

ImageRecord * ImageRecord::fromQuery(QSqlQuery q)
{
	return fromSqlRecord(q.record());
}

ImageRecord * ImageRecord::fromSqlRecord(QSqlRecord r)
{
	// TODO this is a memory leack unless the caller deletes this
	ImageRecord * rec = new ImageRecord();
	//qDebug()<<"fromSqlRecord:"<<r<<", isEmpty? "<<r.isEmpty();
	//qDebug()<<"fromSqlRecord: title:"<<q.value(r.indexOf("title"));
	rec->m_init = true;
	
	
	const QMetaObject & metaobject = ImageRecord::staticMetaObject;
	int count = metaobject.propertyCount();
	
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject.property(i);
		QString name = QString(metaproperty.name());
		if(name.startsWith("db_"))
		{
			QString field = name.right(name.length() - 3);
			QVariant value = r.value(field);
			rec->setProperty(name.toLatin1(),value);
		}
	}
	
	rec->m_init = false;
	return rec;
}

QSqlRecord ImageRecord::toSqlRecord()
{
	QSqlRecord r;
	
	const QMetaObject & metaobject = ImageRecord::staticMetaObject;
	int count = metaobject.propertyCount();
	
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject.property(i);
		QString name = QString(metaproperty.name());
		QVariant value = property(name.toLatin1());
		if(name.startsWith("db_"))
		{
			QString field = name.right(name.length() - 3);
			r.setValue(field, value);
		}
	}
	
	return r;
}



QString ImageRecord::toString() const
{
	QStringList stringList;
	
	const QMetaObject & metaobject = ImageRecord::staticMetaObject;
	int count = metaobject.propertyCount();
	
	QString ptr = QString("").sprintf("%p",(void*)this);
	
	stringList << metaobject.className() << "[" << ptr << "](";
	
	
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject.property(i);
		QString name = QString(metaproperty.name());
		QVariant value = property(name.toLatin1());
		if(name.startsWith("db_"))
		{
			QString field = name.right(name.length() - 3);
			stringList << field << "='"<<value.toString()<<"'";
		}
	}
	
	stringList << ")";
	
	return stringList.join("");
}


QDebug operator<<(QDebug dbg, const ImageRecord &rec)
{
	dbg.space() << qPrintable(rec.toString());
	return dbg.space();
}

QDebug operator<<(QDebug dbg, const ImageRecord *rec)
{
	if(!rec)
	{
		dbg.space() << "ImageRecord(0x0)";
		return dbg.space();
	}
		
	dbg.space() << qPrintable(rec->toString());
	return dbg.space();
}

ImageRecord::ImageRecord(QString file, QObject *parent)
	: QObject(parent)
	, m_recordId(-1)
	, m_imageId("")
	, m_file("")
	, m_datestamp()
	, m_title("")
	, m_batchName("")
	, m_tags("")
	, m_description("")
	, m_location("")
	, m_rating(-1)
	, m_altSizes("")
	, m_colorKeys("")
	, m_softLevel(-1)
	, m_init(false)
{

}

QStringList ImageRecord::tagList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return tags().split(tagRegex);
}

void ImageRecord::setTagList(const QStringList & list)
{
	setTags(list.join(", "));
}

QStringList ImageRecord::altSizeList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return altSizes().split(tagRegex);
}

void ImageRecord::setAltSizeList(const QStringList & list)
{
	setAltSizes(list.join(", "));
}


namespace ImageRecordUtils
{
	static inline QString qcolorToString(const QColor & color)
	{
		//return QString::number((unsigned int)color.rgba());
		
		QStringList rgba;
		rgba << QString::number(color.red());
		rgba << QString::number(color.green());
		rgba << QString::number(color.blue());
		rgba << QString::number(color.alpha());
		return rgba.join(":");
	
	}
	
	static inline QColor qcolorFromString(const QString & string)
	{
		if(string.indexOf(":") < 0)
			return QColor(string.toUInt());
		
		QStringList rgba = string.split(":");
		
		return QColor(  rgba[0].toInt(), // r
				rgba[1].toInt(), // g 
				rgba[2].toInt(), // b
				rgba[3].toInt()); // a
	}
};


QList<QColor> ImageRecord::colorKeyList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	QStringList rgbaList = colorKeys().split(tagRegex);
	QList<QColor> colorList;
	foreach(QString rgba, rgbaList)
		colorList << ImageRecordUtils::qcolorFromString(rgba);
	return colorList;
}

void ImageRecord::setColorKeyList(const QList<QColor> & list)
{
	QStringList rgbaList;
	foreach(QColor color, list)
		rgbaList << ImageRecordUtils::qcolorToString(color);
		
	setColorKeys(rgbaList.join(", "));
}

bool ImageRecord::updateDb(QString memberName, QVariant v, QString field)
{
	if(m_init)
		return true;
		
	if(!m_dbIsOpen)
		initDatabase();
	
	if(field.isEmpty())
		field = memberName;
	
	QSqlQuery query;
	QString sql = QString("UPDATE %1 SET %2 = ? WHERE recordid= ?").arg(IMAGEDB_TABLE).arg(field);
	qDebug() << "ImageRecord::updateDb():"<<sql<<", value:"<<v<<", recordid:"<<m_recordId;
	query.prepare(sql); 
	query.addBindValue(v);
	query.addBindValue(m_recordId);
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "updateDb():"<<query.lastError();
		return false;
	}
	else
	{
		return true;
	}
}
