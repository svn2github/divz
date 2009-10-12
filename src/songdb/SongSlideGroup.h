#ifndef SONGSLIDEGROUP_H
#define SONGSLIDEGROUP_H

#include "model/SlideGroup.h"
#include "SongRecord.h"

/// \brief: SongSlideGroup represents a single song in the document.
/// SongSlideGroup provides the translation of the text of the lyrics
/// into a collection of slides with appros text boxes, etc.
/// SongSlideGroup also allows the use of a "template" slide group. 
/// Once added to the document, the SongEditorWindow can be used to
/// edit the slideTemplates() which is just another SlideGroup that
/// is merged with the slides created from lyrics in the textToSlides()
/// method. The first slide in the slideTemplates() SlideGroup is used
/// as a master slide and subsequent slides correspond to passages in
/// the lyrics.
class SongSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
	Q_PROPERTY(QString text READ text WRITE setText);
	Q_PROPERTY(QString isTextDiffFromDb READ isTextDiffFromDb);

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
