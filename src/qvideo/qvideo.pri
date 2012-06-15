VPATH += $$PWD
DEPENDPATH += $$PWD

DEFINES += DVIZ_HAS_QVIDEO
CONFIG  += DVIZ_HAS_QVIDEO

HEADERS +=  \
	QVideo.h \
	QVideoBuffer.h \
	QVideoDecoder.h \
	QVideoEncoder.h \ 
	QVideoProvider.h \
	../MjpegClient.h

SOURCES += \
    QVideo.cpp \
    QVideoBuffer.cpp \
    QVideoDecoder.cpp \
    QVideoEncoder.cpp \
    QVideoProvider.cpp \
    ../MjpegClient.cpp