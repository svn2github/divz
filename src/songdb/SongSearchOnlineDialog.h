#ifndef SONGSEARCHONLINEDIALOG_H
#define SONGSEARCHONLINEDIALOG_H

#include <QDialog>

namespace Ui {
    class SongSearchOnlineDialog;
}

class SongSearchOnlineDialog : public QDialog {
    Q_OBJECT
public:
    SongSearchOnlineDialog(QWidget *parent = 0);
    ~SongSearchOnlineDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SongSearchOnlineDialog *ui;
};

#endif // SONGSEARCHONLINEDIALOG_H
