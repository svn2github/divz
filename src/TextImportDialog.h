#ifndef TEXTIMPORTDIALOG_H
#define TEXTIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class TextImportDialog;
}

class SlideGroup;
class Slide;
class TextBoxItem;
class QProgressDialog;

#include <QScriptEngine>

class TextImportDialog : public QDialog 
{
	Q_OBJECT
public:
	TextImportDialog(QWidget *parent = 0);
	~TextImportDialog();


protected slots:
	void templateChanged(SlideGroup*);
	
	void doImport();
	void browseBtn();
	void scriptBrowseBtn();
	void preprocBrowseBtn();
	
protected:
	void changeEvent(QEvent *e);
	
	void setupUi();
	
	SlideGroup *getTemplate();
	void setupTextBox(TextBoxItem *tmpText);
	Slide *getTemplateSlide(SlideGroup *templateGroup, int slideNum, bool autoAddTextField=true);
	Slide *getTemplateSlide(SlideGroup *templateGroup, QString name, int slideNum=0, bool autoAddTextField=true);
	
	SlideGroup *generateSlideGroup(SlideGroup *templateGroup, QString text, bool isPlain, QScriptEngine &scriptEngine, QString scriptFilename, SlideGroup *append=0, SlideGroup *primary=0);

private:
	Ui::TextImportDialog *m_ui;
	
	SlideGroup *m_template;
	
	QProgressDialog *m_progress;
};

#endif // TEXTIMPORTDIALOG_H
