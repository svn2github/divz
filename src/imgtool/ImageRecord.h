#ifndef ImageRecord_H
#define ImageRecord_H

#include <QtSql>
#include <QSqlTableModel>
#include <QStringList>
#include <QList>

#include "../PropertyUtils.h"

#define IMAGEDB_FILE "images.db"
#define IMAGEDB_TABLE "images"
#define IMAGEDB_PRIKEY "recordid"

class ImageRecord : public QObject
{
	Q_OBJECT
	// CREATE TABLE images (recordid int primary key, imageid varchar(128), file varchar(512), datestamp varchar(255), title varchar(255),
	#include "ImageRecord.props.h"
	
	PROP_DEF_DB_FIELD(int,		recordId,	RecordId,	"recordid");
	PROP_DEF_DB_FIELD(QString,	imageId,	ImageId,	"imageid"); // QUuid generated 128 bit string
	PROP_DEF_DB_FIELD(QString,	file,		File,		"file"); // abs path + filename
	PROP_DEF_DB_FIELD(QDateTime,	datestamp,	Datestamp,	"datestamp"); // date/time the image was taken
	PROP_DEF_DB_FIELD(QString,	title,		title,		"title");
	PROP_DEF_DB_FIELD(QString,	batchName,	BatchName,	"batchname");
	PROP_DEF_DB_FIELD(QString,	tags,		Tags,		"tags");
	PROP_DEF_DB_FIELD(QString,	description,	Description,	"description");
	PROP_DEF_DB_FIELD(QString,	location,	Location,	"location");
	PROP_DEF_DB_FIELD(int,		rating,		Rating,		"rating"); // 0 - 99
	PROP_DEF_DB_FIELD(QString,	altSizes,	AltSizes,	"altsizes"); // csv list of X*Y sizes
	PROP_DEF_DB_FIELD(QString,	colorKeys,	ColorKeys,	"colorkeys"); 
		// 10 item CSV list of HSV values, first item being the HSV key, for the entire image the other 9 represent a 3x3 grid of HSV keys for the image
	PROP_DEF_DB_FIELD(int,		softLevel,	SoftLevel,	"softlevel"); // 0 - 99, 99 being virtually no HSV deviation across the image, with 0 being sharp-as-tacks spikes in change
	
/* Static */
public:
	static ImageRecord * retrieve(int recordid);
	static QList<ImageRecord*> search(QString text, bool onlyTitle = false);

	static bool addRecord(ImageRecord*);
	static void deleteRecord(ImageRecord*, bool deletePointer = true);
	
	static ImageRecord * fromQuery(QSqlQuery);
	static ImageRecord * fromSqlRecord(QSqlRecord);
	
	static QSqlDatabase db();

public:
	ImageRecord(QString file = "", QObject *parent = 0);
	
	// accessors/setters defined by the PROP_DEF_DB_FIELD() macro
	
	QStringList tagList() const;
	void setTagList(const QStringList &);
	
	QStringList altSizeList() const;
	void setAltSizeList(const QStringList &);
	
	QList<QColor> colorKeyList() const;
	void setColorKeyList(const QList<QColor> &);
	
	QString toString() const;
	
signals:
	void recordChanged(ImageRecord*, QString field, QVariant value);

private:
	bool m_init;
	bool updateDb(QString member, QVariant value, QString field = "");
	
	QSqlRecord toSqlRecord();
	
	static void initDatabase();
	static bool m_dbIsOpen;
	static QSqlDatabase m_db;
};

QDebug operator<<(QDebug dbg, const ImageRecord &ref);
QDebug operator<<(QDebug dbg, const ImageRecord *ref);


#endif // ImageRecord_H
