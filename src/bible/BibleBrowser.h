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

	void addVersesToDocument();
	void sendVersesLive();
	
	void addPrevVerse();
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
	
	void showConfigMenu();
	
protected:
	SlideGroup * createSlideGroup(SlideGroup *templateGroup, bool allowAppend=false);
	Slide *getTemplateSlide(SlideGroup *templateGroup, int slide=0);
	SlideGroup *getTemplate();
	
	void closeEvent(QCloseEvent *);
	
	void setupTextBox(TextBoxItem *);
	Slide * addSlide(SlideGroup *, TextBoxItem *, int height, const QRect & fitRect, const QString & plain);
	
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
	
	QList<SlideGroup*> m_generatedGroups;
	void cullGeneratedGroups();
	bool isGroupLive(SlideGroup*);
	
	bool m_attemptAutoLive;
	bool isLastGeneratedGroupStillLive();
	
	SlideGroup	* m_template;
	TemplateSelectorWidget * m_tmplWidget;
};


#endif
