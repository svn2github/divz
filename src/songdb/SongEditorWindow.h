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

class SlideEditorWindow;
class SongEditorWindow : public AbstractSlideGroupEditor 
{
	Q_OBJECT
public:
	SongEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	~SongEditorWindow();
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
	
	void accepted();
	//void close();
	
	void editSongTemplate();

private:
	QTextEdit *m_editor;
	SongEditorHighlighter *m_highlighter;
	QLineEdit *m_title;
	SlideGroup *m_slideGroup;
	SlideEditorWindow *m_editWin;
	

};


#endif // SONGEDITORWINDOW_H
