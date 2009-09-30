#ifndef SONG_H
#define SONG_H

#include <QStringList>

#define SONG_PROPSET(className,setterName,typeName,memberName) void className::set##setterName(typeName value){m_##memberName = value;}
#define SONG_PROPDEF(setterName,typeName,memberName) void set##setterName(typeName value); typeName memberName() const { return m_##memberName; }

class Song : public QObject
{
	Q_OBJECT

	Q_PROPERTY(quint32 songId	READ songId		WRITE setSongId);
	Q_PROPERTY(QString title	READ title		WRITE setTitle);
	Q_PROPERTY(QString tags		READ tags		WRITE setTags);
	Q_PROPERTY(QString number	READ number		WRITE setNumber);
	Q_PROPERTY(QString text		READ text		WRITE setText);
	Q_PROPERTY(QString author	READ author		WRITE setAuthor);
	Q_PROPERTY(QString copyright	READ copyright		WRITE setCopyright);
	Q_PROPERTY(QString lastUsed	READ lastUsed		WRITE setLastUsed);

public:
	Song();

	SONG_PROPDEF(SongId,		quint32,	songId);
	SONG_PROPDEF(Title,		QString,	title);
	SONG_PROPDEF(Tags,		QString,	tags);
	SONG_PROPDEF(Text,		QString,	text);
	SONG_PROPDEF(Author,		QString,	author);
	SONG_PROPDEF(Copyright,		QString,	copyright);
	SONG_PROPDEF(LastUsed,		QString,	lastUsed);
	


	QStringList tagList() const;
	void setTagList(const QStringList &);
};


#endif // SONG_H