#ifndef GENERICITEMCONFIG_H
#define GENERICITEMCONFIG_H

#include <QGraphicsProxyWidget>
#include <QBasicTimer>
#include "MyGraphicsScene.h"
class AbstractContent;
class Frame;
class QAbstractButton;
class QListWidgetItem;
//class StyledButtonItem;
class QPushButton;
class MyGraphicsScene;

class AbstractVisualItem;

#include <QtGui/QDialog>

namespace Ui {
    class GenericItemConfig;
}

class GenericItemConfig : public QDialog 
{
	Q_OBJECT
	public:
		GenericItemConfig(AbstractContent * content, QWidget *parent = 0);
		virtual ~GenericItemConfig();
		
		void dispose();
	
		// the related content
		AbstractContent * content() const;
	
		// manage property box
		void keepInBoundaries(const QRect & rect);
		
		MyGraphicsScene * scene() { return m_scene; }
		void setScene(MyGraphicsScene * s) { m_scene = s; }
	
	Q_SIGNALS:
		void applyLook(quint32 frameClass, bool mirrored, bool allContents);
    
	protected:
		void changeEvent(QEvent *e);
		
	protected:
		// used by subclasses
		void addTab(QWidget * widget, const QString & label, bool front = false, bool setCurrent = false);
		//void showOkButton(bool show);
        
	protected Q_SLOTS:
		virtual void slotOkClicked() {};
		//void slotRequestClose();
		void slotOpacityChanged(int);
		
	
	private:
		Ui::GenericItemConfig *m_commonUi;
	
	private:
		void populateFrameList();
		void layoutButtons();
		AbstractContent *       m_content;
		//Ui::AbstractConfig *    m_commonUi;
		QPushButton *           m_closeButton;
		QPushButton *           m_okButton;
		Frame *                 m_frame;
		MyGraphicsScene *	m_scene;
		
		double 			m_origOpacity;
		QPointF			m_origPos;
		QSize			m_origSize;
	
	private Q_SLOTS:
// 		void on_newFrame_clicked();
// 		void on_applyLooks_clicked();
// 		void on_listWidget_itemSelectionChanged();
		void slotMirrorOn(bool checked);
		void slotLocationChanged(double);
		void slotSizeChanged(double);
		void slotResetOpacity();
		void slotResetLocation();
		void slotResetSize();
		void slotClosed();
};

#endif // GENERICITEMCONFIG_H
