
VPATH += $$PWD
DEPENDPATH += $$PWD

# for windows - Qt 4.6rc1
#LIBS += -lCamera4
# for linux cenos qt 2010.01
#LIBS += -lCamera


HEADERS += NativeViewerCamera.h \
	   CameraSlideGroup.h \
	   CameraSlideGroupFactory.h \
	   #../camera_test/CameraViewerWidget.h
		../livemix/VideoWidget.h \
		../livemix/CameraThread.h \
		../livemix/VideoSource.h \
		../livemix/SimpleV4L2.h
SOURCES += NativeViewerCamera.cpp \ 
	   CameraSlideGroup.cpp \
	   CameraSlideGroupFactory.cpp \
#	   ../camera_test/CameraViewerWidget.cpp
		../livemix/VideoWidget.cpp \
		../livemix/CameraThread.cpp \
		../livemix/VideoSource.cpp \
		../livemix/SimpleV4L2.cpp
