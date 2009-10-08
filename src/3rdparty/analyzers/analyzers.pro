TEMPLATE = app
TARGET = 
DEPENDPATH += . 
INCLUDEPATH += . 

# Input
HEADERS += analyzerbase.h \
           baranalyzer.h \
           blockanalyzer.h \
           fht.h
#boomanalyzer.h \
#glanalyzer.h \
#glanalyzer2.h \
#glanalyzer3.h \
#sonogram.h \
#turbine.h
SOURCES += analyzerbase.cpp \
           analyzerfactory.cpp \
           baranalyzer.cpp \
           blockanalyzer.cpp \
           fht.cpp
#boomanalyzer.cpp \
#glanalyzer.cpp \
#glanalyzer2.cpp \
#glanalyzer3.cpp \
#sonogram.cpp \
#turbine.cpp
