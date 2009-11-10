#ifndef IMAGEIMPORTDIALOG_H
#define IMAGEIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class ImageImportDialog;
}

class Document;
class DocumentListModel;
class ImageImportDialog : public QDialog {
    Q_OBJECT
public:
    ImageImportDialog(Document *doc, QWidget *parent = 0);
    ~ImageImportDialog();

protected:
    void changeEvent(QEvent *e);
    void accept();

protected slots:
    void importDirBrowse();
    void copyToDirBrowse();
    void advancedBtnToggled(bool);

private:
    Ui::ImageImportDialog *m_ui;
    Document *m_doc;
    DocumentListModel *m_model;
    int m_acceptCount;
};

#endif // IMAGEIMPORTDIALOG_H
