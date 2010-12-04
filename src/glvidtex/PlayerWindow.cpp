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
	
	QString str;
	QStringList parts;
	QPoint point;
	
	QString activeGroup = settings.value("config").toString();
	
	str = settings.value("verbose").toString();
	if(!str.isEmpty())
		verbose = str == "true";
	
	if(verbose && !activeGroup.isEmpty())
		qDebug() << "PlayerWindow: Using config:"<<activeGroup;
	
	#define READ_STRING(key,default) \
		(!activeGroup.isEmpty() ? \
			(!(str = settings.value(QString("%1/%2").arg(activeGroup).arg(key)).toString()).isEmpty() ?  str : \
				settings.value(key,default).toString()) : \
			settings.value(key,default).toString())
			
	#define READ_POINT(key,default) \
		str = READ_STRING(key,default); \
		parts = str.split("x"); \
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
	
	bool frameless = READ_STRING("frameless","true") == "true";
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
	setBrightness(READ_STRING("brightness","0").toInt());
	setContrast(READ_STRING("contrast","0").toInt());
	setHue(READ_STRING("hue","0").toInt());
	setSaturation(READ_STRING("saturation","0").toInt());
	
	// Flip H/V
	bool fliph = READ_STRING("flip-h","false") == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-h: "<<fliph;
	setFlipHorizontal(fliph);
	
	bool flipv = READ_STRING("flip-v","false") == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-v: "<<flipv;
	setFlipVertical(flipv);
	
	// Rotate
	int rv = READ_STRING("rotate","0").toInt();
	if(verbose)
		qDebug() << "PlayerWindow: rotate: "<<rv;
	
	if(rv != 0)
		setCornerRotation(rv == -1 ? GLWidget::RotateLeft  : 
				  rv ==  1 ? GLWidget::RotateRight : 
				             GLWidget::RotateNone);
	
	// Aspet Ratio Mode
	setAspectRatioMode(READ_STRING("ignore-ar","false") == "true" ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
	
	// Canvas Size
	READ_POINT("canvas-size","1000x750");
	setCanvasSize(QSizeF((qreal)point.x(),(qreal)point.y()));
	
	// Alpha Mask
	QString alphaFile = READ_STRING("alphamask","");
	if(!alphaFile.isEmpty())
	{
		QImage alphamask(alphaFile);
		if(alphamask.isNull())
			qDebug() << "PlayerWindow: Error loading alphamask "<<alphaFile;
		else
			setAlphaMask(alphamask);
	}
}
