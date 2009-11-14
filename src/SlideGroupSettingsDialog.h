#ifndef SLIDEGROUPSETTINGSDIALOG_H
#define SLIDEGROUPSETTINGSDIALOG_H

#include <QtGui/QDialog>
class SlideGroup;
class DocumentListModel;

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
    void slotAccepted();

private:
    Ui::SlideGroupSettingsDialog *m_ui;
    SlideGroup *m_slideGroup;
    DocumentListModel *m_model;
};

#endif // SLIDEGROUPSETTINGSDIALOG_H
