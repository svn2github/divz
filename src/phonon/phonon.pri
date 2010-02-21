
VPATH += $$PWD
DEPENDPATH += $$PWD

# for windows - Qt 4.6rc1
#LIBS += -lphonon4
# for linux cenos qt 2010.01
LIBS += -lphonon


HEADERS += NativeViewerPhonon.h \
	   VideoSlideGroup.h \
	   VideoSlideGroupFactory.h
SOURCES += NativeViewerPhonon.cpp \ 
	   VideoSlideGroup.cpp \
	   VideoSlideGroupFactory.cpp
