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
class QMenu;

class TextBoxItem;
class SlideGroup;
class Slide;
	
class BibleGatewayConnector;

class TemplateSelectorWidget;

#include "BibleModel.h"

class BibleBrowser : public QWidget
{
	Q_OBJECT
	
	Q_PROPERTY(bool showEachVerseOnSeperateSlide READ showEachVerseOnSeperateSlide WRITE setShowEachVerseOnSeperateSlide);
	Q_PROPERTY(bool showVerseNumbers READ showVerseNumbers WRITE setShowVerseNumbers);
	
	Q_PROPERTY(bool showFullRefTopEachSlide READ showFullRefTopEachSlide WRITE setShowFullRefTopEachSlide);
 	Q_PROPERTY(bool showFullRefBottomEachSlide READ showFullRefBottomEachSlide WRITE setShowFullRefBottomEachSlide);
 	Q_PROPERTY(bool showFullRefAtFirstTop READ showFullRefAtFirstTop WRITE setShowFullRefAtFirstTop);
 	Q_PROPERTY(bool showFullRefAtBottomLast READ showFullRefAtBottomLast WRITE setShowFullRefAtBottomLast);

	Q_PROPERTY(bool showFullRefAtStart READ showFullRefAtStart WRITE setShowFullRefAtStart);
	Q_PROPERTY(bool showFullRefAtEnd READ showFullRefAtEnd WRITE setShowFullRefAtEnd);
	
	Q_PROPERTY(bool showResponsiveReadingLabels READ showResponsiveReadingLabels WRITE setShowResponsiveReadingLabels);
	
	Q_PROPERTY(bool appendToExistingGroup READ appendToExistingGroup WRITE setAppendToExistingGroup);
	
	Q_PROPERTY(bool allowSplitVersesInAltGroups READ allowSplitVersesInAltGroups WRITE setAllowSplitVersesInAltGroups);
	
	
public:
	BibleBrowser(QWidget *parent=0);
	~BibleBrowser();
	
	bool showEachVerseOnSeperateSlide() { return m_showEachVerseOnSeperateSlide; }
	bool showVerseNumbers() 	{ return m_showVerseNumbers; }
	
	bool showFullRefTopEachSlide() 	{ return m_showFullRefTopEachSlide; }
 	bool showFullRefBottomEachSlide() { return m_showFullRefBottomEachSlide; }
 	bool showFullRefAtFirstTop()	{ return m_showFullRefAtFirstTop; }
 	bool showFullRefAtBottomLast()	{ return m_showFullRefAtBottomLast; }

	bool showFullRefAtStart() 	{ return m_showFullRefAtStart; }
	bool showFullRefAtEnd() 	{ return m_showFullRefAtEnd; }
	
	bool showResponsiveReadingLabels() { return m_showResponsiveReadingLabels; }
	
	bool appendToExistingGroup() { return m_appendToExistingGroup; }
	
	bool allowSplitVersesInAltGroups() { return m_allowSplitVersesInAltGroups; }
	
 	
	
signals:
	void verseListSelected(const BibleVerseList &);
	
public slots:
	void loadVerses(const QString& ref="");
	void clearSearch() { loadVerses(); }
	
	void setShowEachVerseOnSeperateSlide(bool);
	
	void setShowVerseNumbers(bool);
	
	void setShowFullRefTopEachSlide(bool); 
 	void setShowFullRefBottomEachSlide(bool);
 	void setShowFullRefAtFirstTop(bool);
 	void setShowFullRefAtBottomLast(bool);


	void setShowFullRefAtStart(bool);
	void setShowFullRefAtEnd(bool);
	void setShowResponsiveReadingLabels(bool);
	
 	void setAppendToExistingGroup(bool);
 	
 	void setAllowSplitVersesInAltGroups(bool);

	void addVersesToDocument();
	void sendVersesLive();
	
	void addPrevVerse();
	void addAnotherVerse();
	void getChapter();
	void prevChapter();
	void nextChapter();
	
	void addVersesToGroup(const QString& verses, SlideGroup *destination);

protected slots:
	void searchReturnPressed();
	void searchTextChanged(const QString&);
	void referenceAvailable(const BibleVerseRef&, const BibleVerseList&);
	
	void saveSettings();
	
	void templateChanged(SlideGroup*);
	
	void showConfigMenu();
	
protected:
	SlideGroup * createSlideGroup(SlideGroup *templateGroup, bool allowAppend=false);
	Slide *getTemplateSlide(SlideGroup *templateGroup, int slide=0);
	SlideGroup *getTemplate();
	
	void closeEvent(QCloseEvent *);
	
	void setupTextBox(TextBoxItem *);
	Slide * addSlide(SlideGroup *, TextBoxItem *, int height, const QRect & fitRect, const QString & plain);
	Slide * createTitleSlide(SlideGroup *templateGroup, SlideGroup *group, bool isFirst = true);
	void setupOptionalLabels(Slide *currentSlide, int slideNumber);
	
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
	QPushButton   * m_addPrevVerseBtn;
	QPushButton   * m_addAnotherVerseBtn;
	QPushButton   * m_getChapterBtn;
	QPushButton   * m_prevChapterBtn;
	QPushButton   * m_nextChapterBtn;
	QTextEdit     * m_preview;
	BibleGatewayConnector * m_bible;
	BibleVerseList	m_currentList;
	BibleVerseRef	m_currentRef;
	QMenu         * m_configMenu;
	QDialog       * m_configDialog;
	
	bool m_showEachVerseOnSeperateSlide;
	
	bool m_showVerseNumbers;
	
 	bool m_showFullRefTopEachSlide;
 	bool m_showFullRefBottomEachSlide;
 	bool m_showFullRefAtFirstTop;
 	bool m_showFullRefAtBottomLast;
 	
 	bool m_showFullRefAtStart;
	bool m_showFullRefAtEnd;
	
	bool m_showResponsiveReadingLabels;
	
	bool m_appendToExistingGroup;
	
	bool m_allowSplitVersesInAltGroups;
	
	QList<SlideGroup*> m_generatedGroups;
	void cullGeneratedGroups();
	bool isGroupLive(SlideGroup*);
	
	bool m_attemptAutoLive;
	bool isLastGeneratedGroupStillLive();
	
	SlideGroup	* m_template;
	TemplateSelectorWidget * m_tmplWidget;
	
	bool m_lastDownloadAutomated;
	bool m_downloadComplete;
	SlideGroup *m_tempDest; 
};


#endif
