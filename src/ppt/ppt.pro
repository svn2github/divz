TEMPLATE = app
TARGET	 = ppt_test
CONFIG	+= qaxcontainer

//DUMP_ERROR = $$system(dumpcpp {FCE18141-B12B-11d0-B06A-00AA0060271A})

    DEFINES += WIN32_PPT_ENABLED
    HEADERS  = PPTLoader.h msscriptcontrol.h
    SOURCES  = main.cpp PPTLoader.cpp msscriptcontrol.cpp


# install
//target.path = $$[QT_INSTALL_EXAMPLES]/activeqt/qutlook
//sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS qutlook.pro
//sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt/qutlook
//INSTALLS += target sources
