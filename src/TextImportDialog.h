#ifndef TEXTIMPORTDIALOG_H
#define TEXTIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class TextImportDialog;
}

class SlideGroup;
class Slide;
class TextBoxItem;

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
	
protected:
	void changeEvent(QEvent *e);
	
	void setupUi();
	
	SlideGroup *getTemplate();
	void setupTextBox(TextBoxItem *tmpText);
	Slide *getTemplateSlide(SlideGroup *templateGroup, int /*slideNum*/);
	
	SlideGroup *generateSlideGroup(SlideGroup *templateGroup, QString text, bool isPlain, QScriptEngine &scriptEngine, QString scriptFilename);

private:
	Ui::TextImportDialog *m_ui;
	
	SlideGroup *m_template;
};

#endif // TEXTIMPORTDIALOG_H
