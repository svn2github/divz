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

	connect(ui->testConnectionBtn, SIGNAL(clicked()), this, SLOT(testConnection()));

	/// TODO connect listview stuff for player
	//ui->playerListview;

	/// TOOD connect listview stuff for subview list
	//ui->subviewListview

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
	ui->boxSubviewOpts->setEnabled(true);
	ui->boxSubviews->setEnabled(true);
	ui->boxKeystone->setEnabled(true);

	/// TODO: ui->subviewListview->setModel(...);
	/// TODO: ui->subviewListview->setCurrentIndex(...);
	/// TODO: setCurrentSubview(...);
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

