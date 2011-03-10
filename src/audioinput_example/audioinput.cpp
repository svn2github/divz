/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdlib.h>
#include <math.h>

#include <QLabel>
#include <QSpinBox>
#include <QDebug>
#include <QPainter>
#include <QVBoxLayout>

#include "qtmultimedia/audio/qaudiodeviceinfo.h"
#include "qtmultimedia/audio/qaudioinput.h"
#include "audioinput.h"


#define BUFFER_SIZE 4096

// Max 60 seconds
#define OUT_BUFF_LEN_MS 60 * 1000
#define OUT_FREQ 8000
#define OUT_MS_SIZE (OUT_FREQ / 1000)
#define OUT_BUFF_SIZE (OUT_MS_SIZE * OUT_BUFF_LEN_MS)

AudioInfo::AudioInfo(QObject* parent, QAudioInput* device, Analyzer::Base *an)
    :QIODevice( parent ),
    analyzer(an)

{
    input = device;

    m_maxValue = 0;

    outputFile.setFileName("/test.pcm");
    outputFile.open( QIODevice::WriteOnly | QIODevice::Truncate );

	buffer = (char*)malloc(sizeof(char) * OUT_BUFF_SIZE);
	bufferWritePos = 0;
	bufferLengthMs = 1500; //1.5 sec
}

AudioInfo::~AudioInfo()
{
    outputFile.close();
    delete buffer;
}

void AudioInfo::start(QIODevice *output)
{
    open(QIODevice::WriteOnly);
    outputDev = output;
}

void AudioInfo::stop()
{
    close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return 0;
}

/*
#define OUT_BUFF_LEN_MS 500
#define OUT_FREQ 8000
#define OUT_MS_SIZE (OUT_FREQ / 1000)
#define OUT_BUFF_SIZE (OUT_MS_SIZE * OUT_BUFF_LEN_MS)
*/

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
    int samples = len/2; // 2 bytes per sample
    int maxAmp = 32768; // max for S16 samples
    bool clipping = false;

    //outputFile.write(data,len);

    m_maxValue = 0;

    qint16* s = (qint16*)data;

    int bufferTop = OUT_MS_SIZE * bufferLengthMs;
    if(bufferTop > len)
    {
		if(bufferWritePos >= bufferTop) //OUT_BUFF_SIZE)
		{
			qint64 bytes = len < bufferTop ? len : bufferTop;
			outputDev->write(buffer, bytes);
			qDebug() << "wrote "<<bytes<<" output";

			// move data from end to start
			// e.g.:
			// here's the buffer:
			// |xxxxxxxxxx|yyyyyyyyyy|
			// We just wrote 'xxx...' to the output device above,
			// so here we copy 'yyyy...' over top of 'xxxx..'...
			char *from = buffer;
			from += len;
			memcpy(buffer, from, bufferTop - len);

			bufferWritePos = bufferTop - len;
		}


		// Now we write over top of where 'yyyy...' just was in the buffer...

		// buff: 200
		// len: 30
		// now we have a 30 byte hole at end...
		char *writeTo = buffer;
		writeTo += bufferWritePos;

		if(len + bufferWritePos > bufferTop)
		{
			len = bufferTop - bufferWritePos;
		}

		bufferWritePos += len;
		qDebug() << "buffered "<<len<<" bytes, buffer at "<<bufferWritePos<<"/"<<bufferTop;

		memcpy(writeTo, data, len);
}


	scope.clear();

    // sample format is S16LE, only!

    for(int i=0;i<samples;i++) {
        qint16 sample = *s;
        s++;
        if(abs(sample) > m_maxValue) m_maxValue = abs(sample);

        float value = ((float)abs(sample)/(float)maxAmp);
		if(value > 1)
			value = 1;
		scope.push_back(value);
    }
    // check for clipping
    if(m_maxValue>=(maxAmp-1)) clipping = true;

    float value = ((float)m_maxValue/(float)maxAmp);
    if(clipping) m_maxValue = 100;
    else m_maxValue = (int)(value*100);

    emit update();

    if(analyzer)
    	analyzer->analyze(scope);

    return len;
}

void AudioInfo::setBufferMs(int ms)
{
	bufferLengthMs = ms;
}

int AudioInfo::LinearMax()
{
    return m_maxValue;
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    level = 0;
    setMinimumHeight(30);
    setMinimumWidth(200);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.drawRect(QRect(painter.viewport().left()+10, painter.viewport().top()+10,
                painter.viewport().right()-20, painter.viewport().bottom()-20));

    if(level == 0)
        return;

    painter.setPen(Qt::red);

    int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*level/100;
    int x1,y1,x2,y2;
    for(int i=0;i<10;i++) {
        x1 = painter.viewport().left()+11;
        y1 = painter.viewport().top()+10+i;
        x2 = painter.viewport().left()+20+pos;
        y2 = painter.viewport().top()+10+i;
        if(x2 < painter.viewport().left()+10)
            x2 = painter.viewport().left()+10;

        painter.drawLine(QPoint(x1,y1),QPoint(x2,y2));
    }
}

