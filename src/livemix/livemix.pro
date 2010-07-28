TEMPLATE = app
TARGET = livemix

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

QT += opengl network

# Input
HEADERS += CameraThread.h \
	CameraServer.h \
	VideoWidget.h \
	VideoSource.h \
	VideoThread.h \
	VideoFrame.h \
	MjpegThread.h \
	MainWindow.h \
	MdiChild.h \
	MdiVideoChild.h \
	MdiVideoWidget.h \
	MdiCameraWidget.h \
	MdiMjpegWidget.h \
	MdiPreviewWidget.h \
	VideoOutputWidget.h \
	GLVideoThread.h \
	../qq06-glthread/GLWidget.h \
	../qq06-glthread/GLThread.h
	
SOURCES += main.cpp \
	CameraThread.cpp \
	CameraServer.cpp \
	VideoWidget.cpp \
	VideoSource.cpp \
	VideoThread.cpp \
	VideoFrame.cpp \
	MjpegThread.cpp \
	MainWindow.cpp \
	MdiChild.cpp \
	MdiVideoChild.cpp \
	MdiVideoWidget.cpp \
	MdiCameraWidget.cpp \
	MdiMjpegWidget.cpp \
	MdiPreviewWidget.cpp \
	VideoOutputWidget.cpp \
	GLVideoThread.cpp \
	../qq06-glthread/GLWidget.cpp \
	../qq06-glthread/GLThread.cpp

unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2 
}

win32 {
	INCLUDEPATH += \
		../external/ffmpeg/include/msinttypes \
		../external/ffmpeg/include/libswscale \
		../external/ffmpeg/include/libavutil \
		../external/ffmpeg/include/libavdevice \
		../external/ffmpeg/include/libavformat \
		../external/ffmpeg/include/libavcodec \
		../external/ffmpeg/include
	
	LIBS += -L"../external/ffmpeg/lib" \
		-lavcodec-51 \
		-lavformat-52 \
		-lavutil-49 \
		-lavdevice-52 \
		-lswscale-0
}

