#ifndef SONGSLIDEGROUP_H
#define SONGSLIDEGROUP_H

#include "model/SlideGroup.h"
#include "SongRecord.h"

class SongSlideGroup : public SlideGroup
{
private:
// 	SongSlideGroup();
	
	Q_OBJECT 
	Q_PROPERTY(QString text READ text WRITE setText);
	Q_PROPERTY(QString isTextDiffFromDb READ isTextDiffFromDb);
// 	
public:
	SongSlideGroup();
	//GroupType groupType() { return SlideGroup::Song; }	
	
	SongRecord * song() { return m_song; }
	void setSong(SongRecord*);
	
	QString text() { return m_text; }
	void setText(QString);
	
	bool isTextDiffFromDb() { return m_isTextDiffFromDb; }
	
	// SlideGroup::
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) ;

protected:
	void textToSlides();
	void removeAllSlides();

private:
	SongRecord * m_song;
	QString m_text;
	bool m_isTextDiffFromDb;


};


#endif
