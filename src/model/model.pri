VPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS +=  \
	AbstractItem.h \
	AbstractVisualItem.h \
	TextItem.h \
	Slide.h  \
	ItemFactory.h \
	TextBoxItem.h \
        BoxItem.h \
        VideoFileItem.h \
        SlideGroup.h \
        Document.h \
        Output.h \
        SlideGroupFactory.h

    
SOURCES += \
	AbstractItem.cpp \
	AbstractVisualItem.cpp \
	TextItem.cpp \
	Slide.cpp \
	ItemFactory.cpp \
	TextBoxItem.cpp \
        BoxItem.cpp \
        VideoFileItem.cpp \
        SlideGroup.cpp \
        Document.cpp \
        Output.cpp \
        SlideGroupFactory.cpp
