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
        SlideGroup.h \
        Document.h \
        Output.h \
        SlideGroupFactory.h \
        BackgroundItem.h \
        ImageItem.h \
        AbstractItemFilter.h \
        OutputViewItem.h \
        SlideTemplateManager.h

    
SOURCES += \
	AbstractItem.cpp \
	AbstractVisualItem.cpp \
	TextItem.cpp \
	Slide.cpp \
	ItemFactory.cpp \
	TextBoxItem.cpp \
        BoxItem.cpp \
        SlideGroup.cpp \
        Document.cpp \
        Output.cpp \
        SlideGroupFactory.cpp \
        BackgroundItem.cpp \
        ImageItem.cpp \
        AbstractItemFilter.cpp \
        OutputViewItem.cpp \
        SlideTemplateManager.cpp

DVIZ_HAS_QVIDEO {

	HEADERS += \
		VideoFileItem.h
	SOURCES += \
		VideoFileItem.cpp

}