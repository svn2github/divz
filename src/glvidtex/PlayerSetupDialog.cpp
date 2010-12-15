#include "PlayerSetupDialog.h"
#include "ui_PlayerSetupDialog.h"

#include "PlayerConnection.h"
#include "GLWidget.h"

#include <QtGui>

PlayerSetupDialog::PlayerSetupDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::PlayerSetupDialog)
	, m_con(0)
	, m_sub(0)
	, m_subviewModel(0)
	, m_playerList(0)
{
	ui->setupUi(this);
	setupUI();
}

PlayerSetupDialog::~PlayerSetupDialog()
{
    delete ui;
}

void PlayerSetupDialog::setupUI()
{
	connect(ui->addPlayerBtn, SIGNAL(clicked()), this, SLOT(newPlayer()));
	connect(ui->removePlayerBtn, SIGNAL(clicked()), this, SLOT(removePlayer()));
	
	connect(ui->newSubviewBtn, SIGNAL(clicked()), this, SLOT(newSubview()));
	connect(ui->delSubviewBtn, SIGNAL(clicked()), this, SLOT(removeSubview()));

	connect(ui->testConnectionBtn, SIGNAL(clicked()), this, SLOT(testConnection()));

	connect(ui->playerListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(playerSelected(const QModelIndex &)));
	connect(ui->playerListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(playerSelected(const QModelIndex &)));

	connect(ui->subviewListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(subviewSelected(const QModelIndex &)));
	connect(ui->subviewListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(subviewSelected(const QModelIndex &)));

	connect(ui->aphaMaskBrowseBtn, SIGNAL(clicked()), this, SLOT(alphamaskBrowse()));
	connect(ui->keyTLx, SIGNAL(valueChanged(int)), this, SLOT(subviewTopLeftXChanged(int)));
	connect(ui->keyTLy, SIGNAL(valueChanged(int)), this, SLOT(subviewTopLeftYChanged(int)));
	connect(ui->keyTRx, SIGNAL(valueChanged(int)), this, SLOT(subviewTopRightXChanged(int)));
	connect(ui->keyTRy, SIGNAL(valueChanged(int)), this, SLOT(subviewTopRightYChanged(int)));
	connect(ui->keyBLx, SIGNAL(valueChanged(int)), this, SLOT(subviewBottomLeftXChanged(int)));
	connect(ui->keyBLy, SIGNAL(valueChanged(int)), this, SLOT(subviewBottomLeftYChanged(int)));
	connect(ui->keyBRx, SIGNAL(valueChanged(int)), this, SLOT(subviewBottomRightXChanged(int)));
	connect(ui->keyBRy, SIGNAL(valueChanged(int)), this, SLOT(subviewBottomRightYChanged(int)));

	connect(ui->outputX, SIGNAL(valueChanged(int)), this, SLOT(screenXChanged(int)));
	connect(ui->outputY, SIGNAL(valueChanged(int)), this, SLOT(screenYChanged(int)));
	connect(ui->outputWidth, SIGNAL(valueChanged(int)), this, SLOT(screenWChanged(int)));
	connect(ui->outputHeight, SIGNAL(valueChanged(int)), this, SLOT(screenHChanged(int)));

	connect(ui->viewportX, SIGNAL(valueChanged(int)), this, SLOT(viewportXChanged(int)));
	connect(ui->viewportY, SIGNAL(valueChanged(int)), this, SLOT(viewportYChanged(int)));
	connect(ui->viewportWidth, SIGNAL(valueChanged(int)), this, SLOT(viewportWChanged(int)));
	connect(ui->viewportHeight, SIGNAL(valueChanged(int)), this, SLOT(viewportHChanged(int)));

	connect(ui->optIgnoreAR, SIGNAL(toggled(bool)), this, SLOT(ignoreArBoxChanged(bool)));

	connect(ui->aphaMaskBrowseBtn, SIGNAL(clicked()), this, SLOT(alphamaskBrowse()));
	connect(ui->alphaMaskFile, SIGNAL(textChanged(QString)), this, SLOT(showAlphaMaskPreview(QString)));
}

void PlayerSetupDialog::setPlayerList(PlayerConnectionList *list)
{
	m_playerList = list;
	ui->playerListview->setModel(list);
}

