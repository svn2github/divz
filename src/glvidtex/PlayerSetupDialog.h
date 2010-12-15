#ifndef PLAYERSETUPDIALOG_H
#define PLAYERSETUPDIALOG_H

#include <QDialog>
#include <QtGui>

namespace Ui {
    class PlayerSetupDialog;
}

class GLWidgetSubview;
class PlayerConnection;
class PlayerSetupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PlayerSetupDialog(QWidget *parent = 0);
	~PlayerSetupDialog();

	
private slots:
	void newPlayer();
	void removePLayer();
	
	void testConnection();
	
	void playerSelected(const QModelIndex &);
	void currentPlayerChanged(const QModelIndex &idx,const QModelIndex &);
	
	void subviewSelected(const QModelIndex &);
	void currentSubviewChanged(const QModelIndex &idx,const QModelIndex &);
	
	void setCurrentPlayer(PlayerConnection*);
	void setCurrentSubview(GLWidgetSubview*);
	
	void alphamaskBrowse();
	
	void subviewTopLeftXChanged(int);
	void subviewTopLeftYChanged(int);
	
	void subviewTopRightXChanged(int);
	void subviewTopRightYChanged(int);
	
	void subviewBottomLeftXChanged(int);
	void subviewBottomLeftYChanged(int);
	
	void subviewBottomRightXChanged(int);
	void subviewBottomRightYChanged(int);
	
	void screenXChanged(int);
	void screenYChanged(int);
	void screenWChanged(int);
	void screenHChanged(int);
	
	void viewportXChanged(int);
	void viewportYChanged(int);
	void viewportWChanged(int);
	void viewportHChanged(int);
	
	void ignoreArBoxChanged(bool);
	
	void showAlphaMaskPreview(const QString&);
	
	
private:
	void setupUI();
	
	Ui::PlayerSetupDialog *ui;
	
	PlayerConnection *m_con;
	GLWidgetSubview *m_sub;
};

#endif // PLAYERSETUPDIALOG_H
