VPATH += $$PWD
DEPENDPATH += $$PWD


FORMS += AbstractConfig.ui \
	TextProperties.ui 
	
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
    TextProperties.h 

    
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
    TextProperties.cpp