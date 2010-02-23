/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Graphics Dojo project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtWebKit>
#include <QPaintEvent>
#include <QLabel>
#include <QPixmap>
#include <QPainter>

#if QT_VERSION < 0x0040500
#error You need Qt 4.5 or newer
#endif

class Container : public QWidget
{
    Q_OBJECT

public:
	Container(QWidget *parent = 0): QWidget(parent) 
	{
		
		connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(repaintView()));
		repaintTimer.setSingleShot(true);
		repaintTimer.setInterval(0);
		//QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,true);
		
		copyLabel = new QLabel();
		copyLabel->resize(640,480);
		
		copyPixmap = QPixmap(640,480);
		QPainter painter(&copyPixmap);
		painter.fillRect(copyPixmap.rect(),Qt::black);
		painter.end();
		copyLabel->setPixmap(copyPixmap);
		copyLabel->show();

		lockPainter = false;
		view = new QWebView(this);
		view->installEventFilter(this);
	
		QVBoxLayout *layout = new QVBoxLayout(this);
		setLayout(layout);
		layout->addWidget(view);
	
// 		QPalette palette = view->palette();
// 		palette.setBrush(QPalette::Base, Qt::transparent);
// 		view->page()->setPalette(palette);
// 		view->setAttribute(Qt::WA_OpaquePaintEvent, false);
		connect(view, SIGNAL(titleChanged(const QString&)), SLOT(setWindowTitle(const QString&)));
	 	
	 	
		view->load(QUrl("http://www.google.com/"));
		//view->setZoomFactor(0.8);
	
		gradient.setColorAt(0.0, QColor(249, 247, 96));
		gradient.setColorAt(1.0, QColor(235, 203, 32));
		gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	
		resize(640, 480);
		
	}

protected:
	bool eventFilter(QObject *obj, QEvent *event)
 	{
		if (obj == view) 
		{
			//qDebug() << "eventFilter: Got event type: "<<event->type();
 			if (event->type() == QEvent::Paint) 
 			{
 				QPaintEvent *paintEvent = static_cast<QPaintEvent*>(event);
 				qDebug() << "WebView: Repaint requested for " << paintEvent->rect()<<", region: "<<paintEvent->region();
 				if(lockPainter)
 				{
 					qDebug() << "WebView: Received paint event inside copy painter lock, ignoring";
 				}
 				else
 				{
 					if(paintEvent->rect().topLeft() == QPoint(0,0))
 					{
 						// QRegions in the paint event don't paint what we need when scrolling the view
 						repaintRegion = repaintRegion.united(QRect(QPoint(0,0),view->size()));
 					}
 					else
 					{
						repaintRegion = repaintRegion.united(paintEvent->region());
					}
					
					if(!repaintTimer.isActive())
					{
						qDebug() << "WebView: Starting repaint timer";
						repaintTimer.start();
					}
				}
 				
 			}
		} 
		// pass the event on to the parent class
		return QWidget::eventFilter(obj, event);
	}


private slots:
	void repaintView()
	{
		if(lockPainter)
		{
			qDebug() << "WebView: copy painter locked, ignoring";
		}
		else
		{
// 			if(repaintRegion.isEmpty())
// 			{
// 				qDebug() << "WebView: repaint region empty, ignoring";
// 				return;
// 			}
// 			
			static int repaints = 0;
			repaints++;
			qDebug() << "WebView: [ PAINT"<<repaints<<"] Repainting region:"<<repaintRegion;
			
			lockPainter = true;
			QPainter painter(&copyPixmap);
			view->render(&painter,repaintRegion.boundingRect().topLeft(),repaintRegion);
			painter.end();
			copyLabel->setPixmap(copyPixmap);
			lockPainter = false;
			
			repaintRegion = QRegion();

		}
	}

private:
	void paintEvent(QPaintEvent *event) 
	{
		QPainter p(this);
		p.fillRect(event->rect(), Qt::transparent);
		p.setPen(Qt::NoPen);
		p.setBrush(gradient);
		p.setOpacity(0.6);
		p.drawRoundedRect(rect(), 10, 10);
		p.end();
	}

private:
	QWebView *view;
	QLinearGradient gradient;
	QLabel * copyLabel;
	QPixmap copyPixmap;
	bool lockPainter;
	QRegion repaintRegion;
	QTimer repaintTimer;
};

#include "transparentweb.moc"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	Container w;
	//w.setAttribute(Qt::WA_TranslucentBackground, true);
	//w.setWindowFlags(Qt::FramelessWindowHint);
	w.show();
	return app.exec();
}
