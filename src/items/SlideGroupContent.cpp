#include "SlideGroupContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/VideoFileItem.h"
#include "items/CornerItem.h"
// #include "CPixmap.h"
#include "RenderOpts.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QUrl>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

#define DEBUG_VIDEOFILECONTENT 0

SlideGroupContent::SlideGroupContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
{
/*	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
        setToolTip(tr("Video - right click for options."));
	
// 	setText("Welcome");
        //for(int i=0;i<m_cornerItems.size();i++)
        //	m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

//         m_video = new QVideo(this);
//         //connect(m_video, SIGNAL(movieStateChanged(QMovie::MovieState)),
//         //           this, SLOT(movieStateChanged(QMovie::MovieState)));
//         connect(m_video, SIGNAL(currentFrame(QFFMpegVideoFrame)),
//                      this, SLOT(setVideoFrame(QFFMpegVideoFrame)));

	// add play/pause button
	m_bSwap = new ButtonItem(ButtonItem::Control, Qt::blue, QIcon(":/data/action-pause.png"), this);
	m_bSwap->setToolTip(tr("Pause Video"));
	connect(m_bSwap, SIGNAL(clicked()), this, SLOT(slotTogglePlay()));
	addButtonItem(m_bSwap);
	
	m_dontSyncToModel = false;*/
}

