
VPATH += $$PWD
DEPENDPATH += $$PWD

win32 {

	DEFINES += WIN32_PPT_ENABLED
	
	CONFIG	+= qaxcontainer

	DUMP_ERROR = $$system(dumpcpp {FCE18141-B12B-11d0-B06A-00AA0060271A})

	!isEmpty(DUMP_ERROR) {
	    message("Microsoft PowerPointviewer type library not found!")
	    REQUIRES += PowerPointViewer
	} else {
	    HEADERS  += powerpointviewer.h msscriptcontrol.h
	    SOURCES  += powerpointviewer.cpp msscriptcontrol.cpp
	}
}


HEADERS += NativeViewerWin32PPT.h \
	   PPTLoader.h \
	   PPTSlideGroup.h \
	   PPTSlideGroupFactory.h
SOURCES += NativeViewerWin32PPT.cpp \ 
	   PPTLoader.cpp \
	   PPTSlideGroup.cpp \
	   PPTSlideGroupFactory.cpp
