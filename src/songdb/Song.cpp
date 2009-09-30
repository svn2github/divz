#include "Song.h"

Song::Song() :
	m_songId(0)
	, m_title("")
	, m_tags("")
	, m_number("")
	, m_text("")

{
}

SONG_PROPSET(Song, SongId,		quint32,	songId);
SONG_PROPSET(Song, Title,		QString,	title);
SONG_PROPSET(Song, Tags,		QString,	tags);
SONG_PROPSET(Song, Text,		QString,	text);

QStringList Song::tagList() const
{
	static QRegExp tagRegex("\\s*,\\s*")
	return tags().split(tagRegex);
}

void Song::setTagList(const QStringList & list)
{
	setTags(list.join(", "));
}