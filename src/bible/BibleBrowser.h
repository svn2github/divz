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

protected slots:
	void searchReturnPressed();
	void searchTextChanged(const QString&);
	void referenceAvailable(const BibleVerseRef&, const BibleVerseList&);
	
	void createSlideGroup();
	void saveSettings();
	
protected:
	void closeEvent(QCloseEvent *);
	
	Slide * addSlide(SlideGroup *, TextBoxItem *, int height, const QSize & fitSize, const QString & plain);
	
	void setupUI();
	void setupVersionCombo();

	QWidget	      * m_searchBase;
	QComboBox     * m_versionCombo;
	QLineEdit     * m_search;
	QPushButton   * m_addBtn;
	QLabel        * m_spinnerLabel;
	QTextEdit     * m_preview;
	BibleGatewayConnector * m_bible;
	BibleVerseList	m_currentList;
	BibleVerseRef	m_currentRef;
	
	bool m_showVerseNumbers;
// 	bool m_showFullRefEachSlide;
	bool m_showFullRefAtStart;
	bool m_showFullRefAtEnd;
	bool m_showResponsiveReadingLabels;
};


#endif
