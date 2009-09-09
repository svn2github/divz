/***************************************************************************
 *                                                                         *
 *   This file is part of the Fotowall project,                            *
 *       http://code.google.com/p/fotowall                                 *
 *                                                                         *
 *   Copyright (C) 2007-2009 by Enrico Ros <enrico.ros@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "MainWindow.h"
#include "MyGraphicsScene.h"


#include <QAction>
#include <QApplication>
#include <QDir>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFile>
#include <QImageReader>
#include <QInputDialog>
#include <QMenu>
/*#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>*/
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QStyleOption>
#include <QCheckBox>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <assert.h>

#include "XmlRead.h"
#include "XmlSave.h"
#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"


#include <QCommonStyle>
class RubberBandStyle : public QCommonStyle 
{
	public:
		void drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0) const
		{
			if (element == CE_RubberBand) 
			{
				painter->save();
				QColor color = option->palette.color(QPalette::Highlight);
				painter->setPen(color);
				color.setAlpha(80); painter->setBrush(color);
				painter->drawRect(option->rect.adjusted(0,0,-1,-1));
				painter->restore();
				return;
			}
			return QCommonStyle::drawControl(element, option, painter, widget);
		}
		
		int styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
		{
			if (hint == SH_RubberBand_Mask)
				return false;
			return QCommonStyle::styleHint(hint, option, widget, returnData);
		}
};
/*
#include <QMessageBox>
class WarningBox : public QDialog
{
	public:
		WarningBox(const QString & key, const QString & title, const QString & text)
	#if QT_VERSION >= 0x040500
		: QDialog(0, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
	#else
		: QDialog(0, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
	#endif
		{
			// skip this if asked to not repeat it
			QSettings s;
			if (s.value(key, false).toBool())
				return;
		
			// create contents
			QLabel * label = new QLabel(this);
			label->setTextInteractionFlags(Qt::NoTextInteraction);
			label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
			label->setOpenExternalLinks(true);
			label->setContentsMargins(2, 0, 0, 0);
			label->setTextFormat(Qt::RichText);
			label->setWordWrap(true);
			label->setText(text);
		
			QLabel * iconLabel = new QLabel(this);
			iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			iconLabel->setPixmap(style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(32, 32));
		
			QCheckBox * checkBox = new QCheckBox(this);
			checkBox->setText(tr("show this warning again next time"));
		
			QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
			buttonBox->setCenterButtons(style()->styleHint(QStyle::SH_MessageBox_CenterButtons, 0, this));
			QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(close()));
			buttonBox->setFocus();
		
			QGridLayout * grid = new QGridLayout(this);
			grid->addWidget(iconLabel, 0, 0, 2, 1, Qt::AlignTop);
			grid->addWidget(label, 0, 1, 1, 1);
			grid->addWidget(checkBox, 1, 1, 1, 1);
			grid->addWidget(buttonBox, 2, 0, 1, 2);
		
			// customize and dialog
			setWindowTitle(title);
			setModal(true);
			setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
			QSize screenSize = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
			setMinimumWidth(qMin(screenSize.width() - 480, screenSize.width()/2));
			setMinimumHeight(190);
			grid->activate();
			adjustSize();
			resize(minimumSize());
			exec();
		
			// avoid popping up again, if chosen
			if (!checkBox->isChecked())
				s.setValue(key, true);
		}
};*/

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView 
{
	public:
		MyGraphicsView(QWidget * parent)
			: QGraphicsView(parent)
			, m_desk(0)
		{
			// customize widget
			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			setInteractive(true);
			setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing /*| QPainter::SmoothPixmapTransform */);
			setDragMode(QGraphicsView::RubberBandDrag);
			setAcceptDrops(true);
			setFrameStyle(QFrame::NoFrame);
		
		//             // don't autofill the view with the Base brush
		//             QPalette pal;
		//             pal.setBrush(QPalette::Base, Qt::NoBrush);
		//             setPalette(pal);
		
			// use own style for drawing the RubberBand (opened on the viewport)
			viewport()->setStyle(new RubberBandStyle);
		
			// can't activate the cache mode by default, since it inhibits dynamical background picture changing
			//setCacheMode(CacheBackground);
		}
	
		void setMyScene(MyGraphicsScene * desk)
		{
			setScene(desk);
			m_desk = desk;
		}
	
	protected:
		void resizeEvent(QResizeEvent * event)
		{
// 			if (m_desk)
// 				m_desk->resize(contentsRect().size());
			QGraphicsView::resizeEvent(event);
		}
	
	private:
		MyGraphicsScene * m_desk;
};




class SimpleTextItem : public QGraphicsItemGroup
{
private:
	QGraphicsSimpleTextItem * m_shadow;
	QGraphicsSimpleTextItem * m_text;
	bool m_shadowEnabled;
	
public:
	SimpleTextItem(QString str = "") : QGraphicsItemGroup()
	{
		m_text   = new QGraphicsSimpleTextItem(str);
		m_shadow = new QGraphicsSimpleTextItem(str);
		
// 		QPen textPenShadow;
// 		textPenShadow.setWidthF(3);
// 		textPenShadow.setColor(QColor(0,0,0,100));
// 		m_shadow->setPen(textPenShadow);
		m_shadow->setBrush(QColor(0,0,0,100));
		
		setZValue(1);
		
		addToGroup(m_shadow);
		addToGroup(m_text);
		
		m_shadowEnabled = true;
	}
	
	~SimpleTextItem()
	{
		if(m_text != NULL) 
		{
			delete m_text;
			m_text = 0;
		}
		
		if(m_shadow != NULL) 
		{
			delete m_shadow;
			m_shadow = 0;
		}
	}
	
