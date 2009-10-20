#ifndef SLIDESETTINGSDIALOG_H
#define SLIDESETTINGSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class SlideSettingsDialog;
}

class Slide;
class SlideSettingsDialog : public QDialog {
    Q_OBJECT
public:
    SlideSettingsDialog(Slide *slide, QWidget *parent = 0);
    ~SlideSettingsDialog();

protected:
    void changeEvent(QEvent *e);
protected slots:
    void setAutoChangeTime(double);
    void slotGuess();
    void slotNever();

private:
    Ui::SlideSettingsDialog *m_ui;
    Slide * m_slide;
};

#endif // SLIDESETTINGSDIALOG_H
