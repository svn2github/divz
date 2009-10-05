#ifndef GRIDDIALOG_H
#define GRIDDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class GridDialog;
}

class GridDialog : public QDialog {
    Q_OBJECT
public:
    GridDialog(QWidget *parent = 0);
    ~GridDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GridDialog *m_ui;
};

#endif // GRIDDIALOG_H
