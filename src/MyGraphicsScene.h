#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include "items/TextContent.h"

#include <QGraphicsScene>
#include <QDataStream>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QTime>



class AbstractContent;
class GenericItemConfig;
class Slide;
class TextItem;

class MyGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	public:
	//         friend class XmlRead;
	//         friend class XmlSave;
	
		MyGraphicsScene(QObject * parent = 0);
 		~MyGraphicsScene();
 		
 		typedef enum SlideTransition { None, CrossFade };
 		
 		void setSlide(Slide *, SlideTransition t = CrossFade);
		
// 		TextContent * addTextContent();
		
		TextItem * newTextItem(QString text = "Lorem Ipsum");
                AbstractVisualItem * newBoxItem();
                AbstractVisualItem * newVideoItem();
		
		
		
	signals:
		void showPropertiesWidget(QWidget * widget);
		
	private:
// 		TextContent * createText(const QPoint & pos);
		void addContent(AbstractContent * content); //, const QPoint & pos);
	
		QList<AbstractContent *> m_content;
		QList<GenericItemConfig *> m_configs;
		
		Slide * m_slide;
		Slide * m_slidePrev;
		SlideTransition * m_currentTransition;
			
	private slots:
		friend class AbstractConfig; // HACK here, only to call 1 method
		friend class GenericItemConfig; // HACK here, only to call 1 method
//         	friend class PixmapButton; // HACK here, only to call 1 method
        
		void slotTransitionStep();
		
		void slotSelectionChanged();
        	void slotConfigureContent(const QPoint & scenePoint);
		void slotStackContent(int);
		void slotDeleteContent();
		void slotDeleteConfig(GenericItemConfig * config);
		void slotApplyLook(quint32 frameClass, bool mirrored, bool allContent);
};

#endif
