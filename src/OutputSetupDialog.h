#ifndef OUTPUTSETUPDIALOG_H
#define OUTPUTSETUPDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class OutputSetupDialog;
}

class OutputSetupDialog : public QDialog {
	Q_OBJECT
public:
	OutputSetupDialog(QWidget *parent = 0);
	~OutputSetupDialog();

protected slots:
	void accepted();
	void rejected();

protected:
	void changeEvent(QEvent *e);

private:
	void setupScreenList();
	void setupOutputList();

	Ui::OutputSetupDialog *m_ui;
};

#endif // OUTPUTSETUPDIALOG_H
