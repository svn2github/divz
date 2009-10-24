#ifndef OUTPUTCONTROL_H
#define OUTPUTCONTROL_H

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

#include <qtcolorpicker.h>

class OutputInstance;
class SlideGroupViewControl;
class Document;
class Slide;

class OutputControl :  public QWidget
{
	Q_OBJECT
	
public:
	OutputControl(QWidget*parent=0);
	
	OutputInstance		* outputInstance() { return m_inst; }
	SlideGroupViewControl	* viewControl()    { return m_ctrl; }
	
	bool outputIsSynced() { return m_outputIsSynced; }
	
public slots:
	void setOutputInstance(OutputInstance*);
	void setViewControl(SlideGroupViewControl*);
	
	void setCrossFadeSpeed(int value);
	
	void setIsOutputSynced(bool);
	void setSyncSource(OutputInstance*);
	//void setSyncBackground(const QColor &);
	
	void setOverlayDocument(Document*);
	void setOverlayEnabled(bool);
	void setSyncTextOnlyFilterEnabled(bool);
	void setSyncTextResizeEnabled(bool);
	
	
protected slots:
	void syncOutputChanged(int idx);
	void overlayDocumentChanged(const QString&);
	void overlayBrowseClicked();
	void setIsOutputSynced(int);
	//void overlayGroupChanged(SlideGroup*);
	
	// inorder to reload the list of outputs for the sync widget box
	void setupSyncWithBox();

protected:
	void setupUI();
	void setupSyncWidgetUI();
	
	QLabel			* m_overlayPreviewLabel;
	QStackedWidget		* m_stack;
	OutputInstance		* m_inst;
	SlideGroupViewControl	* m_ctrl;
	OutputInstance		* m_syncInst;
	
	QSlider			* m_fadeSlider;
	
	
	QPushButton		* m_blackButton;
	QPushButton		* m_clearButton;
	
	QLineEdit		* m_overlayDocBox;
	Document		* m_overlayDoc;
	Slide			* m_overlaySlide;
	
	QPushButton		* m_overlayEnabledBtn;
	
	QWidget			* m_syncWidget;
	QComboBox		* m_syncWithBox;
	
	//QtColorPicker		* m_colorPicker;
	
	bool			m_outputIsSynced;
};


#endif
