TEMPLATE = app
TARGET = exiv2
SOURCES += src/exiv2.cpp \
	src/actions.cpp
	
HEADERS += src/exiv2.hpp \
	src/actions.hpp 

include(qt_build_root.pri)