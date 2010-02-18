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

class TemplateSelectorWidget;

#include "BibleModel.h"

class BibleBrowser : public QWidget
{
	Q_OBJECT
	
public:
	BibleBrowser(QWidget *parent=0);
	~BibleBrowser();
	
	bool showEachVerseOnSeperateSlide() { return m_showEachVerseOnSeperateSlide; }
	bool showVerseNumbers() 	{ return m_showVerseNumbers; }
	
	bool showRefTopEachSlide() 	{ return m_showRefTopEachSlide; }
 	bool showRefBottomEachSlide()	{ return m_showRefBottomEachSlide; }
 	bool showFullRefAtFirstTop()	{ return m_showFullRefAtFirstTop; }
 	bool showFullRefAtBottomLast()	{ return m_showFullRefAtBottomLast; }

	bool showFullRefAtStart() 	{ return m_showFullRefAtStart; }
	bool showFullRefAtEnd() 	{ return m_showFullRefAtEnd; }
	
	bool showResponsiveReadingLabels() { return m_showResponsiveReadingLabels; }
	
 	
	
signals:
	void verseListSelected(const BibleVerseList &);
	
public slots:
	void loadVerses(const QString& ref="");
	void clearSearch() { loadVerses(); }
	
	void setShowEachVerseOnSeperateSlide(bool);
	
	void setShowVerseNumbers(bool);
	
	void setShowRefTopEachSlide(bool); 
 	void setShowRefBottomEachSlide(bool);
 	void setShowFullRefAtFirstTop(bool);
 	void setShowFullRefAtBottomLast(bool);


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
	
	void templateChanged(SlideGroup*);
	
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
	
	bool m_showEachVerseOnSeperateSlide;
	
	bool m_showVerseNumbers;
	
 	bool m_showRefTopEachSlide;
 	bool m_showRefBottomEachSlide;
 	bool m_showFullRefAtFirstTop;
 	bool m_showFullRefAtBottomLast;
 	
 	bool m_showFullRefAtStart;
	bool m_showFullRefAtEnd;
	
	bool m_showResponsiveReadingLabels;
	
	QList<SlideGroup*> m_generatedGroups;
	void cullGeneratedGroups();
	bool isGroupLive(SlideGroup*);
	
	bool m_attemptAutoLive;
	bool isLastGeneratedGroupStillLive();
	
	SlideGroup	* m_template;
	TemplateSelectorWidget * m_tmplWidget;
};


#endif
