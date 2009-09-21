VPATH += $$PWD
DEPENDPATH += $$PWD


FORMS += AbstractConfig.ui \
	TextProperties.ui \
	GenericItemConfig.ui
	
HEADERS +=  AbstractConfig.h \
    AbstractContent.h \
    AbstractDisposeable.h \
    BezierCubicItem.h \
    ButtonItem.h \
    CornerItem.h \
    MirrorItem.h \
    StyledButtonItem.h \
    TextConfig.h \
    TextContent.h \
    TextProperties.h \
    SimpleTextContent.h  \
    TextBoxContent.h \
    TextBoxConfig.h \
    BoxContent.h \
    VideoFileContent.h \
    GenericItemConfig.h \
    VideoFileConfig.h
 
    
SOURCES += \
    AbstractConfig.cpp \
    AbstractContent.cpp \
    AbstractDisposeable.cpp \
    BezierCubicItem.cpp \
    ButtonItem.cpp \
    CornerItem.cpp \
    MirrorItem.cpp \
    StyledButtonItem.cpp \
    TextConfig.cpp \
    TextContent.cpp \
    TextProperties.cpp \
    SimpleTextContent.cpp \
    TextBoxContent.cpp \
    TextBoxConfig.cpp \
    BoxContent.cpp \
    VideoFileContent.cpp \
    GenericItemConfig.cpp \
    VideoFileConfig.cpp
