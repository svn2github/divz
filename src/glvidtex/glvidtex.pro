
MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build
UI_DIR = .build

HEADERS       = GLWidget.h \
		../livemix/VideoSource.h \
		../livemix/VideoThread.h \
		../livemix/VideoFrame.h \
		../livemix/CameraThread.h \
		GLDrawable.h \
		GLVideoDrawable.h \
		../ImageFilters.h \
		RichTextRenderer.h \
		VideoSender.h \
		VideoReceiver.h \
		GLImageDrawable.h \
		GLVideoLoopDrawable.h \
		GLVideoInputDrawable.h \
		GLVideoFileDrawable.h \
		GLVideoReceiverDrawable.h \
		GLTextDrawable.h \
		GLSceneGroup.h \
		../livemix/MjpegThread.h \
		GLVideoMjpegDrawable.h \
		CornerItem.h \
		GLEditorGraphicsScene.h \
		GLPlayerServer.h \
		GLPlayerClient.h \
		
SOURCES       = GLWidget.cpp \
		../livemix/VideoSource.cpp \
		../livemix/VideoThread.cpp \
		../livemix/VideoFrame.cpp \
		../livemix/CameraThread.cpp \
		GLDrawable.cpp \
		GLVideoDrawable.cpp \
		../ImageFilters.cpp \
		RichTextRenderer.cpp \
		VideoSender.cpp \
		VideoReceiver.cpp \
		GLImageDrawable.cpp \
		GLVideoLoopDrawable.cpp \
		GLVideoInputDrawable.cpp \
		GLVideoFileDrawable.cpp \
		GLVideoReceiverDrawable.cpp \
		GLTextDrawable.cpp \
		GLSceneGroup.cpp \
		MetaObjectUtil.cpp \
		../livemix/MjpegThread.cpp \
		GLVideoMjpegDrawable.cpp \
		CornerItem.cpp \
		GLEditorGraphicsScene.cpp   \
		GLPlayerServer.cpp \
		GLPlayerClient.cpp 
		

editor: {
	TARGET = gleditor
	
	HEADERS += ../livemix/EditorUtilityWidgets.h \
		../livemix/ExpandableWidget.h \
		EditorWindow.h \
		EditorGraphicsView.h \
		RtfEditorWindow.h
		
	SOURCES += editor-main.cpp \
		../livemix/EditorUtilityWidgets.cpp \
		../livemix/ExpandableWidget.cpp \
		EditorWindow.cpp \
		EditorGraphicsView.cpp \
		RtfEditorWindow.cpp
		
	include(../3rdparty/richtextedit/richtextedit.pri)
}
director: {
	TARGET = gldirector
	
	FORMS += DirectorWindow.ui \
		 PlayerSetupDialog.ui


	
	HEADERS += ../livemix/EditorUtilityWidgets.h \
		../livemix/ExpandableWidget.h \
		EditorWindow.h \
		EditorGraphicsView.h \
		DirectorWindow.h \
		PlayerConnection.h \
		RtfEditorWindow.h \
		PlayerSetupDialog.h \
		KeystonePointsEditor.h \
		FlowLayout.h \
		../livemix/VideoWidget.h \
		VideoInputSenderManager.h
		
		
	SOURCES += director-main.cpp \
		../livemix/EditorUtilityWidgets.cpp \
		../livemix/ExpandableWidget.cpp \
		EditorWindow.cpp \
		EditorGraphicsView.cpp \
		DirectorWindow.cpp \
		PlayerConnection.cpp \
		RtfEditorWindow.cpp \
		PlayerSetupDialog.cpp \
		KeystonePointsEditor.cpp \
		FlowLayout.cpp \
		../livemix/VideoWidget.cpp \
		VideoInputSenderManager.cpp
		
	include(../3rdparty/richtextedit/richtextedit.pri)
}
player: {
	TARGET = glplayer
	
	HEADERS += PlayerWindow.h \
		VideoInputSenderManager.h
		
	SOURCES += player-main.cpp \
		PlayerWindow.cpp \
		VideoInputSenderManager.cpp
}
glvidtex: {
	TARGET = glvidtex
	
	HEADERS += PlayerWindow.h \
		VideoInputSenderManager.h \
		../livemix/VideoWidget.h 
		
		
	SOURCES += main.cpp \
		PlayerWindow.cpp \
		VideoInputSenderManager.cpp \
		../livemix/VideoWidget.cpp
}

unix: {
	HEADERS += \
		../livemix/SimpleV4L2.h
	SOURCES += \
		../livemix/SimpleV4L2.cpp

	# CentOS fix - see http://theitdepartment.wordpress.com/2009/03/15/centos-qt-fcfreetypequeryface/
	LIBS += -L/opt/fontconfig-2.4.2/lib
}

opencv: {
	DEFINES += OPENCV_ENABLED
	LIBS += -L/usr/local/lib -lcv -lcxcore
}


RESOURCES     = glvidtex.qrc
QT           += opengl multimedia network


win32 {
    QT_MOBILITY_HOME = C:/Qt/qt-mobility-opensource-src-1.0.2
}
unix {
    #QT_MOBILITY_HOME = /opt/qt-mobility-opensource-src-1.0.1
     QT_MOBILITY_HOME = /opt/qt-mobility-opensource-src-1.1.0-beta2
}
	
# To enable, use: qmake CONFIG+=mobility, and make sure QT_MOBILITY_HOME is correct
# To run: Make sure QT_PLUGIN_PATH has $QT_MOBILITY_HOME/plugins added, else media will not play
# 	  ..and make sure $LD_LIBRARY_PATH has $QT_MOBILITY_HOME/lib - otherwise app will not start.
mobility: {
	isEmpty(QT_MOBILITY_SOURCE_TREE):QT_MOBILITY_SOURCE_TREE = $$QT_MOBILITY_HOME 
	isEmpty(QT_MOBILITY_BUILD_TREE):QT_MOBILITY_BUILD_TREE = $$QT_MOBILITY_HOME 
	
	#now include the dynamic config
	include($$QT_MOBILITY_BUILD_TREE/config.pri)
	
	CONFIG += mobility multimedia
	MOBILITY = multimedia

	INCLUDEPATH += \
		$$QT_MOBILITY_HOME/src \
		$$QT_MOBILITY_HOME/src/global \
		$$QT_MOBILITY_HOME/src/multimedia \
		$$QT_MOBILITY_HOME/src/multimedia/audio \
		$$QT_MOBILITY_HOME/src/multimedia/video
		
	LIBS += -L$$QT_MOBILITY_BUILD_TREE/lib \
		-lQtMultimediaKit
	
	DEFINES += \
		QT_MOBILITY_ENABLED \
		HAS_QT_VIDEO_SOURCE
	
	HEADERS += \
		QtVideoSource.h
	SOURCES += \
		QtVideoSource.cpp
		
	message("QtMobility enabled. Before running, ensure \$QT_PLUGIN_PATH contains $$QT_MOBILITY_HOME/plugins, otherwise media will not play.")
}
else: {
	message("QtMobility not enabled (use qmake CONFIG+=mobility and ensure $$QT_MOBILITY_HOME exists), QtVideoSource will not be built.")
    	DEFINES -= QT_MOBILITY_ENABLED
}

RESOURCES += ../dviz.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS textures.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/opengl/textures
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)


unix {
	LIBS += -lavdevice -lavformat -lavcodec -lavutil -lswscale -lbz2 
	INCLUDEPATH += /usr/include/ffmpeg
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

