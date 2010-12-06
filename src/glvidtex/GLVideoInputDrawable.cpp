#include "GLVideoInputDrawable.h"

#include "../livemix/CameraThread.h"

GLVideoInputDrawable::GLVideoInputDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_source(0)
{
	if(!file.isEmpty())
		setVideoInput(file);
	
        //QTimer::singleShot(1500, this, SLOT(testXfade()));
}
	
void GLVideoInputDrawable::testXfade()
{
	qDebug() << "GLVideoInputDrawable::testXfade(): loading input /dev/video1";
	setVideoInput("/dev/video1");
	setCardInput("S-Video");
}
	
bool GLVideoInputDrawable::setVideoInput(const QString& camera)
{
	m_videoInput = camera;
	qDebug() << "GLVideoInputDrawable::setVideoInput(): camera:"<<camera;
	
	CameraThread *source = CameraThread::threadForCamera(camera.isEmpty() ?  DEFAULT_INPUT : camera);
	
	if(!source)
	{
		qDebug() << "GLVideoInputDrawable::setVideoInput: "<<camera<<" does not exist!";
		return false;
	}
	
	m_source = source;
	m_source->setFps(30);
// 	if(camera == "/dev/video1")
// 		source->setInput("S-Video");
	//usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently.
	// With the crash reproducable, I can now work to fix it.
	m_source->enableRawFrames(true);
	
	setVideoSource(source);
	
	return true;
}

QString GLVideoInputDrawable::cardInput()
{
	if(!m_source)
		return QString();
	QStringList list = cardInputs();
	return list[m_source->input()];
}

QStringList GLVideoInputDrawable::cardInputs()
{
	if(!m_source)
		return QStringList();
	return m_source->inputs();	
}

bool GLVideoInputDrawable::deinterlace()
{
	return m_source ? m_source->deinterlace() : false;
}

void GLVideoInputDrawable::setDeinterlace(bool flag)
{
	if(m_source)
		m_source->setDeinterlace(flag);
}

bool GLVideoInputDrawable::setCardInput(const QString& input)
{
	if(!m_source)
		return false;
	return m_source->setInput(input);
}
