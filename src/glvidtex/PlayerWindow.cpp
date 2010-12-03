#include "PlayerWindow.h"

#include <QtGui>
#include "GLWidget.h"

PlayerWindow::PlayerWindow(QWidget *parent)
	: GLWidget(parent)
{
	bool verbose = true;
	QString configFile = "player.ini";
	
	if(verbose)
		qDebug() << "PlayerWindow: Reading settings from "<<configFile;
		
	QSettings settings(configFile,QSettings::IniFormat);
	
	QString tmp;
	QStringList parts;
	QPoint point;
	 
	#define READ_POINT(key,default) \
		tmp = settings.value(key,default).toString(); \
		parts = tmp.split("x"); \
		point = QPoint(parts[0].toInt(),parts[1].toInt()); \
		if(verbose) qDebug() << "PlayerWindow: " key ": " << point; 
	
	// Window position and size
	READ_POINT("window-pos","10x10");
	QPoint windowPos = point;
	
	READ_POINT("window-size","640x480");
	QPoint windowSize = point;
	
	if(verbose)
		qDebug() << "PlayerWindow: pos:"<<windowPos<<", size:"<<windowSize;
	
	resize(windowSize.x(),windowSize.y());
	move(windowPos.x(),windowPos.y());
	
	bool frameless = settings.value("frameless","true").toString() == "true";
	if(frameless)
		setWindowFlags(Qt::FramelessWindowHint);// | Qt::ToolTip);
	
	// Keystoning / Corner Translations
	READ_POINT("key-tl","0x0");
	setTopLeftTranslation(point);
	
	READ_POINT("key-tr","0x0");
	setTopRightTranslation(point);
	
	READ_POINT("key-bl","0x0");
	setBottomLeftTranslation(point);
	
	READ_POINT("key-br","0x0");
	setBottomRightTranslation(point);
	
	// Brightness/Contrast, Hue/Sat
	setBrightness(settings.value("brightness","0").toInt());
	setContrast(settings.value("contrast","0").toInt());
	setHue(settings.value("hue","0").toInt());
	setSaturation(settings.value("saturation","0").toInt());
	
	// Flip H/V
	bool fliph = settings.value("flip-h","false").toString() == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-h: "<<fliph;
	setFlipHorizontal(fliph);
	
	bool flipv = settings.value("flip-v","false").toString() == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-v: "<<flipv;
	setFlipVertical(flipv);
	
	// Aspet Ratio Mode
	setAspectRatioMode(settings.value("ignore-ar","false").toString() == "true" ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
	
	// Canvas Size
	READ_POINT("canvas-size","1000x750");
	setCanvasSize(QSizeF((qreal)point.x(),(qreal)point.y()));
	
	// Alpha Mask
	QString alphaFile = settings.value("alphamask","").toString();
	if(!alphaFile.isEmpty())
	{
		QImage alphamask(alphaFile);
		if(alphamask.isNull())
			qDebug() << "PlayerWindow: Error loading alphamask "<<alphaFile;
		else
			setAlphaMask(alphamask);
	}
	
}