	bool shadowEnabled() { return m_shadowEnabled; }
	void setShadowEnabled(bool flag) 
	{ 
		m_shadowEnabled = flag;
		if(flag)
		{
			m_shadow->show();
		}
		else
		{
			m_shadow->hide();
		}
	}
	
	  QFont font () const { return m_text->font(); }
	QString text () const { return m_text->text(); }

	void setPen ( const QPen & pen )
	{
		m_text->setPen(pen);
		//m_shadow->setPen(pen);
	}
	
	void setBrush( const QBrush & brush )
	{
		m_text->setBrush(brush);
		//m_shadow->setBrush(brush);
	}
	
	void setFont( const QFont & font)
	{
		m_text->setFont(font);
		m_shadow->setFont(font);
	}
	
	void setZValue( qreal z )
	{
		((QGraphicsSimpleTextItem*)this)->setZValue(z);
		m_text->setZValue(z);
		m_shadow->setZValue(z-0.001);
	}
	
	void setPos( qreal x, qreal y, qreal shadowOffset = 4)
	{
		m_shadow->setPos(x+shadowOffset,y+shadowOffset);
		m_text->setPos(x,y);
	}
	
	
};


MainWindow::MainWindow(QWidget * parent)
    : QWidget(parent)
{
	// setup widget
	QRect geom = QApplication::desktop()->availableGeometry();
	resize(2 * geom.width() / 3, 2 * geom.height() / 3);
	
	
	MyGraphicsScene *scene = new MyGraphicsScene(this);
	MyGraphicsView *graphicsView = new MyGraphicsView(this);
	
	graphicsView->setMyScene(scene);
	scene->setSceneRect(0,0,1000,1000);
	
	
	//qDebug("Checking for OpenGL...");
	#ifndef QT_NO_OPENGL
		graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		qDebug("Loaded OpenGL viewport.");
	#endif
	graphicsView->setRenderHint( QPainter::TextAntialiasing, true);
	graphicsView->setRenderHint( QPainter::Antialiasing, true );
	
	
// 	SimpleTextItem * textItem = new SimpleTextItem("Hello, World");
// 	QPen textPen;
// 	textPen.setWidthF(3);
// 	textPen.setColor(QColor(0,0,0,240));
// 	textItem->setPen(textPen);
// 	textItem->setBrush(QColor(255,255,255,255));
// 	textItem->setFont(QFont("Tahoma",88,QFont::Bold));
// 	textItem->setZValue(2);
// 	textItem->setPos(10,230);
// 	scene->addItem(textItem);
// 	textItem->setFlags(QGraphicsItem::ItemIsMovable);
// 	
	
	//TextContent * text = scene->addTextContent();
	m_slide = new Slide();
	
	printf("\n\n");
	
	if(QFile("test.xml").exists())
	{
		XmlRead r("test.xml");
		r.readSlide(m_slide);
		
		QList<AbstractItem *> items = m_slide->itemList();
		AbstractItem * item = items.at(0);
		
		assert(item != NULL);
		
		printf("> Load Test:\n");
		printf("Item Class: %d\n",item->itemClass());
		printf("Item Name: %s\n",item->itemName().toAscii().constData());
		printf("Item Id: %d\n",item->itemId());
		
		if(item->itemClass() == ITEM_TEXT )
		{
// 			qDebug("Text Item: Text: '%s'\n", ((TextItem *)item)->text().toAscii().constData());
		}
		else
		{
			printf("(Unknown item class)\n");
		}
		
                //printf("Mark1\n");
		AbstractVisualItem *v = (AbstractVisualItem*)item;
                //printf("Mark2\n");
		AbstractContent * visual = v->createDelegate(scene);
                //printf("Mark3\n");
		scene->initContent(visual, QPoint(v->pos().x(),v->pos().y()));
		
                //printf("Done loading\n");
		
	}
	else
	{
		TextItem *t = m_slide->createText(QPoint());
		t->setText("Hello World!");
		t->setPos(QPointF(10,10));
		t->setItemName("TextItem-1");
		t->setItemId(ItemFactory::nextId());
		
		printf("> Save Test:\n");
		printf("Item Class: %d\n",t->itemClass());
                qDebug("Item Name: %s\n",t->itemName().toAscii().constData());
		printf("Item Id: %d\n",t->itemId());
		
		AbstractContent * item = t->createDelegate(scene);
		scene->initContent(item, QPoint(t->pos().x(),t->pos().y()));
		
	}
	
	
	printf("\n\n");


	
	QBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(graphicsView);
// 	layout->addWidget(rotateSlider);
// 	layout->addLayout(controlLayout);
	
	
	
	setLayout(layout);
}

MainWindow::~MainWindow()
{
	QList<AbstractItem *> items = m_slide->itemList();
// 	AbstractItem * item = items.at(0);
		
	TextItem *t = (TextItem*)items.at(0);
// 	t->setText("Hello World!");
// 	t->setPos(QPointF(10,10));
// 	t->setItemName("TextItem-1");
// 	t->setItemId(ItemFactory::nextId());
	
	printf("> Save Test:\n");
	printf("Item Class: %d\n",t->itemClass());
	printf("Item Name: %s\n",t->itemName().toAscii().constData());
	printf("Item Id: %d\n",t->itemId());
	
	if(t->itemClass() == ITEM_TEXT )
	{
                qDebug("Text Item: Text: '%s'", t->text().toAscii().constData());
	}
	else
	{
		printf("(Unknown item class)\n");
	}
	
	XmlSave save("test.xml");
	save.saveSlide(m_slide);
	
	delete m_slide;
	m_slide = 0;

}

