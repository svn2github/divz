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
class SlideSettingsDialog;
class QTabWidget;
class QtColorPicker;
#include <QFileInfo>

class GenericItemConfig : public QWidget 
{
	Q_OBJECT
	public:
		GenericItemConfig(AbstractContent * content, QWidget *parent = 0);
		virtual ~GenericItemConfig();
		
		void dispose();
		
		QDialog * toDialog(QWidget *parent=0);
	
		// the related content
		AbstractContent * content() const;
	
		// manage property box
		void keepInBoundaries(const QRect & rect);
		
		MyGraphicsScene * scene() { return m_scene; }
		void setScene(MyGraphicsScene * s) { m_scene = s; }
		
		QTabWidget * tabWidget();
	
	Q_SIGNALS:
		void applyLook(quint32 frameClass, bool mirrored, bool allContents);
    
	protected:
		void changeEvent(QEvent *e);
		// used by subclasses
		void addTab(QWidget * widget, const QString & label, bool front = false, bool setCurrent = false);
		//void showOkButton(bool show);
        
	protected Q_SLOTS:
		virtual void slotOkClicked() {};
		//void slotRequestClose();
		void slotOpacityChanged(int);
		
		void slotVideoDblClicked(const QFileInfo&);
		void slotImageDblClicked(const QFileInfo&);
		
		void itemChanged(QString fieldName, QVariant value, QVariant oldValue);
	
	protected:
		friend class SlideSettingsDialog;
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
		QSizeF			m_origSize;
		
		QtColorPicker 		* m_shadowColorPicker;
		
		bool m_updateShadowDistanceAndDirection_changing;
		
		void updateShadowDistanceAndDirection();
	
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
		void setLineColor(const QColor &);
		void setBgColor(const QColor &);
		void setShadowColor(const QColor &);
		void setShadowBlur(int);
		void slotImageBrowse();
		void slotVideoBrowse();
		
		void slotLineSizeChanged(double);
		void slotShadowXOffsetChanged(double);
		void slotShadowYOffsetChanged(double);
		void slotMirrorOffsetChanged(double);
		
		void slotShadowAlphaChanged(int);
		void slotShadowDistanceChanged(int);
		void setShadowAlphaBox();
		
		void slotImageFileChanged(const QString&);
		void slotVideoFileChanged(const QString&);
		
		void slotOutlineEnabled(bool);
		void slotShadowEnabled(bool);
		
		void slotBgNone(bool);
		void slotBgColor(bool);
		void slotBgGradient(bool);
		void slotBgImage(bool);
		void slotBgVideo(bool);
		
		void shadowOffsetPreset0();
		void shadowOffsetPresetB();
		void shadowOffsetPresetBL();
		void shadowOffsetPresetBR();
		void shadowOffsetPresetL();
		void shadowOffsetPresetR();
		void shadowOffsetPresetT();
		void shadowOffsetPresetTL();
		void shadowOffsetPresetTR();
		
		void setShadowOffsets(double x, double y);
		
		void zoomEffectEnabled(bool);
		void setZoomAnchorCenter(bool);
		void setZoomAnchorRandom(bool);
		void setZoomAnchorPoint(int);
		void updateZoomDirection();
		void zoomSpeedChanged(int);
		void zoomLoopChanged(bool);
		void zoomFactorChanged(double);
		
		void shadowDistanceBoxChanged(int);
		void shadowDirectionBoxChanged(int);
		
		void videoEndActionChanged(int);
	
		
};

#endif // GENERICITEMCONFIG_H
