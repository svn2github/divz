#ifndef LiveVideoFileLayer_H
#define LiveVideoFileLayer_H

#include "LiveVideoLayer.h"

class GLVideoDrawable ;
class CameraThread;
class GLDrawable;
class GLWidget;
class VideoThread;
class QtVideoSource;
class QMediaPlaylist;
class QMediaPlayer;

class LiveVideoFileLayer : public LiveVideoLayer
{
	Q_OBJECT
	
	//Q_PROPERTY(QString file READ file WRITE setFile);
	Q_PROPERTY(QStringList fileList READ fileList WRITE setFileList);
	
public:
	Q_INVOKABLE LiveVideoFileLayer(QObject *parent=0);
	~LiveVideoFileLayer();
	
	virtual QString typeName() { return "Video File"; }
	
	QStringList fileList() { return layerProperty("fileList").toStringList(); }
	
	
	QMediaPlaylist * playlist();
	QMediaPlayer * player();

public slots:
	// Set a property (emits instancePropertyChanged)
	void addFile(const QString&);
	void setFileList(const QStringList&);

	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);

private slots:
	void btnDelItem();
	void btnMoveItemUp();
	void btnMoveItemDown();

protected:
	virtual GLDrawable *createDrawable(GLWidget *widget);
	// If its the first drawable, setup with defaults
	// Otherwise, copy from 'copyFrom'
	virtual void initDrawable(GLDrawable *drawable, bool isFirstDrawable = false);
	virtual QWidget *createLayerPropertyEditors();
	
	void setVideo(QtVideoSource*);
	QtVideoSource  *video() { return m_video; }
	
private:
	QtVideoSource *m_video;
	
	QPointer<QListWidget> m_listWidget;
	void setupListWidget();
};

#endif
