TEMPLATE = app
TARGET = livemix

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

QT += opengl network multimedia

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
	DVizSharedMemoryThread.h \
	MdiDVizWidget.h \
	../glvidtex/GLWidget.h \
	../glvidtex/GLDrawable.h \
	../glvidtex/GLVideoDrawable.h
	
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
	DVizSharedMemoryThread.cpp \
	MdiDVizWidget.cpp \
	SimpleV4L2.cpp \
	../glvidtex/GLWidget.cpp \
	../glvidtex/GLDrawable.cpp \
	../glvidtex/GLVideoDrawable.cpp

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

