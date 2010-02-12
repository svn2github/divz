VPATH += $$PWD
DEPENDPATH += $$PWD

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