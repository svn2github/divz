#ifndef GENERICITEMCONFIG_H
#define GENERICITEMCONFIG_H

#include <QtGui/QDialog>

namespace Ui {
    class GenericItemConfig;
}

class GenericItemConfig : public QDialog {
    Q_OBJECT
public:
    GenericItemConfig(QWidget *parent = 0);
    ~GenericItemConfig();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GenericItemConfig *m_ui;
};

#endif // GENERICITEMCONFIG_H
