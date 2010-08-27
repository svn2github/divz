TEMPLATE = app
TARGET = livemix

DEPENDPATH += .
INCLUDEPATH += .

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

include(../qtcolorpicker/qtcolorpicker.pri)

QT += opengl network multimedia

win32 {
         CONFIG += console
    #debug {
    #}
}

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
#	MdiPreviewWidget.h \
	DVizSharedMemoryThread.h \
	MdiDVizWidget.h \
	../glvidtex/GLWidget.h \
	../glvidtex/GLDrawable.h \
	../glvidtex/GLVideoDrawable.h \
	../glvidtex/StaticVideoSource.h \
	../glvidtex/TextVideoSource.h \
	../ImageFilters.h \
	LiveScene.h \
	LiveLayer.h \
	LiveVideoInputLayer.h \
	LiveStaticSourceLayer.h \
	LiveTextLayer.h \
	ExpandableWidget.h \
	LayerControlWidget.h \
	LiveVideoLayer.h \
	LiveVideoFileLayer.h
	
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
#	MdiPreviewWidget.cpp \
	DVizSharedMemoryThread.cpp \
	MdiDVizWidget.cpp \
	../glvidtex/GLWidget.cpp \
	../glvidtex/GLDrawable.cpp \
	../glvidtex/GLVideoDrawable.cpp \
	../glvidtex/StaticVideoSource.cpp \
	../glvidtex/TextVideoSource.cpp \
	../ImageFilters.cpp \
	LiveScene.cpp \
	LiveLayer.cpp \
	LiveVideoInputLayer.cpp \
	LiveStaticSourceLayer.cpp \
	LiveTextLayer.cpp \
	ExpandableWidget.cpp \
	LayerControlWidget.cpp \ 
	LiveVideoLayer.cpp \
	LiveVideoFileLayer.cpp


unix {
	HEADERS += \
		../livemix/SimpleV4L2.h
	SOURCES += \
		../livemix/SimpleV4L2.cpp

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

