#include <QStyle>
#include <QSettings>

#include <QtGui/QApplication>
#include <QFont>
#include <QFontMetrics>

// RenderOpts stuff is only here to satisfy the linker, not used in this test
#include "RenderOpts.h"
// init RenderOpts defaults
bool RenderOpts::LastMirrorEnabled = true;
bool RenderOpts::ARGBWindow = false;
bool RenderOpts::HQRendering = false;
bool RenderOpts::FirstRun = false;
bool RenderOpts::OxygenStyleQuirks = false;
bool RenderOpts::DisableVideoProvider = false;
QColor RenderOpts::hiColor;


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
		
	QFont font;
	QFont("Sans-Serif",12);
	QFontMetrics metrics(font);
	QRect textRect1 = metrics.boundingRect("test");
	QRect textRect2 = metrics.boundingRect("test ");
	qDebug("Without Space: %d, With Space: %d", textRect1.width(), textRect2.width());
	
	return -1;
}

