/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>

// #include "window.h"
#include "GLWidget.h"
#include "../livemix/VideoThread.h"
#include "../livemix/CameraThread.h"


int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(glvidtex);

    QApplication app(argc, argv);
//     Window window;
//     window.show();
	GLWidget w;
	
	
	
	#ifdef Q_OS_WIN
	QString defaultCamera = "vfwcap://0";
	#else
	QString defaultCamera = "/dev/video0";
	#endif

	CameraThread *thread = CameraThread::threadForCamera(defaultCamera);
	if(thread)
	{
		thread->setFps(30);
		//usleep(250 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently. 
		// With the crash reproducable, I can now work to fix it.
		thread->enableRawFrames(true);
		//thread->setDeinterlace(true);
		//m_source = thread;
		
		GLVideoDrawable *camera = new GLVideoDrawable(&w);
		camera->setVideoSource(thread);
		camera->setRect(w.viewport());
		w.addDrawable(camera);
		camera->addShowAnimation(GLDrawable::AnimFade);
		//camera->addShowAnimation(GLDrawable::AnimZoom);
		camera->addShowAnimation(GLDrawable::AnimSlideTop,1000);
		camera->addHideAnimation(GLDrawable::AnimSlideBottom,1000);
		//camera->addHideAnimation(GLDrawable::AnimZoom);
		camera->show();
		
		QTimer *timer = new QTimer;
		QObject::connect(timer, SIGNAL(timeout()), camera, SLOT(hide()));
		timer->start(5000);
		timer->setSingleShot(true);
		
// 		VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(camera);
// 		opts->adjustSize();
// 		opts->show();
	}
	
	// add secondary frame
	{
		GLVideoDrawable *videoBug = new GLVideoDrawable(&w);
		
		
		StaticVideoSource *source = new StaticVideoSource();
		source->setImage(QImage("me2.jpg"));
		//source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));
		
		source->start();
		videoBug->setVideoSource(source);
		videoBug->setRect(w.viewport());
		videoBug->setZIndex(-1);
		//videoBug->setOpacity(0.5);
		//videoBug->show();
		
		
		//videoBug->addShowAnimation(GLDrawable::AnimFade);
		videoBug->addShowAnimation(GLDrawable::AnimSlideTop,1000);
		//videoBug->addShowAnimation(GLDrawable::AnimZoom);
		videoBug->addHideAnimation(GLDrawable::AnimFade);
		videoBug->addHideAnimation(GLDrawable::AnimZoom);
		
		QTimer *timer = new QTimer;
		QObject::connect(timer, SIGNAL(timeout()), videoBug, SLOT(show()));
		timer->start(5000);
		timer->setSingleShot(true);
		
		timer = new QTimer;
		QObject::connect(timer, SIGNAL(timeout()), videoBug, SLOT(hide()));
		timer->start(8000);
		timer->setSingleShot(true);
		
		w.addDrawable(videoBug);
	}
	
// 	if(!thread)
// 	{
// 		VideoThread * thread = new VideoThread();
// 		thread->setVideo("../data/Seasons_Loop_3_SD.mpg");
// 		thread->start();
// 		m_source = thread;
// 	}

	GLVideoDrawable *videoBug = new GLVideoDrawable(&w);
	
	
	StaticVideoSource *source = new StaticVideoSource();
	//source->setImage(QImage("me2.jpg"));
	source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));
	
	source->start();
	videoBug->setVideoSource(source);
	videoBug->setRect(QRectF(
		w.viewport().right() - source->image().width() * 1.25,
		w.viewport().bottom() - source->image().height() * 1.25,
		source->image().width(),
		source->image().height()));
	videoBug->setZIndex(1);
	videoBug->setOpacity(0.5);
	//videoBug->show();
	
	videoBug->addShowAnimation(GLDrawable::AnimSlideLeft,1000).curve = QEasingCurve::OutElastic;
	
	videoBug->addHideAnimation(GLDrawable::AnimFade,300);
	videoBug->addHideAnimation(GLDrawable::AnimSlideTop,300);
	
	QTimer * timer = new QTimer;
	QObject::connect(timer, SIGNAL(timeout()), videoBug, SLOT(show()));
	timer->start(1000);
	timer->setSingleShot(true);
	
	timer = new QTimer;
	QObject::connect(timer, SIGNAL(timeout()), videoBug, SLOT(hide()));
	timer->start(6000);
	timer->setSingleShot(true);
	
	w.addDrawable(videoBug);
	
	w.resize(640,480);
	
	w.show();
    return app.exec();
}
