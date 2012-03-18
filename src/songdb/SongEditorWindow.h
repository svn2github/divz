#ifndef SONGEDITORWINDOW_H
#define SONGEDITORWINDOW_H

#include "SongSlideGroupFactory.h"
#include "SongEditorHighlighter.h"

#include <QtGui>

class MyQTextEdit;
class SlideEditorWindow;
class QCloseEvent;
class SongRecord;
class QPushButton;
class ArrangementListModel;
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
	
	void arrModelChange(QStringList, bool changeLineEdit=true);
	void resetArr();
	
	void arrListViewToggled(bool);

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

	ArrangementListModel *m_arrModel;
	QListView *m_arrListView;
};


#endif // SONGEDITORWINDOW_H
