#include <QApplication>

#include "GLWidget.h"

#include "GLImageDrawable.h"
#include "GLVideoLoopDrawable.h"
#include "GLVideoInputDrawable.h"
#include "GLVideoFileDrawable.h"
#include "GLVideoReceiverDrawable.h"
#include "GLTextDrawable.h"
#include "GLVideoMjpegDrawable.h"

#include "MetaObjectUtil.h"

#include "DirectorWindow.h"

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	
	qApp->setApplicationName("GLDirector");
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");
	
	MetaObjectUtil_Register(GLImageDrawable);
	MetaObjectUtil_Register(GLTextDrawable);
	MetaObjectUtil_Register(GLVideoFileDrawable);
	MetaObjectUtil_Register(GLVideoInputDrawable);
	MetaObjectUtil_Register(GLVideoLoopDrawable);
	MetaObjectUtil_Register(GLVideoReceiverDrawable);

	
	DirectorWindow *director = new DirectorWindow();
	director->show();

	return app.exec();
}

