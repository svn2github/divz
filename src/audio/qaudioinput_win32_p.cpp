/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//static int RandomGlobalVar = 0;

#include "qaudioinput_win32_p.h"

QT_BEGIN_NAMESPACE

//#define DEBUG_AUDIO 0

static CRITICAL_SECTION waveInCriticalSection;

QAudioInputPrivate::QAudioInputPrivate(const QByteArray &device, const QAudioFormat& audioFormat):
    settings(audioFormat)
{
    bytesAvailable = 0;
    buffer_size = 0;
    period_size = 0;
    m_device = device;
    totalTimeValue = 0;
    intervalTime = 1000;
    errorState = QAudio::NoError;
    deviceState = QAudio::StopState;
    audioSource = 0;
    pullMode = true;
    resuming = false;

    connect(this,SIGNAL(processMore()),SLOT(deviceReady()));

    InitializeCriticalSection(&waveInCriticalSection);
}

QAudioInputPrivate::~QAudioInputPrivate()
{
    close();
    DeleteCriticalSection(&waveInCriticalSection);
}

void CALLBACK QAudioInputPrivate::waveInProc( HWAVEIN hWaveIn, UINT uMsg,
        DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
    Q_UNUSED(dwParam1)
    Q_UNUSED(dwParam2)
    Q_UNUSED(hWaveIn)

    QAudioInputPrivate* qAudio;
    qAudio = (QAudioInputPrivate*)(dwInstance);
    if(!qAudio)
        return;

    switch(uMsg) {
        case WIM_OPEN:
            break;
        case WIM_DATA:
            EnterCriticalSection(&waveInCriticalSection);
            if(qAudio->waveFreeBlockCount > 0)
                qAudio->waveFreeBlockCount--;
            LeaveCriticalSection(&waveInCriticalSection);
            qAudio->feedback();
            break;
        case WIM_CLOSE:
            break;
        default:
            return;
    }
}

WAVEHDR* QAudioInputPrivate::allocateBlocks(int size, int count)
{
    int i;
    unsigned char* buffer;
    WAVEHDR* blocks;
    DWORD totalBufferSize = (size + sizeof(WAVEHDR))*count;

    if((buffer=(unsigned char*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,
                    totalBufferSize)) == 0) {
        qWarning("QAudioInput: Memory allocation error");
        return 0;
    }
    blocks = (WAVEHDR*)buffer;
    buffer += sizeof(WAVEHDR)*count;
    for(i = 0; i < count; i++) {
        blocks[i].dwBufferLength = size;
        blocks[i].lpData = (LPSTR)buffer;
        blocks[i].dwBytesRecorded=0;
        blocks[i].dwUser = 0L;
        blocks[i].dwFlags = 0L;
        blocks[i].dwLoops = 0L;
        result = waveInPrepareHeader(hWaveIn,&blocks[i], sizeof(WAVEHDR));
        if(result != MMSYSERR_NOERROR) {
            qWarning("QAudioInput: Can't prepare block %d",i);
            return 0;
        }
        buffer += size;
    }
    return blocks;
}

void QAudioInputPrivate::freeBlocks(WAVEHDR* blockArray)
{
    HeapFree(GetProcessHeap(), 0, blockArray);
}

QAudio::Error QAudioInputPrivate::error() const
{
    return errorState;
}

QAudio::State QAudioInputPrivate::state() const
{
    return deviceState;
}

QAudioFormat QAudioInputPrivate::format() const
{
    return settings;
}

QIODevice* QAudioInputPrivate::start(QIODevice* device)
{
    if(deviceState != QAudio::StopState)
        close();

    if(!pullMode && audioSource) {
        delete audioSource;
    }

    if(device) {
        //set to pull mode
        pullMode = true;
        audioSource = device;
    } else {
        //set to push mode
        pullMode = false;
        audioSource = new InputPrivate(this);
        audioSource->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
    }

    if( !open() )
        return 0;

    emit stateChanged(deviceState);

    return audioSource;
}

void QAudioInputPrivate::stop()
{
    if(deviceState == QAudio::StopState)
        return;

    deviceState = QAudio::StopState;

    close();
    emit stateChanged(deviceState);
}

