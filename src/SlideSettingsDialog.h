#ifndef SLIDESETTINGSDIALOG_H
#define SLIDESETTINGSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class SlideSettingsDialog;
}

class Slide;
class SlideGroup;
class SlideGroupListModel;

class SlideSettingsDialog : public QDialog 
{
	Q_OBJECT
	
public:
	SlideSettingsDialog(Slide *slide, QWidget *parent = 0);
	~SlideSettingsDialog();
	
	// Indicates that the slide we're editing is part of an alternate group for this primary group
	void setPrimaryGroup(SlideGroup*);

protected:
	void changeEvent(QEvent *e);
	
protected slots:
	void setAutoChangeTime(double);
	void slotGuess();
	void slotNever();
	void slotAccepted();

private:
	Ui::SlideSettingsDialog *m_ui;
	Slide * m_slide;
	SlideGroupListModel *m_primaryGroupModel;
};

#endif // SLIDESETTINGSDIALOG_H
