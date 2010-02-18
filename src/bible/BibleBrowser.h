#ifndef BIBLEBROWSER_H
#define BIBLEBROWSER_H

#include <QWidget>
#include <QCloseEvent>

class QByteArray;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QLabel;
class QComboBox;

class TextBoxItem;
class SlideGroup;
class Slide;
	
class BibleGatewayConnector;
#include "BibleModel.h"

class BibleBrowser : public QWidget
{
	Q_OBJECT
	
public:
	BibleBrowser(QWidget *parent=0);
	~BibleBrowser();
	
	bool showVerseNumbers() 	{ return m_showVerseNumbers; }
// 	bool showFullRefEachSlide() 	{ return m_showFullRefEachSlide; }
	bool showFullRefAtStart() 	{ return m_showFullRefAtStart; }
	bool showFullRefAtEnd() 	{ return m_showFullRefAtEnd; }
	bool showResponsiveReadingLabels() { return m_showResponsiveReadingLabels; }
	
signals:
	void verseListSelected(const BibleVerseList &);
	
public slots:
	void loadVerses(const QString& ref="");
	void clearSearch() { loadVerses(); }
	
	void setShowVerseNumbers(bool);
// 	void setShowFullRefEachSlide(bool);
	void setShowFullRefAtStart(bool);
	void setShowFullRefAtEnd(bool);
	void setShowResponsiveReadingLabels(bool);

	void addVersesToDocument();
	void sendVersesLive();
	
	void addAnotherVerse();
	void getChapter();
	void prevChapter();
	void nextChapter();

protected slots:
	void searchReturnPressed();
	void searchTextChanged(const QString&);
	void referenceAvailable(const BibleVerseRef&, const BibleVerseList&);
	
	void saveSettings();
	
protected:
	SlideGroup * createSlideGroup();
	
	void closeEvent(QCloseEvent *);
	
	Slide * addSlide(SlideGroup *, TextBoxItem *, int height, const QSize & fitSize, const QString & plain);
	
	void setupUI();
	void setupVersionCombo();

	QWidget	      * m_searchBase;
	QComboBox     * m_versionCombo;
	QLineEdit     * m_search;
	QPushButton   * m_liveBtn;
	QPushButton   * m_addBtn;
	QLabel        * m_spinnerLabel;
	QWidget       * m_refBase;
	QLabel        * m_referenceLabel;
	QPushButton   * m_addAnotherVerseBtn;
	QPushButton   * m_getChapterBtn;
	QPushButton   * m_prevChapterBtn;
	QPushButton   * m_nextChapterBtn;
	QTextEdit     * m_preview;
	BibleGatewayConnector * m_bible;
	BibleVerseList	m_currentList;
	BibleVerseRef	m_currentRef;
	
	bool m_showVerseNumbers;
// 	bool m_showFullRefEachSlide;
	bool m_showFullRefAtStart;
	bool m_showFullRefAtEnd;
	bool m_showResponsiveReadingLabels;
	
	QList<SlideGroup*> m_generatedGroups;
	void cullGeneratedGroups();
	bool isGroupLive(SlideGroup*);
	
	bool m_attemptAutoLive;
	bool isLastGeneratedGroupStillLive();
};


#endif
