#ifndef SLIDEGROUPSETTINGSDIALOG_H
#define SLIDEGROUPSETTINGSDIALOG_H

#include <QtGui/QDialog>
#include "model/SlideGroup.h"

namespace Ui {
    class SlideGroupSettingsDialog;
}

class SlideGroupSettingsDialog : public QDialog {
    Q_OBJECT

public:
    SlideGroupSettingsDialog(SlideGroup *group, QWidget *parent = 0);
    ~SlideGroupSettingsDialog();

protected:
    void changeEvent(QEvent *e);

protected slots:
    void titleChanged(const QString&);
    void autoChangeGroup(bool);

private:
    Ui::SlideGroupSettingsDialog *m_ui;
    SlideGroup *m_slideGroup;
};

#endif // SLIDEGROUPSETTINGSDIALOG_H
