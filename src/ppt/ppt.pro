TEMPLATE = app
TARGET	 = ppt_test
CONFIG	+= qaxcontainer

DUMP_ERROR = $$system(dumpcpp {FCE18141-B12B-11d0-B06A-00AA0060271A})

!isEmpty(DUMP_ERROR) {
    message("Microsoft PowerPointviewer type library not found!")
    REQUIRES += PowerPointViewer
} else {
    HEADERS  = powerpointviewer.h PPTLoader.h
    SOURCES  = powerpointviewer.cpp main.cpp PPTLoader.cpp
}

# install
//target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/qutlook
//sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS qutlook.pro
//sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/qutlook
//INSTALLS += target sources