void RenderArea::setLevel(int value)
{
    level = value;
    repaint();
}


InputTest::InputTest()
{
    setWindowTitle("Audio Delay Buffer");

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    /*
    canvas = new RenderArea;
    layout->addWidget(canvas);
    */

    layout->setContentsMargins(0,0,0,0);

    BarAnalyzer * analyzer = new BarAnalyzer(this);
    layout->addWidget(analyzer);
    /*

    deviceBox = new QComboBox(this);
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(devices.at(i).deviceName(), qVariantFromValue(devices.at(i)));
    }
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click for Push Mode"));
    connect(button,SIGNAL(clicked()),SLOT(toggleMode()));
    layout->addWidget(button);

    button2 = new QPushButton(this);
    button2->setText(tr("Click To Suspend"));
    connect(button2,SIGNAL(clicked()),SLOT(toggleSuspend()));
    layout->addWidget(button2);
    */

    QHBoxLayout *hbox = new QHBoxLayout();
    hbox->addWidget(new QLabel("Buffer Milliseconds:"));

    QSlider *slider = new QSlider();
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum((int)40);
	slider->setMaximum((int)5 * 1000);
	slider->setSingleStep(5);
	slider->setPageStep(10);
	hbox->addWidget(slider);

	QSpinBox *spin = new QSpinBox();
	spin->setMinimum(slider->minimum());
	spin->setMaximum(slider->maximum());
	spin->setSuffix(" ms");
	hbox->addWidget(spin);

	QObject::connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
	QObject::connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));

	spin->setValue(1500);
	slider->setValue(1500);

	layout->addLayout(hbox);


    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    buffer = new char[BUFFER_SIZE];

    pullMode = true;

    // AudioInfo class only supports mono S16LE samples!
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec(QLatin1String("audio/pcm"));

    audioInput = new QAudioInput(format,this);
    connect(audioInput,SIGNAL(notify()),SLOT(status()));
    connect(audioInput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    audioinfo  = new AudioInfo(this,audioInput,analyzer);
    //connect(audioinfo,SIGNAL(update()),SLOT(refreshDisplay()));

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
	if (!info.isFormatSupported(format)) {
	     qWarning()<<"raw audio format not supported by backend, cannot play audio.";
	    //return;
	}

	audioOutput = new QAudioOutput(format, this);
	//connect(audio,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
	output = audioOutput->start();

	QObject::connect(slider, SIGNAL(valueChanged(int)), audioinfo, SLOT(setBufferMs(int)));

    audioinfo->start(output);
    audioInput->start(audioinfo);

    setWindowFlags(Qt::WindowStaysOnTopHint);

}

InputTest::~InputTest() {}

void InputTest::status()
{
    qWarning()<<"bytesReady = "<<audioInput->bytesReady()<<" bytes, clock = "<<audioInput->clock()<<"ms, totalTime = "<<audioInput->totalTime()/1000<<"ms";
}

void InputTest::readMore()
{
    if(!audioInput)
        return;
    qint64 len = audioInput->bytesReady();
    if(len > 4096)
        len = 4096;
    qint64 l = input->read(buffer,len);
    if(l > 0) {
        audioinfo->write(buffer,l);
    }
}

void InputTest::toggleMode()
{
    // Change bewteen pull and push modes
    audioInput->stop();

    if (pullMode) {
        button->setText(tr("Click for Pull Mode"));
        input = audioInput->start(0);
        connect(input,SIGNAL(readyRead()),SLOT(readMore()));
        pullMode = false;
    } else {
        button->setText(tr("Click for Push Mode"));
        pullMode = true;
        audioInput->start(audioinfo);
    }
}

void InputTest::toggleSuspend()
{
    // toggle suspend/resume
    if(audioInput->state() == QAudio::SuspendState) {
        qWarning()<<"status: Suspended, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::ActiveState) {
        qWarning()<<"status: Active, suspend()";
        audioInput->suspend();
        button2->setText("Click To Resume");
    } else if (audioInput->state() == QAudio::StopState) {
        qWarning()<<"status: Stopped, resume()";
        audioInput->resume();
        button2->setText("Click To Suspend");
    } else if (audioInput->state() == QAudio::IdleState) {
        qWarning()<<"status: IdleState";
    }
}

void InputTest::state(QAudio::State state)
{
    qWarning()<<" state="<<state;
}

void InputTest::refreshDisplay()
{
    canvas->setLevel(audioinfo->LinearMax());
    canvas->repaint();
}

void InputTest::deviceChanged(int idx)
{
	/*
    audioinfo->stop();
    audioInput->stop();
    audioInput->disconnect(this);
    delete audioInput;

    device = deviceBox->itemData(idx).value<QAudioDeviceInfo>();
    audioInput = new QAudioInput(device, format, this);
    connect(audioInput,SIGNAL(notify()),SLOT(status()));
    connect(audioInput,SIGNAL(stateChanged(QAudio::State)),SLOT(state(QAudio::State)));
    audioinfo->start();
    audioInput->start(audioinfo);
    */
}
