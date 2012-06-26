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
	void slotTemplateFolderBrowse();
	
	void slotAccepted();
	
	void controlPortChanged(int);
	void linkActivated(const QString&);
	
	void viewerPortChanged(int);
	void tabletPortChanged(int);
	//void slotRejected();
	
	void addResBtn();
	void delResBtn();
	void adjustTableSize();
	
	void slideGroupTypeChanged(int);
	void saveUserEventActions();
	
	void httpUserSetup();

private:
	Ui::AppSettingsDialog *m_ui;
	int m_currentEventGroupType;
};

#endif // APPSETTINGSDIALOG_H
