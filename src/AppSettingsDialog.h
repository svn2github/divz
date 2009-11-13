#ifndef APPSETTINGSDIALOG_H
#define APPSETTINGSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
	class AppSettingsDialog;
}

class AppSettingsDialog : public QDialog {
	Q_OBJECT
public:
	AppSettingsDialog(QWidget *parent = 0);
	~AppSettingsDialog();

protected:
	void changeEvent(QEvent *e);

protected slots:
	void slotUseOpenGLChanged(bool);
	void slotConfigOutputs();
	
	void slotDiskCacheBrowse();
	
	void slotAccepted();
	
	void portChanged(int);
	void linkActivated(const QString&);
	//void slotRejected();

private:
	Ui::AppSettingsDialog *m_ui;
};

#endif // APPSETTINGSDIALOG_H
