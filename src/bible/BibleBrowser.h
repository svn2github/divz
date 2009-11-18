#ifndef BIBLEBROWSER_H
#define BIBLEBROWSER_H

#include <QWidget>

class QByteArray;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QLabel;
class QComboBox;

class TextBoxItem;
class SlideGroup;
	
class BibleGatewayConnector;
#include "BibleModel.h"

class BibleBrowser : public QWidget
{
	Q_OBJECT
	
public:
	BibleBrowser(QWidget *parent=0);
	~BibleBrowser();
	
	
signals:
	void verseListSelected(const BibleVerseList &);
	
public slots:
	void loadVerses(const QString& ref="");
	void clearSearch() { loadVerses(); }
	
protected slots:
	void searchReturnPressed();
	void searchTextChanged(const QString&);
	void referenceAvailable(const BibleVerseRef&, const BibleVerseList&);
	
	void createSlideGroup();

protected:
	void addSlide(SlideGroup *, TextBoxItem *, int height, const QSize & fitSize, const QString & plain);
	
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
};


#endif
