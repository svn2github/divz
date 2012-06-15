
VPATH += $$PWD
DEPENDPATH += $$PWD

# for windows - Qt 4.6rc1
#LIBS += -lCamera4
# for linux cenos qt 2010.01
#LIBS += -lCamera

DEFINES += DVIZ_HAS_CAMERA

HEADERS += NativeViewerCamera.h \
	   CameraSlideGroup.h \
	   CameraSlideGroupFactory.h \
		../livemix/VideoWidget.h \
		../livemix/CameraThread.h \
		../livemix/VideoSource.h 
SOURCES += NativeViewerCamera.cpp \ 
	   CameraSlideGroup.cpp \
	   CameraSlideGroupFactory.cpp \
		../livemix/VideoWidget.cpp \
		../livemix/CameraThread.cpp \
		../livemix/VideoSource.cpp 

unix: {
	HEADERS += \
		../livemix/SimpleV4L2.h
	SOURCES += \
		../livemix/SimpleV4L2.cpp
}
