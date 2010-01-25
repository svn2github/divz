#ifndef MYGRAPHICSSCENE_H
#define MYGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QDataStream>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QTime>
#include <QVariant>


class AbstractContent;
class AbstractVisualItem;
class GenericItemConfig;

class Slide;
class AbstractItem;
class TextItem;
class BackgroundItem;

class QVideoProvider;

class MyGraphicsView;

class MyGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
	public:
	//         friend class XmlRead;
	//         friend class XmlSave;
	
		typedef enum ContextHint
                {
                	Editor,
                	Preview,
                	BackgroundPreview,
                	Monitor,
                	Live,
                	StaticPreview,
                };
	
		MyGraphicsScene(ContextHint hint = Editor, QObject * parent = 0);
 		~MyGraphicsScene();
 		
 		typedef enum SlideTransition { None, CrossFade };
 		
 		void setSlide(Slide *, SlideTransition t = None, int speed = -1, int quality = -1);
 		Slide * slide() { return m_slide; }
 		
 		// changing the master slide has no effect while the slide is displayed - master slide
 		// is merged with items comming in from the new slide during the setSlide() call
 		void setMasterSlide(Slide *);
 		Slide * masterSlide() { return m_masterSlide; }
		
		bool isDataLoadComplete();
		
		AbstractVisualItem * newTextItem(QString text = "Lorem Ipsum");
		AbstractVisualItem * newBoxItem();
		AbstractVisualItem * newVideoItem();
		AbstractVisualItem * newImageItem();
		AbstractVisualItem * newOutputView();
		
		void clear();
		
		ContextHint contextHint() { return m_contextHint; }
		void setContextHint(ContextHint);
		
		void setSceneRect(const QRectF &);
		
		AbstractContent * createVisualDelegate(AbstractItem *item, QGraphicsItem * parent =0);
		void removeVisualDelegate(AbstractItem *item);
		AbstractContent * findVisualDelegate(AbstractItem *item);
		QList<AbstractItem *> copyBuffer();
		
		QList<AbstractContent *> abstractContent(bool onlyMasterItems = false);
		
		void configureContent(AbstractContent *content);
		
		QList<QWidget*> controlWidgets();
	
	signals:
		void showPropertiesWidget(QWidget * widget);
		void sceneRectChanged(const QRectF &);
		
		void crossFadeStarted(Slide *oldSlide, Slide *newSlide);
		void crossFadeFinished(Slide *oldSlide, Slide *newSlide);
		
		void slideDiscarded(Slide*);
		
		void itemDoubleClicked(AbstractContent *item);
	
	public slots:
		void copyCurrentSelection(bool removeSelection = false);
		void pasteCopyBuffer();
		void selectAll();
		
	protected slots:
		void slotItemDoubleClicked(AbstractContent*);
	
	protected:
		friend class MyGraphicsView;
		void keyPressEvent(QKeyEvent * event);
		bool eventFilter(QObject *obj, QEvent *event);

		
	private:
// 		TextContent * createText(const QPoint & pos);
		void addContent(AbstractContent * content, bool takeOnwership = false); //, const QPoint & pos);
		int maxZValue();
		
		void applyMasterSlideItemFlags(AbstractContent *content);
		
		QList<AbstractContent *> m_content;
		QList<AbstractContent *> m_ownedContent;
		QList<AbstractContent *> m_prevContent;
		QList<GenericItemConfig *> m_configs;
		QList<AbstractItem *> m_copyBuffer;
		
		Slide * m_slide;
		Slide * m_slidePrev;
		SlideTransition m_currentTransition;
		
		Slide * m_masterSlide;
		
		int m_fadeSteps;
		int m_fadeStepCounter;
		QList<double> m_fadeIncx;
		QTimer * m_fadeTimer;
		
		ContextHint m_contextHint;
		
		QGraphicsItem * m_fadeRoot;
		QGraphicsItem * m_liveRoot;
		QGraphicsItem * m_staticRoot;
		
		BackgroundItem * m_bg;
			
	private slots:
		friend class AbstractConfig; // HACK here, only to call 1 method
		friend class GenericItemConfig; // HACK here, only to call 1 method
//         	friend class PixmapButton; // HACK here, only to call 1 method
        
        	void startTransition();
		void slotTransitionStep();
		void endTransition();
		
		void slotSelectionChanged();
        	void slotConfigureContent(const QPoint & scenePoint);
		void slotStackContent(int);
		void slotDeleteContent();
		void slotDeleteConfig(GenericItemConfig * config);
		void slotApplyLook(quint32 frameClass, bool mirrored, bool allContent);
		
		void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant old);
};

#endif
