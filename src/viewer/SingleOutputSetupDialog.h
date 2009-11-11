#ifndef SingleOutputSetupDialog_H
#define SingleOutputSetupDialog_H

#include <QtGui/QDialog>
class Output;

namespace Ui {
    class SingleOutputSetupDialog;
}

class SingleOutputSetupDialog : public QDialog {
	Q_OBJECT
public:
	SingleOutputSetupDialog(QWidget *parent = 0);
	~SingleOutputSetupDialog();

protected slots:
	void accept();
	void reject();
	
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
	
	void slotStayOnTop(bool);
	void slotMjpegEnabled(bool);
	void slotMjpegPort(int);
	void slotMjpegFps(int);
	
	void slotDiskCacheBrowse();

protected:
	void changeEvent(QEvent *e);

private:
	void setupScreenList();
	void setOutput(Output*);

	Ui::SingleOutputSetupDialog *m_ui;
	
	Output * m_output;
	int m_outputIdx;
};

#endif // SingleOutputSetupDialog_H
