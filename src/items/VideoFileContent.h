#ifndef VIDEOFILECONTENT_H
#define VIDEOFILECONTENT_H

#include "AbstractContent.h"
#include "qvideo/QVideoBuffer.h"
#include "qvideo/QVideo.h"

/// \brief TODO
class VideoFileContent : public AbstractContent
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename)
    
    public:
        VideoFileContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
        ~VideoFileContent();

    public Q_SLOTS:
        QString filename() const { return m_filename; }
         void setFilename(const QString & text);

    private slots:
         void setVideoFrame(QFFMpegVideoFrame);

    public:
        // ::AbstractContent
        QString contentName() const { return tr("VideoFile"); }
        QWidget * createPropertyWidget();
        bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
        QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
        int contentHeightForWidth(int width) const;
//         void selectionChanged(bool selected);
        
        void syncFromModelItem(AbstractVisualItem*);
        AbstractVisualItem * syncToModelItem(AbstractVisualItem*);

        // ::QGraphicsItem
//         void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
        void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    private:
//         void updateTextConstraints();
//         void updateCache();

        // text document, layouting & rendering
         QString m_filename;
//         QList<QRect> m_blockRects;
//         QRect m_textRect;
//         int m_textMargin;
         QImage m_image;
         QSize m_imageSize;
         QVideo * m_video;

};

#endif
