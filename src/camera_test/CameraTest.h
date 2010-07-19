#ifndef CameraTest_h
#define CameraTest_h

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QThread>
#include <QDebug>

class TestThread : public QThread
{
	Q_OBJECT
public:
	void run()
	{
		qDebug() << "TestThread::run: Test thread ID "<<QThread::currentThreadId();
		sleep(2); // Main thread runs fine during this sleep
		
		connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
		m_timer.setInterval(1000/30);
		m_timer.start();
		
		exec();
		
		//qDebug() << "TestThread::run: done running!";
	}
	
signals:
	void frameReady();
	
private slots:
	void tick()
	{
		qDebug() << "TestThread::tick: In ID "<<QThread::currentThreadId();
		sleep(50); // Main thread freezes during this sleep
		emit frameReady();
	}

private:
	QTimer m_timer;
};

class CameraTest : public QWidget
{
	Q_OBJECT
public:
	CameraTest() : QWidget(), m_frameCount(0)
	{
		qDebug() << "CameraTest::ctor: Main thread ID "<<QThread::currentThreadId();
		
		m_thread = new TestThread();
		connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()));
		m_thread->start();
		
		m_elapsedTime.start();
		
		connect(&m_testTimer, SIGNAL(timeout()), this, SLOT(frameReady()));
		m_testTimer.setInterval(1000/2);
		m_testTimer.start();
	}

public slots:
	void frameReady()
	{
		qDebug() << "CameraTest::frameReady: In ID "<<QThread::currentThreadId();
		update();
	}

protected:
	void paintEvent(QPaintEvent*)
	{
		qDebug() << "CameraTest::paintEvent: In ID "<<QThread::currentThreadId();
		QPainter p(this);
		p.fillRect(rect(), Qt::green);
		
		int sec = (m_elapsedTime.elapsed() / 1000);

		m_frameCount ++;
		int fps = (m_frameCount <= 0 ? 1 : m_frameCount) / (sec <= 0 ? 1 : sec);
		p.drawText(5,15,QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount));
	}

private:
	TestThread * m_thread;
	QTime m_elapsedTime;
	long m_frameCount;
	QTimer m_testTimer;
};

#endif //CameraTest_h
