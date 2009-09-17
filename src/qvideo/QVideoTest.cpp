#include "QVideoTest.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "QVideo.h"

QVideoTest::QVideoTest(QString file,QWidget * parent) : QWidget(parent)
{
	m_video = new QVideo(this);
	//if(!m_video->load("Abstract_Loop_2_SD.mpg")) //Abstract_Countdown_1_SD.mpg")) //Seasons_Loop_4_SD.mpg"))
	//if(!m_video->load("Abstract_Countdown_3_SD.mpg")) //Seasons_Loop_4_SD.mpg"))
	if(!m_video->load(file)) //Seasons_Loop_4_SD.mpg"))
		return;

	m_video->setAdvanceMode(QVideo::Manual);

	m_video->setLooped(true);
	connect(m_video, SIGNAL(currentFrame(QFFMpegVideoFrame)), this, SLOT(newFrame(QFFMpegVideoFrame)));

	//m_screen = m_video->screen();
	m_screen = new QLabel();

	QPushButton * play_button = new QPushButton("Play", this);
	connect(play_button, SIGNAL(clicked()), m_video, SLOT(play()));

	QPushButton * pause_button = new QPushButton("Pause", this);
	connect(pause_button, SIGNAL(clicked()), m_video, SLOT(pause()));

	QPushButton * stop_button = new QPushButton("Stop", this);
	connect(stop_button, SIGNAL(clicked()), m_video, SLOT(stop()));

// 	QPushButton * movie_button = new QPushButton("Make Movie", this);
// 	connect(movie_button, SIGNAL(clicked()), m_video, SLOT(makeMovie()));

	QPushButton * jump_button = new QPushButton("Jump", this);
	connect(jump_button, SIGNAL(clicked()), this, SLOT(jump()));

	
	QHBoxLayout * button_layout = new QHBoxLayout();
	button_layout->addWidget(play_button);
	button_layout->addWidget(pause_button);
	button_layout->addWidget(stop_button);
	//button_layout->addWidget(movie_button);
	button_layout->addWidget(jump_button);

	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(m_screen);
	layout->addLayout(button_layout);

	this->setLayout(layout);

	//m_elapsed_time.start();
	//startTimer(m_video->fps());
}

QVideoTest::~QVideoTest()
{
	delete m_screen;
	m_screen = 0;
}


void QVideoTest::jump()
{
// 	//printf("Hit timer\n");
	//printf("New Frame!\n");
// 	int elapsed = m_elapsed_time.elapsed();
	m_video->advance(1000);
// 	m_elapsed_time.restart();
}

void QVideoTest::newFrame(QFFMpegVideoFrame f)
{
// 	//printf("Hit timer\n");
	//printf("New Frame!\n");
// 	int elapsed = m_elapsed_time.elapsed();
// 	QImage image = m_video->advance(elapsed);
// 	m_elapsed_time.restart();

	QImage image = *(f.frame);

	m_screen->setPixmap(QPixmap::fromImage(image));
	m_screen->adjustSize();
}