void PlayerSetupDialog::setCurrentPlayer(PlayerConnection* con)
{
	if(con == m_con)
		return;
	if(m_con)
	{
		disconnect(ui->playerName, 0, m_con, 0);
		disconnect(ui->playerHost, 0, m_con, 0);
		disconnect(ui->playerUser, 0, m_con, 0);
		disconnect(ui->playerPass, 0, m_con, 0);
	}
	
	if(m_subviewModel)
	{
		delete m_subviewModel;
		m_subviewModel = 0;
	}
	
	m_con = con;
	
	if(!con)
	{
		ui->boxConnection->setEnabled(false);
		ui->boxOutput->setEnabled(false);
		ui->boxSubviewOpts->setEnabled(false);
		ui->boxSubviews->setEnabled(false);
		ui->boxKeystone->setEnabled(true);
		return;
	}
	
	connect(ui->playerName, SIGNAL(textChanged(QString)), con, SLOT(setName(QString)));
	connect(ui->playerHost, SIGNAL(textChanged(QString)), con, SLOT(setHost(QString)));
	connect(ui->playerUser, SIGNAL(textChanged(QString)), con, SLOT(setUser(QString)));
	connect(ui->playerPass, SIGNAL(textChanged(QString)), con, SLOT(setPass(QString)));

	ui->boxConnection->setEnabled(true);
	ui->boxOutput->setEnabled(true);
// 	ui->boxSubviewOpts->setEnabled(true);
// 	ui->boxSubviews->setEnabled(true);
// 	ui->boxKeystone->setEnabled(true);

	m_subviewModel = new PlayerSubviewsModel(con);

	ui->subviewListview->setModel(m_subviewModel);
	ui->subviewListview->setCurrentIndex(m_subviewModel->index(0,0));
	
	GLWidgetSubview *sub = con->subviews().at(0);
	if(!sub)
	{
		sub = new GLWidgetSubview();
		con->addSubview(sub);
	}
	if(sub->title().isEmpty())
		sub->setTitle("Default Subview");
		
	setCurrentSubview(sub); 
}

void PlayerSetupDialog::setCurrentSubview(GLWidgetSubview * sub)
{
	if(sub == m_sub)
		return;
	if(m_sub)
	{
		disconnect(ui->subviewTitle, 0, m_sub, 0);
		disconnect(ui->subviewViewportX, 0, m_sub, 0);
		disconnect(ui->subviewViewportY, 0, m_sub, 0);
		disconnect(ui->subviewViewportX2, 0, m_sub, 0);
		disconnect(ui->subviewViewportY2, 0, m_sub, 0);
		disconnect(ui->alphaMaskFile, 0, m_sub, 0);
		disconnect(ui->subviewBrightnessSlider, 0, m_sub, 0);
		disconnect(ui->subviewContrastSlider, 0, m_sub, 0);
		disconnect(ui->subviewHueSlider, 0, m_sub, 0);
		disconnect(ui->subviewSaturationSlider, 0, m_sub, 0);
		disconnect(ui->optFlipH, 0, m_sub, 0);
		disconnect(ui->optFlipV, 0, m_sub, 0);
	}

	m_sub = sub;
	if(!sub)
	{
		ui->boxSubviewOpts->setEnabled(false);
		ui->boxSubviews->setEnabled(false);
		ui->boxKeystone->setEnabled(false);

		return;
	}

	connect(ui->subviewTitle, SIGNAL(textChanged(QString)), sub, SLOT(setTitle(QString)));
	connect(ui->subviewViewportX, SIGNAL(valueChanged(double)), sub, SLOT(setLeftPercent(double)));
	connect(ui->subviewViewportY, SIGNAL(valueChanged(double)), sub, SLOT(setTopPercent(double)));
	connect(ui->subviewViewportX2, SIGNAL(valueChanged(double)), sub, SLOT(setRightPercent(double)));
	connect(ui->subviewViewportY2, SIGNAL(valueChanged(double)), sub, SLOT(setBottomPercent(double)));

	connect(ui->alphaMaskFile, SIGNAL(textChanged(QString)), sub, SLOT(setAlphaMaskFile(QString)));
	connect(ui->subviewBrightnessSlider, SIGNAL(valueChanged(int)), sub, SLOT(setBrightness(int)) );
	connect(ui->subviewContrastSlider, SIGNAL(valueChanged(int)), sub, SLOT(setContrast(int)) );
	connect(ui->subviewHueSlider, SIGNAL(valueChanged(int)), sub, SLOT(setHue(int)) );
	connect(ui->subviewSaturationSlider, SIGNAL(valueChanged(int)), sub, SLOT(setSaturation(int)) );

	connect(ui->optFlipH, SIGNAL(toggled(bool)), sub, SLOT(setFlipHorizontal(bool)));
	connect(ui->optFlipV, SIGNAL(toggled(bool)), sub, SLOT(setFlipVertical(bool)));

	ui->boxSubviewOpts->setEnabled(true);
	ui->boxSubviews->setEnabled(true);
	ui->boxKeystone->setEnabled(true);
}

void PlayerSetupDialog::newPlayer()
{
	if(!m_playerList)
		return;
		
	PlayerConnection *con = new PlayerConnection();
	m_playerList->addPlayer(con);
	setCurrentPlayer(con);
	ui->playerListview->setCurrentIndex(m_playerList->index(m_playerList->size()-1,0));
}

