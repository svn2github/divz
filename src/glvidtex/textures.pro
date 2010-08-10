
MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build



HEADERS       = glwidget.h \
		window.h \
		../livemix/VideoSource.h \
		../livemix/VideoThread.h \
		../livemix/VideoFrame.h \
		../livemix/CameraThread.h
		
SOURCES       = glwidget.cpp \
		main.cpp \
		window.cpp \
		../livemix/VideoSource.cpp \
		../livemix/VideoThread.cpp \
		../livemix/VideoFrame.cpp \
		../livemix/CameraThread.cpp \
		../livemix/SimpleV4L2.cpp

RESOURCES     = textures.qrc
QT           += opengl multimedia

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS textures.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)


unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2 
}

win32 {
	INCLUDEPATH += \
		../external/ffmpeg/include/msinttypes \
		../external/ffmpeg/include/libswscale \
		../external/ffmpeg/include/libavutil \
		../external/ffmpeg/include/libavdevice \
		../external/ffmpeg/include/libavformat \
		../external/ffmpeg/include/libavcodec \
		../external/ffmpeg/include
	
	LIBS += -L"../external/ffmpeg/lib" \
		-lavcodec-51 \
		-lavformat-52 \
		-lavutil-49 \
		-lavdevice-52 \
		-lswscale-0
}

