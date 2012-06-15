VPATH += $$PWD
DEPENDPATH += $$PWD


FORMS += \
        TextProperties.ui \
	GenericItemConfig.ui
	
HEADERS += \
    AbstractContent.h \
    AbstractDisposeable.h \
    BezierCubicItem.h \
    ButtonItem.h \
    CornerItem.h \
    MirrorItem.h \
    StyledButtonItem.h \
    TextContent.h \
    TextProperties.h \
    SimpleTextContent.h  \
    TextBoxContent.h \
    TextBoxConfig.h \
    BoxContent.h \
    GenericItemConfig.h \
    BackgroundContent.h \
    BackgroundConfig.h \
    BoxConfig.h \
    ImageContent.h \
    ImageConfig.h \
    OutputViewContent.h \
    OutputViewConfig.h \
    #SlideGroupContent.h
 
    
SOURCES += \
    AbstractContent.cpp \
    AbstractDisposeable.cpp \
    BezierCubicItem.cpp \
    ButtonItem.cpp \
    CornerItem.cpp \
    MirrorItem.cpp \
    StyledButtonItem.cpp \
    TextContent.cpp \
    TextProperties.cpp \
    SimpleTextContent.cpp \
    TextBoxContent.cpp \
    TextBoxConfig.cpp \
    BoxContent.cpp \
    GenericItemConfig.cpp \
    BackgroundContent.cpp \
    BackgroundConfig.cpp \
    BoxConfig.cpp \
    ImageContent.cpp \
    ImageConfig.cpp \
    OutputViewContent.cpp \
    OutputViewConfig.cpp \
    #SlideGroupContent.cpp


DVIZ_HAS_QVIDEO: {
	
	HEADERS += \
		VideoFileContent.h \
		VideoFileConfig.h
		
	SOURCES += \
		VideoFileContent.cpp \
		VideoFileConfig.cpp
}
