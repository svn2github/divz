#include "MainWindow.h"
#include "RenderOpts.h"

#include <QStyle>
#include <QSettings>

#if defined(STATIC_LINK)
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
#endif

// init RenderOpts defaults
bool RenderOpts::LastMirrorEnabled = true;
bool RenderOpts::ARGBWindow = false;
bool RenderOpts::HQRendering = false;
bool RenderOpts::FirstRun = false;
bool RenderOpts::OxygenStyleQuirks = false;
bool RenderOpts::DisableVideoProvider = false;
QColor RenderOpts::hiColor;

#include <assert.h>

#include <QtGui/QApplication>

#include "XmlRead.h"
#include "XmlSave.h"
#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"

int main(int argc, char **argv)
{
		
	#if !defined(Q_OS_MAC) // raster on OSX == b0rken
		// use the Raster GraphicsSystem as default on 4.5+
		#if QT_VERSION >= 0x040500
		QApplication::setGraphicsSystem("raster");
		#endif
	#endif

	QApplication app(argc, argv);
	
	app.setApplicationName("DViz");
	app.setApplicationVersion("0.1.5");
	app.setOrganizationName("Josiah Bryan");
	app.setOrganizationDomain("mybryanlife.com");

	RenderOpts::OxygenStyleQuirks = app.style()->objectName() == QLatin1String("oxygen");
	
	QSettings s;
	//RenderOpts::FirstRun = s.value("fotowall/firstTime", true).toBool();
	RenderOpts::hiColor = app.palette().color(QPalette::Highlight);
	RenderOpts::DisableVideoProvider = app.arguments().contains("-novideo");
	//s.setValue("fotowall/firstTime", false);

	MainWindow mw;
	//mw.showMaximized();
	mw.show();
	
	return app.exec();
	
// 	printf("\n\n");
// 	
// 	//printf("Thanks for the fish!\n");
// 	if(QFile("test.xml").exists())
// 	{
// 		XmlRead r("test.xml");
// 		Slide s;
// 		r.readSlide(&s);
// 		
// 		QList<AbstractItem *> items = s.itemList();
// 		AbstractItem * item = items.at(0);
// 		
// 		assert(item != NULL);
// 		
// 		printf("> Load Test:\n");
// 		printf("Item Class: %d\n",item->itemClass());
// 		printf("Item Name: %s\n",item->itemName().toAscii().constData());
// 		printf("Item Id: %d\n",item->itemId());
// 		
// 		if(item->itemClass() == ITEM_TEXT )
// 		{
// 			printf("Text Item: Text: '%s'", ((TextItem *)item)->text().toAscii().constData());
// 		}
// 		else
// 		{
// 			printf("(Unknown item class)\n");
// 		}
// 		
// 		
// 	}
// 	else
// 	{
// 		Slide s;
// 		TextItem *t = s.createText(QPoint());
// 		t->setText("Hello World!");
// 		t->setPos(QPointF(10,10));
// 		t->setItemName("TextItem-1");
// 		t->setItemId(ItemFactory::nextId());
// 		
// 		printf("> Save Test:\n");
// 		printf("Item Class: %d\n",t->itemClass());
// 		printf("Item Name: %s\n",t->itemName().toAscii().constData());
// 		printf("Item Id: %d\n",t->itemId());
// 		
// 		if(t->itemClass() == ITEM_TEXT )
// 		{
// 			printf("Text Item: Text: '%s'", t->text().toAscii().constData());
// 		}
// 		else
// 		{
// 			printf("(Unknown item class)\n");
// 		}
// 		
// 		XmlSave save("test.xml");
// 		save.saveSlide(&s);
// 	}
// 	
// 	
// 	printf("\n\n");
// 	
	return -1;
}

