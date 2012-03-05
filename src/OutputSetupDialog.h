#ifndef OUTPUTSETUPDIALOG_H
#define OUTPUTSETUPDIALOG_H

#include <QtGui/QDialog>
class Output;

namespace Ui {
    class OutputSetupDialog;
}

class OutputSetupDialog : public QDialog {
	Q_OBJECT
public:
	OutputSetupDialog(QWidget *parent = 0);
	~OutputSetupDialog();

protected slots:
	void accept();
	void reject();
	
	void slotOutputListCellActivated(int,int);
	void slotScreenListCellActivated(int,int);
	void slotOutputEnabledStateChanged(int);
	void slotTabChanged(int);
	void slotOutputNameChanged(const QString&);
	void slotCustX(int);
	void slotCustY(int);
	void slotCustW(int);
	void slotCustH(int);
	void slotNetRoleChanged(bool);
	void slotHostChanged(const QString&);
	void slotPortChanged(int);
	void slotAllowMultChanged(int);
	void slotNew();
	void slotDel();
	
	void slotStayOnTop(bool);
	void slotMjpegEnabled(bool);
	void slotMjpegPort(int);
	void slotMjpegFps(int);
	
	void slotIgnoreAR(bool);
	
	void slotRequireAlt(bool);

protected:
	void changeEvent(QEvent *e);

private:
	void setupScreenList();
	void setupOutputList();
	void setOutput(Output*);

	Ui::OutputSetupDialog *m_ui;
	
	Output * m_output;
	int m_outputIdx;
};

#endif // OUTPUTSETUPDIALOG_H
