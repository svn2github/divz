#ifndef OUTPUTCONTROL_H
#define OUTPUTCONTROL_H

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>

#include <qtcolorpicker.h>

class OutputInstance;
class SlideGroupViewControl;
class Document;
class Slide;
class SlideGroup;
class AbstractSlideGroupEditor;

#include "model/AbstractItemFilter.h"

class OutputControl :  public QWidget
{
	Q_OBJECT
	
public:
	OutputControl(QWidget*parent=0);
	
	OutputInstance		* outputInstance() { return m_inst; }
	SlideGroupViewControl	* viewControl()    { return m_ctrl; }
	
	bool outputIsSynced() { return m_outputIsSynced; }
	
	bool isClearToggled() { return m_clearButton->isChecked(); }
	bool isBlackToggled() { return m_blackButton->isChecked(); }
	bool isLogoToggled() { return m_logoButton->isChecked(); }
	
public slots:
	void setOutputInstance(OutputInstance*);
	void setViewControl(SlideGroupViewControl*);
	void setCustomFilters(AbstractItemFilterList);
	
	void setCrossFadeSpeed(int value);
	
	void setIsOutputSynced(bool);
	void setSyncSource(OutputInstance*);
	void setTextOnlyBackground(const QColor &);
	
	void setOverlayDocument(Document*);
	void setOverlayEnabled(bool);
	void setTextOnlyFilterEnabled(bool);
	void setTextResizeEnabled(bool);
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);
	void fadeLogoFrame(bool);
	
	

/*public*/ 
signals:
	void outputIsSyncedChanged(bool);
	
protected slots:
	void syncOutputChanged(int idx);
	void overlayDocumentChanged(const QString&);
	void overlayBrowseClicked();
	void setIsOutputSynced(int);
	//void overlayGroupChanged(SlideGroup*);
	
	// inorder to reload the list of outputs for the sync widget box
	void setupSyncWithBox();
	
	// inorder to untoggle black/clear buttons
	void slideChanged(int);
	
	void filterListItemChanged(QListWidgetItem * item);
	
	void setAdvancedWidgetVisible(bool);
	
	void newLogo();
	void editLogo();
	void logoActionTriggered(QAction*);
	void logoEditorClosed();

protected:
	void setupUI();
	void setupSyncWidgetUI();
	void setupAdvancedUI();
	void setupFilterList(AbstractItemFilterList selected = AbstractItemFilterList(), AbstractItemFilterList defaultEnabled = AbstractItemFilterList());
	
	void setupFoldbackSettings();
	
	QLabel			* m_overlayPreviewLabel;
	QStackedWidget		* m_stack;
	OutputInstance		* m_inst;
	SlideGroupViewControl	* m_ctrl;
	OutputInstance		* m_syncInst;
	
	QSlider			* m_fadeSlider;
	
	QPushButton		* m_advancedButton;
	QWidget			* m_advancedWidget;
	
	QPushButton		* m_blackButton;
	QPushButton		* m_clearButton;
	
	QLineEdit		* m_overlayDocBox;
	Document		* m_overlayDoc;
	Slide			* m_overlaySlide;
	
	QPushButton		* m_overlayEnabledBtn;
	
	QWidget			* m_syncWidget;
	QComboBox		* m_syncWithBox;
	
	QtColorPicker		* m_colorPicker;
	
	bool			m_outputIsSynced;
	
	QComboBox		* m_syncStatusSelector;
	QPushButton 		* m_textResizeBtn;
	
	
	AbstractItemFilterList	m_customFilterList;
	QListWidget		* m_filterList;
	
	bool			m_timerWasActiveBeforeFade;
	
	QPushButton *m_logoButton;
	QPushButton *m_configLogo;
	SlideGroup *m_selectedLogo;
	
	QMenu * m_logoMenu;
	
	void setupLogoMenu();
	
	AbstractSlideGroupEditor *m_editWin;
};


#endif
