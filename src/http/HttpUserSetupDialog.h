#ifndef HTTPUSERSETUPDIALOG_H
#define HTTPUSERSETUPDIALOG_H

#include <QDialog>

namespace Ui {
	class HttpUserSetupDialog;
}

class HttpUserSetupDialog : public QDialog 
{
	Q_OBJECT
public:
	HttpUserSetupDialog(QWidget *parent = 0);
	~HttpUserSetupDialog();

protected slots:
	void addUser();
	void delUser();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::HttpUserSetupDialog *ui;
};

#endif // HTTPUSERSETUPDIALOG_H