void PlayerSetupDialog::removePlayer()
{
	if(!m_con)
		return;
	if(!m_playerList)
		return;
		
	/// TODO confirm deletion
	
	m_playerList->removePlayer(m_con);
	m_con->deleteLater();
	m_con = 0;
	
	if(m_playerList->size() > 0)
		setCurrentPlayer(m_playerList->at(0));
	else
		setCurrentPlayer(0);
}

void PlayerSetupDialog::newSubview()
{
	if(!m_con)
		return;
		
	GLWidgetSubview *sub = new GLWidgetSubview();
	m_con->addSubview(sub);
	setCurrentSubview(sub);
	ui->subviewListview->setCurrentIndex(m_subviewModel->index(m_con->subviews().size()-1,0));
}

void PlayerSetupDialog::removeSubview()
{
	if(!m_sub)
		return;
	if(!m_con)
		return;
		
	/// TODO confirm deletion
	
	m_con->removeSubview(m_sub);
	m_sub->deleteLater();
	m_sub = 0;
	
	if(m_con->subviews().size() > 0)
		setCurrentSubview(m_con->subviews().at(0));
	else
		setCurrentSubview(0);
}

void PlayerSetupDialog::testConnection()
{
	if(!m_con) 
		return;
	/// TODO test connection
}

void PlayerSetupDialog::playerSelected(const QModelIndex & idx)
{
	if(!m_playerList)
		return;
	if(idx.isValid())
		setCurrentPlayer(m_playerList->at(idx.row()));
}
//void PlayerSetupDialog::currentPlayerChanged(const QModelIndex &idx,const QModelIndex &);

void PlayerSetupDialog::subviewSelected(const QModelIndex & idx)
{
	if(!m_subviewModel)
		return;
	if(!m_con)
		return;
	if(idx.isValid())
		setCurrentSubview(m_con->subviews().at(idx.row()));
}
//void PlayerSetupDialog::currentSubviewChanged(const QModelIndex &idx,const QModelIndex &);

void PlayerSetupDialog::alphamaskBrowse()
{
	/// TODO add file browser logic
}

void PlayerSetupDialog::subviewTopLeftXChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[0].setX(value);
	m_sub->setCornerTranslations(poly);
}
void PlayerSetupDialog::subviewTopLeftYChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[0].setY(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewTopRightXChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[1].setX(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewTopRightYChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[1].setY(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewBottomLeftXChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[3].setX(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewBottomLeftYChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[3].setY(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewBottomRightXChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[2].setX(value);
	m_sub->setCornerTranslations(poly);
}

void PlayerSetupDialog::subviewBottomRightYChanged(int value)
{
	if(!m_sub)
		return;
	QPolygonF poly = m_sub->cornerTranslations();
	poly[2].setY(value);
	m_sub->setCornerTranslations(poly);
}


void PlayerSetupDialog::screenXChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->screenRect();
	rect = QRect(value, rect.y(), 
		     rect.width(), rect.height());
	m_con->setScreenRect(rect);
}

void PlayerSetupDialog::screenYChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->screenRect();
	rect = QRect(rect.x(), value, 
		     rect.width(), rect.height());
	m_con->setScreenRect(rect);
}

void PlayerSetupDialog::screenWChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->screenRect();
	rect = QRect(rect.x(), rect.y(), 
		     value, rect.height());
	m_con->setScreenRect(rect);
}

void PlayerSetupDialog::screenHChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->screenRect();
	rect = QRect(rect.x(), rect.y(), 
		     rect.width(), value);
	m_con->setScreenRect(rect);
}


void PlayerSetupDialog::viewportXChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->viewportRect();
	rect = QRect(value, rect.y(), 
		     rect.width(), rect.height());
	m_con->setViewportRect(rect);
}

void PlayerSetupDialog::viewportYChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->viewportRect();
	rect = QRect(rect.x(), value, 
		     rect.width(), rect.height());
	m_con->setViewportRect(rect);
}

void PlayerSetupDialog::viewportWChanged(int value)
{
	if(!m_con)
		return;
	QRect rect = m_con->viewportRect();
	rect = QRect(rect.x(), rect.y(), 
		     value, rect.height());
	m_con->setViewportRect(rect);
}

void PlayerSetupDialog::viewportHChanged(int value)

{
	if(!m_con)
		return;
	QRect rect = m_con->viewportRect();
	rect = QRect(rect.x(), rect.y(), 
		     rect.width(), value);
	m_con->setViewportRect(rect);
}

void PlayerSetupDialog::ignoreArBoxChanged(bool value)
{
	if(!m_con)
		return;
	m_con->setAspectRatioMode(value ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
}

void PlayerSetupDialog::showAlphaMaskPreview(const QString& value)
{
	ui->alphaMaskPreview->setPixmap(QPixmap(value));
}

