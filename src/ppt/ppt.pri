
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
	    HEADERS  = powerpointviewer.h PPTLoader.h
	    SOURCES  = powerpointviewer.cpp main.cpp PPTLoader.cpp NativeViewerWin32PPT.cpp
	}
}


HEADERS += NativeViewerWin32PPT.h
SOURCES += NativeViewerWin32PPT.cpp
