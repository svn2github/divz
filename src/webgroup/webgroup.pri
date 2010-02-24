
VPATH += $$PWD
DEPENDPATH += $$PWD

QT      +=  webkit network

HEADERS += NativeViewerWebKit.h \
	   WebSlideGroup.h \
	   WebSlideGroupFactory.h
SOURCES += NativeViewerWebKit.cpp \ 
	   WebSlideGroup.cpp \
	   WebSlideGroupFactory.cpp
