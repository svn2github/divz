#ifndef IMAGEIMPORTDIALOG_H
#define IMAGEIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class ImageImportDialog;
}

class ImageImportDialog : public QDialog {
    Q_OBJECT
public:
    ImageImportDialog(QWidget *parent = 0);
    ~ImageImportDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ImageImportDialog *m_ui;
};

#endif // IMAGEIMPORTDIALOG_H
