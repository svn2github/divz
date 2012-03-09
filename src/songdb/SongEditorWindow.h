#ifndef SONGEDITORWINDOW_H
#define SONGEDITORWINDOW_H

#include "SongSlideGroupFactory.h"

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QLineEdit>

class SongEditorHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
	SongEditorHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;
	
	QTextCharFormat rearFormat;
	QTextCharFormat tagFormat;
};

class MyQTextEdit;
class SlideEditorWindow;
class QCloseEvent;
class SongRecord;
class QPushButton;
#include <QCheckBox>

class SongEditorWindow : public AbstractSlideGroupEditor 
{
	Q_OBJECT
public:
	SongEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	~SongEditorWindow();
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
	
	void setSlideGroup(SlideGroup*g,bool syncToDatabase);
	
	void accepted();
	//void rejected();
	
	void editSongTemplate();
	
	void setSyncToDatabase(bool);
	
	void showSyncOption(bool);
	void isSyncOptionVisible() { m_syncBox->isVisible(); }
	
signals:
	void songCreated(SongRecord*);
	void songSaved();

protected:
// 	void closeEvent(QCloseEvent*);

private slots:
	void editorWindowClosed();
	
	void showArrPreview(bool);
	
	void arrTextChanged(const QString&);
	
	void editTextChanged();

private:
	MyQTextEdit *m_editor;
	MyQTextEdit *m_arrangementPreview;
	SongEditorHighlighter *m_highlighter;
	QLineEdit *m_title;
	QLineEdit *m_defaultArrangement;
	QLineEdit *m_arrangement;
	SlideGroup *m_slideGroup;
	AbstractSlideGroupEditor *m_editWin;
	
	QPushButton * m_tmplEditButton;
	
	bool m_syncToDatabase;
	
	QCheckBox * m_syncBox;
	
	QStringList m_currArrPreviewList;
};


#endif // SONGEDITORWINDOW_H
