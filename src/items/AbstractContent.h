#ifndef __AbstractContent_h__
#define __AbstractContent_h__

#include "AbstractDisposeable.h"
#include <QDomElement>
#include "3rdparty/enricomath.h"
#include "model/AbstractVisualItem.h"

class AbstractConfig;
class ButtonItem;
class CornerItem;
class Frame;
class MirrorItem;
class QGraphicsTextItem;
class QPointF;

#include "MyGraphicsScene.h"


/// \brief Base class of Canvas Item (with lots of gadgets!)
class AbstractContent : public AbstractDisposeable
{
		Q_OBJECT
			
	public:
		AbstractContent(QGraphicsScene * scene, QGraphicsItem * parent = 0, bool noRescale = false);
		virtual ~AbstractContent();
	
		// ::AbstractDisposeable
		void dispose(bool anim=true);
		
		virtual void syncFromModelItem(AbstractVisualItem*);
		virtual AbstractVisualItem * syncToModelItem(AbstractVisualItem *model); //defaults to m_modelItem
		
		// size
		QRect contentsRect() const;
		void resizeContents(const QRect & rect, bool keepRatio = false);
		void resetContentsRatio();
		void delayedDirty(int ms = 400);
	
		// frame (and frame text)
		void setFrame(Frame * frame);
		quint32 frameClass() const;
		void setFrameTextEnabled(bool enabled);
		bool frameTextEnabled() const;
		void setFrameText(const QString & text);
		QString frameText() const;
		void addButtonItem(ButtonItem * buttonItem);
	
		// rotation
		void setRotation(double angle, Qt::Axis axis);
		double rotation(Qt::Axis axis) const;
	
		// mirror
		void setMirrorEnabled(bool enabled);
		bool mirrorEnabled() const;
	
		// misc
		void ensureVisible(const QRectF & viewportRect);
		bool beingTransformed() const;
	
		// to be reimplemented by subclasses
		virtual QString contentName() const = 0;
		virtual QWidget * createPropertyWidget();
	//         virtual bool fromXml(QDomElement & parentElement);
	//         virtual void toXml(QDomElement & parentElement) const;
		virtual QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const = 0;
	
		// ::QGraphicsItem
		QRectF boundingRect() const;
		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	
		// may be reimplemented by subclasses
		virtual int contentHeightForWidth(int width) const;
		virtual bool contentOpaque() const;
		
		AbstractVisualItem *modelItem() { return m_modelItem; }
		
		// to be reimpl
		
		void setOpacity(double);
		
		virtual void applySceneContextHint(MyGraphicsScene::ContextHint);
		MyGraphicsScene::ContextHint sceneContextHint() { return m_contextHint; }
		

	Q_SIGNALS:
		void configureMe(const QPoint & scenePoint);
		void changeStack(int opcode);
		void backgroundMe();
		void deleteItem();
		void contentChanged();
		void resized();

	protected:
		// may be reimplemented by subclasses
		virtual void selectionChanged(bool selected);
	
		// called by subclasses too
		void GFX_CHANGED() const;
		void setControlsVisible(bool visible);
		QPixmap ratioScaledPixmap(const QPixmap * source, const QSize & size, Qt::AspectRatioMode ratio) const;
	
		// ::QGraphicsItem
		void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
		void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
		void dropEvent(QGraphicsSceneDragDropEvent * event);
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		void keyPressEvent(QKeyEvent * event);
		QVariant itemChange(GraphicsItemChange change, const QVariant & value);
		
		void setModelItemIsChanging(bool flag = true);
		void setModelItem(AbstractVisualItem *model);
		
	
		
		
		bool m_dontSyncToModel;
		
	protected Q_SLOTS:
		void slotConfigure();
		void slotStackFront();
		void slotStackRaise();
		void slotStackLower();
		void slotStackBack();
		void slotSaveAs();
		void modelItemChanged(QString fieldName, QVariant value);
	
	protected:
		void createCorner(Qt::Corner corner, bool noRescale);
		void layoutChildren();
		void applyRotations();
		QRect               m_contentsRect;
		QRectF              m_frameRect;
		Frame *             m_frame;
		QGraphicsTextItem * m_frameTextItem;
		QList<ButtonItem *> m_controlItems;
		QList<CornerItem *> m_cornerItems;
		bool                m_controlsVisible;
		bool                m_dirtyTransforming;
		QTimer *            m_transformRefreshTimer;
		QTimer *            m_gfxChangeTimer;
		MirrorItem *        m_mirrorItem;
		double              m_xRotationAngle;
		double              m_yRotationAngle;
		double              m_zRotationAngle;
		AbstractVisualItem *m_modelItem;
		bool		    m_modelItemIsChanging;
		bool		    m_hovering;
		
		MyGraphicsScene::ContextHint	m_contextHint;
		
	
	private Q_SLOTS:
		void slotPerspective(const QPointF & sceneRelPoint, Qt::KeyboardModifiers modifiers);
		void slotClearPerspective();
		void slotDirtyEnded();
	
		// used by desk arrangement functions
	public:
		Vector2 vForce, vVel, vPos;
};

#endif
