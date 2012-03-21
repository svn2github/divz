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
	// Removing the Q_PROP macro defs for these items because I suspect they
	// may be causing errant behaviour for save/restore and centering text.
	//Q_PROPERTY(QString text READ text WRITE setText);
	//Q_PROPERTY(QString isTextDiffFromDb READ isTextDiffFromDb);
	
	// Advisory property for use in the song editor window
	Q_PROPERTY(bool syncToDatabase READ syncToDatabase WRITE setSyncToDatabase);

public:
	SongSlideGroup();
	~SongSlideGroup();
	
	static QString songTagRegexpList() { return "Title|Verse|Chorus|Tag|Bridge|End(ing)?|Intro(duction)?"; }
	
	typedef enum { GroupType = 2 };
	int groupType() const { return GroupType; }	
	
	SongRecord * song() { return m_song; }
	void setSong(SongRecord*);
	
	QString text() { return m_text; }
	void setText(QString);
	
	QStringList arrangement() { return m_arrangement; }
	void     setArrangement(QStringList arr);

	bool isTextDiffFromDb() { return m_isTextDiffFromDb; }
	bool syncToDatabase() { return m_syncToDatabase; }
	
	inline SlideGroup * slideTemplates() { return m_slideTemplates; }
	void setSlideTemplates(SlideGroup*);
	
	SlideGroup * createDefaultTemplates();
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;

	void changeBackground(AbstractVisualItem::FillType fillType, QVariant fillValue, Slide *onlyThisSlide);
	
	// Rearrange a block of text based on the arragment
	static QString rearrange(QString text, QStringList arragement);
	static QStringList findDefaultArragement(QString text);

public slots:
	void setSyncToDatabase(bool flag);
	
protected:
	typedef enum SongTextFilter { Standard, AllowRear, AllowAll };
	void removeAllSlides();
	
	void fromVariantMap(QVariantMap &);
	void   toVariantMap(QVariantMap &) const;
	
protected slots:
	void textToSlides(SongTextFilter filter = Standard);
	void aspectRatioChanged(double x);

private:
	void hitTextToSlides();
	
	SongRecord * m_song;
	QString m_text;
	bool m_isTextDiffFromDb;
	bool m_syncToDatabase;
	
	QStringList m_arrangement;
	
	SlideGroup *m_slideTemplates;
	
	double m_lastAspectRatio;
	QTimer m_textRegenTimer;


};
Q_DECLARE_METATYPE(SongSlideGroup*);

#endif
