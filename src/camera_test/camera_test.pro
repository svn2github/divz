TEMPLATE = app
TARGET = camera_test
DEPENDPATH += .
INCLUDEPATH += .
QT += opengl  network


# Input
HEADERS += CameraThread.h \
	CameraServer.h \
	CameraViewerWidget.h
	
	
SOURCES += main.cpp \
	CameraThread.cpp \
	CameraServer.cpp \
	CameraViewerWidget.cpp


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

