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
class AbstractConfig;

class MyGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	public:
	//         friend class XmlRead;
	//         friend class XmlSave;
	
		MyGraphicsScene(QObject * parent = 0);
 		~MyGraphicsScene();
		
		TextContent * addTextContent();
		
		void initContent(AbstractContent * content, const QPoint & pos);
		
		
	signals:
		void showPropertiesWidget(QWidget * widget);
		
	private:
		TextContent * createText(const QPoint & pos);
	
		QList<AbstractContent *> m_content;
		QList<AbstractConfig *> m_configs;
		
	private slots:
		friend class AbstractConfig; // HACK here, only to call 1 method
//         	friend class PixmapButton; // HACK here, only to call 1 method
        
		void slotSelectionChanged();
        	void slotConfigureContent(const QPoint & scenePoint);
		void slotStackContent(int);
		void slotDeleteContent();
		void slotDeleteConfig(AbstractConfig * config);
		void slotApplyLook(quint32 frameClass, bool mirrored, bool allContent);
};

#endif