bool QAudioInputPrivate::open()
{
#ifdef DEBUG_AUDIO
    QTime now(QTime::currentTime());
    qDebug()<<now.second()<<"s "<<now.msec()<<"ms :open()";
#endif
    header = 0;
    if(buffer_size == 0) {
        // Default buffer size, 100ms, default period size is 20ms
        buffer_size = settings.frequency()*settings.channels()*(settings.sampleSize()/8)*0.1;
	period_size = buffer_size/5;
    } else {
        period_size = buffer_size/5;
    }
    timeStamp.restart();
    wfx.nSamplesPerSec = settings.frequency();
    wfx.wBitsPerSample = settings.sampleSize();
    wfx.nChannels = settings.channels();
    wfx.cbSize = 0;

    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    UINT_PTR devId = WAVE_MAPPER;

    WAVEINCAPS wic;
    unsigned long iNumDevs,ii;
    iNumDevs = waveInGetNumDevs();
    for(ii=0;ii<iNumDevs;ii++) {
        if(waveInGetDevCaps(ii, &wic, sizeof(WAVEINCAPS))
	    == MMSYSERR_NOERROR) {
	    QString tmp;
	    tmp = QString::fromUtf16((const unsigned short*)wic.szPname);
	    if(tmp.compare(tr(m_device)) == 0) {
	        devId = ii;
		break;
	    }
	}
    }

    if(waveInOpen(&hWaveIn, devId, &wfx,
                (DWORD_PTR)&waveInProc,
                (DWORD_PTR) this,
                CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        errorState = QAudio::OpenError;
        deviceState = QAudio::StopState;
        emit stateChanged(deviceState);
        qWarning("QAudioInput: failed to open audio device");
        return false;
    }
    waveBlocks = allocateBlocks(period_size, buffer_size/period_size);
    waveFreeBlockCount = buffer_size/period_size;
    waveCurrentBlock = 0;

    for(int i=0; i<buffer_size/period_size; i++) {
        result = waveInAddBuffer(hWaveIn, &waveBlocks[i], sizeof(WAVEHDR));
        if(result != MMSYSERR_NOERROR) {
            qWarning("QAudioInput: failed to setup block %d,err=%d",i,result);
            errorState = QAudio::OpenError;
            deviceState = QAudio::StopState;
            emit stateChanged(deviceState);
            return false;
        }
    }
    result = waveInStart(hWaveIn);
    if(result) {
        qWarning("QAudioInput: failed to start audio input");
        errorState = QAudio::OpenError;
        deviceState = QAudio::StopState;
        emit stateChanged(deviceState);
        return false;
    }
    timeStampOpened.restart();
    totalTimeValue = 0;
    errorState  = QAudio::NoError;
    deviceState = QAudio::ActiveState;
    return true;
}

void QAudioInputPrivate::close()
{
    deviceState = QAudio::StopState;
    int delay = (buffer_size-bytesReady())*1000/(settings.frequency()
                  *settings.channels()*(settings.sampleSize()/8));
    waveInReset(hWaveIn);
    Sleep(delay+10);

    for(int i=0; i<waveFreeBlockCount; i++) {
        if(waveBlocks[i].dwFlags & WHDR_PREPARED)
            waveInUnprepareHeader(hWaveIn,&waveBlocks[i],sizeof(WAVEHDR));
    }
    freeBlocks(waveBlocks);
    waveInClose(hWaveIn);
}

int QAudioInputPrivate::bytesReady() const
{
    int buf = ((buffer_size/period_size)-waveFreeBlockCount)*period_size;
    if(buf < 0)
        buf = 0;
    return buf;
}

qint64 QAudioInputPrivate::read(char* data, qint64 len)
{
    bool done = false;

    char*  p = data;
    qint64 l = 0;
    qint64 written = 0;
    while(!done) {
        // Read in some audio data
        if(waveBlocks[header].dwBytesRecorded > 0) {
            if(pullMode) {
                l = audioSource->write(waveBlocks[header].lpData,
                        waveBlocks[header].dwBytesRecorded);
#ifdef DEBUG_AUDIO
		qDebug()<<"[A] IN: "<<waveBlocks[header].dwBytesRecorded<<", OUT: "<<l;
#endif
                if(l < 0) {
                    // error
                    qWarning("QAudioInput: IOError");
                    errorState = QAudio::IOError;

                } else if(l == 0) {
                    // cant write to IODevice
                    qWarning("QAudioInput: IOError, can't write to QIODevice");
                    errorState = QAudio::IOError;

                } else {
                    totalTimeValue += waveBlocks[header].dwBytesRecorded
                        /((settings.channels()*settings.sampleSize()/8))
                        *10000/settings.frequency()*100;
                    errorState = QAudio::NoError;
                    deviceState = QAudio::ActiveState;
		    resuming = false;
                }
            } else {
                // push mode
                memcpy(p,waveBlocks[header].lpData,waveBlocks[header].dwBytesRecorded);
                l = waveBlocks[header].dwBytesRecorded;
#ifdef DEBUG_AUDIO
		qDebug()<<"[B] IN: "<<waveBlocks[header].dwBytesRecorded<<", OUT: "<<l;
#endif
                totalTimeValue += waveBlocks[header].dwBytesRecorded
                    /((settings.channels()*settings.sampleSize()/8))
                    *10000/settings.frequency()*100;
                errorState = QAudio::NoError;
                deviceState = QAudio::ActiveState;
		resuming = false;
            }
        } else {
            //no data, not ready yet, next time
            return 0;
        }
        EnterCriticalSection(&waveInCriticalSection);
        waveFreeBlockCount++;
        LeaveCriticalSection(&waveInCriticalSection);
        waveBlocks[header].dwBytesRecorded=0;
        waveBlocks[header].dwFlags = 0L;
        result = waveInPrepareHeader(hWaveIn,&waveBlocks[header], sizeof(WAVEHDR));
        if(result != MMSYSERR_NOERROR) {
            qWarning("QAudioInput: failed to prepare block %d,err=%d",header,result);
            errorState = QAudio::OpenError;
            deviceState = QAudio::StopState;
            emit stateChanged(deviceState);
        }
        result = waveInAddBuffer(hWaveIn, &waveBlocks[header], sizeof(WAVEHDR));
        if(result != MMSYSERR_NOERROR) {
            qWarning("QAudioInput: failed to setup block %d,err=%d",header,result);
            errorState = QAudio::OpenError;
            deviceState = QAudio::StopState;
            emit stateChanged(deviceState);
        }
        header++;
        if(header >= buffer_size/period_size)
            header = 0;
        p+=l;

        if(!pullMode) {
	    if(l+period_size > len && waveFreeBlockCount == buffer_size/period_size)
	        done = true;
	} else {
	    if(waveFreeBlockCount == buffer_size/period_size)
	        done = true;
	}
	written+=l;
    }
#ifdef DEBUG_AUDIO
    qDebug()<<"read in len="<<written;
#endif
    return written;
}

void QAudioInputPrivate::resume()
{
    if(deviceState == QAudio::SuspendState) {
        deviceState = QAudio::ActiveState;
        for(int i=0; i<buffer_size/period_size; i++) {
            result = waveInAddBuffer(hWaveIn, &waveBlocks[i], sizeof(WAVEHDR));
            if(result != MMSYSERR_NOERROR) {
                qWarning("QAudioInput: failed to setup block %d,err=%d",i,result);
                errorState = QAudio::OpenError;
                deviceState = QAudio::StopState;
                emit stateChanged(deviceState);
                return;
            }
        }
        waveFreeBlockCount = buffer_size/period_size;
        waveCurrentBlock = 0;
        header = 0;
	resuming = true;
        waveInStart(hWaveIn);
        QTimer::singleShot(20,this,SLOT(feedback()));
        emit stateChanged(deviceState);
    }
}

void QAudioInputPrivate::setBufferSize(int value)
{
    buffer_size = value;
}

int QAudioInputPrivate::bufferSize() const
{
    return buffer_size;
}

int QAudioInputPrivate::periodSize() const
{
    return period_size;
}

void QAudioInputPrivate::setNotifyInterval(int ms)
{
    intervalTime = ms;
}

int QAudioInputPrivate::notifyInterval() const
{
    return intervalTime;
}

qint64 QAudioInputPrivate::totalTime() const
{
    return totalTimeValue;
}

void QAudioInputPrivate::suspend()
{
    if(deviceState == QAudio::ActiveState) {
        waveInReset(hWaveIn);
        deviceState = QAudio::SuspendState;
        emit stateChanged(deviceState);
    }
}

void QAudioInputPrivate::feedback()
{

    bytesAvailable = bytesReady();
#ifdef DEBUG_AUDIO
    QTime now(QTime::currentTime());
    qDebug()<<now.second()<<"s "<<now.msec()<<"ms :feedback() INPUT: bytesAvailable:"<<bytesAvailable;
#endif
    if(!(deviceState==QAudio::StopState||deviceState==QAudio::SuspendState))
        emit processMore();
}

bool QAudioInputPrivate::deviceReady()
{
#ifdef DEBUG_AUDIO
    QTime now(QTime::currentTime());
    qDebug()<<now.second()<<"s "<<now.msec()<<"ms :deviceReady() INPUT";
#endif
    if(pullMode) {
        // reads some audio data and writes it to QIODevice
        read(0,0);
    } else {
        // emits readyRead() so user will call read() on QIODevice to get some audio data
	InputPrivate* a = qobject_cast<InputPrivate*>(audioSource);
	a->trigger();
    }
    if(deviceState != QAudio::ActiveState)
        return true;

    if(timeStamp.elapsed() > intervalTime && intervalTime > 50) {
        emit notify();
        timeStamp.restart();
    }
    return true;
}

qint64 QAudioInputPrivate::clock() const
{
    if(deviceState != QAudio::ActiveState)
        return 0;

    return timeStampOpened.elapsed();
}

void QAudioInputPrivate::reset()
{
    close();
}

InputPrivate::InputPrivate(QAudioInputPrivate* audio)
{
    audioDevice = qobject_cast<QAudioInputPrivate*>(audio);
}

InputPrivate::~InputPrivate() {}

qint64 InputPrivate::readData( char* data, qint64 len)
{
    // push mode, user read() called
    if(audioDevice->deviceState != QAudio::ActiveState)
        return 0;
    // Read in some audio data
    return audioDevice->read(data,len);
}

qint64 InputPrivate::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)

    emit readyRead();
    return 0;
}

void InputPrivate::trigger()
{
    emit readyRead();
}

QT_END_NAMESPACE
