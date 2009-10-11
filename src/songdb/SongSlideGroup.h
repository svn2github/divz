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
	GroupType groupType() const { return SlideGroup::Song; }	
	
	SongRecord * song() { return m_song; }
	void setSong(SongRecord*);
	
	QString text() { return m_text; }
	void setText(QString);
	
	bool isTextDiffFromDb() { return m_isTextDiffFromDb; }
	
	inline SlideGroup * slideTemplates() { return m_slideTemplates; }
	void setSlideTemplates(SlideGroup*);
	
	SlideGroup * createDefaultTemplates();
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;

protected:
	typedef enum SongTextFilter { Standard, AllowRear, AllowAll };
	void textToSlides(SongTextFilter filter = Standard);
	void removeAllSlides();
	
protected slots:
	void aspectRatioChanged(double x);

private:
	SongRecord * m_song;
	QString m_text;
	bool m_isTextDiffFromDb;
	
	SlideGroup *m_slideTemplates;


};


#endif
