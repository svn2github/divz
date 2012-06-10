#include "TextImportDialog.h"
#include "ui_TextImportDialog.h"

#include "model/SlideTemplateManager.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/TextBoxItem.h"
#include "model/ItemFactory.h"
#include "model/Output.h"
#include "model/Document.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "MediaBrowser.h"

#include "bible/BibleBrowser.h"

#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QtGui>
#include <QDir>
#include <QProgressDialog>



using namespace SlideTemplateUtilities;

TextImportDialog::TextImportDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::TextImportDialog),
    m_template(0)
{
	m_ui->setupUi(this);
	setupUi();
}

TextImportDialog::~TextImportDialog()
{
	delete m_ui;
}

void TextImportDialog::setupUi()
{
	// Setup directory completors for the file and script boxen
	QCompleter *completer = new QCompleter(this);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_ui->filename->setCompleter(completer);

	completer = new QCompleter(this);
	dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_ui->scriptFile->setCompleter(completer);
	
	completer = new QCompleter(this);
	dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_ui->preprocFile->setCompleter(completer);
	
	// Remember the last format selected
	bool isPlainText = QSettings().value("textimport/plaintext",true).toBool();
	m_ui->fmtPlain->setChecked(isPlainText);
	m_ui->fmtFields->setChecked(!isPlainText);
	
	// Find the template
	QSettings settings;
	int templateId = settings.value("textimport/template-id",0).toInt();
	if(templateId > 0)
		m_template = SlideTemplateManager::instance()->findTemplate(SlideTemplateManager::Generic,templateId);
	
	// Setup the template selector widget
	TemplateSelectorWidget *tmplWidget = new TemplateSelectorWidget(SlideTemplateManager::Generic,"Template:",this);
	if(m_template)
		tmplWidget->setSelectedGroup(m_template);
	else
		m_template = tmplWidget->selectedGroup();
	
	// Add template editor to UI
	//QVBoxLayout *vbox = new QVBoxLayout(m_ui->templateBox);
	//vbox->addWidget(tmplWidget);
	m_ui->templateBox->layout()->addWidget(tmplWidget);
			
	connect(tmplWidget, SIGNAL(currentGroupChanged(SlideGroup*)), this, SLOT(templateChanged(SlideGroup*)));
	
	// Connect slots
	connect(m_ui->buttonBox,     SIGNAL(accepted()), this, SLOT(doImport()));
	connect(m_ui->browse,        SIGNAL(clicked()),  this, SLOT(browseBtn()));
	connect(m_ui->scriptBrowse,  SIGNAL(clicked()),  this, SLOT(scriptBrowseBtn()));
	connect(m_ui->preprocBrowse, SIGNAL(clicked()),  this, SLOT(preprocBrowseBtn()));
	
	
	m_ui->filename->setText(AppSettings::previousPath("textimport"));
	m_ui->scriptFile->setText(AppSettings::previousPath("script"));
	m_ui->preprocFile->setText(AppSettings::previousPath("preproc"));
}

void TextImportDialog::browseBtn()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Text File"),
		AppSettings::previousPath("textimport"),
		tr("Text Files (*.txt);;Any File (*.*)")
	);
	
	if(!fileName.isEmpty())
	{
		AppSettings::setPreviousPath("textimport",fileName);
		m_ui->filename->setText(fileName);
	}
}

void TextImportDialog::scriptBrowseBtn()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Script File"),
		AppSettings::previousPath("script"),
		tr("JavaScript Files (*.js);;Any File (*.*)")
	);
	
	if(!fileName.isEmpty())
	{
		AppSettings::setPreviousPath("script",fileName);
		m_ui->scriptFile->setText(fileName);
	}
}

void TextImportDialog::preprocBrowseBtn()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select PRe-Processor"),
		AppSettings::previousPath("preproc"),
		tr("Perl Files (*.pl);;Bash Scripts (*.sh);;Windows EXE Files (*.exe);;Any File (*.*)")
	);
	
	if(!fileName.isEmpty())
	{
		AppSettings::setPreviousPath("preproc",fileName);
		m_ui->preprocFile->setText(fileName);
	}
}

void TextImportDialog::templateChanged(SlideGroup* group)
{
	if(m_template && group->property("-auto-template").toBool())
	{
		// we auto-created this template in getTemplate() - delete it since we dont need it now
		delete m_template;
		m_template = 0;
	}
	
	m_template = group;
	QSettings().setValue("textimport/template-id", m_template ? m_template->groupId() : 0);
}


SlideGroup * TextImportDialog::getTemplate()
{
	if(!m_template)
	{
		SlideGroup *group = new SlideGroup();
		Slide *slide = new Slide();
	
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(Qt::blue);
		
		group->addSlide(slide); 
		
		// Allow this template to be deleted if user selects one using the SlideTemplateManager UI
		group->setProperty("-auto-template",true);
		
		// We dont setup the individual text boxes - those will be created as needed in getTemplateSlide(SlideGroup *, int number)
		
		m_template = group;
	}
	
	return m_template;
}

void TextImportDialog::setupTextBox(TextBoxItem *tmpText)
{
	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);

	tmpText->setPos(QPointF(0,0));
	tmpText->setOutlinePen(pen);
	tmpText->setOutlineEnabled(true);
	tmpText->setFillBrush(Qt::white);
	tmpText->setFillType(AbstractVisualItem::Solid);
	tmpText->setShadowEnabled(true);
	tmpText->setShadowBlurRadius(6);
}

Slide *TextImportDialog::getTemplateSlide(SlideGroup *templateGroup, int slideNum, bool autoAddTextField)
{
	QRect fitRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
	
	// This has the effect of allowing the template designer to, for example, design two slides:
	// one for the "first" slide (perhaps the title slide), which will be used when slideNum=0, 
	// then design another slide (in the template, the second slide), which will be used for
	// *every* other slide in the import, since when slideNum (in this example) is >0, say for example, 3
	// it will be restrited to numSlides-1, which would be '1', which would get the second slide
	// in the template. 
	if(slideNum >= templateGroup->numSlides())
		slideNum = templateGroup->numSlides()-1;
		
	Slide *slide = templateGroup->at(slideNum);
	if(!slide)
	{
		slide = new Slide();
	
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

		bg->setFillType(AbstractVisualItem::Solid);
		bg->setFillBrush(Qt::blue);
		
		templateGroup->addSlide(slide);
	}
	
	if(autoAddTextField)
	{
		TextBoxItem *tmpText = findTextItem(slide, "#text");
		
		if(!tmpText)
		{
			tmpText = new TextBoxItem();
			tmpText->setItemId(ItemFactory::nextId());
			tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
			
			tmpText->setText("<center><span style='font-family:Calibri,Tahoma,Arial,Sans-Serif;font-weight:800'><b>#text</b></span></center>");
			tmpText->changeFontSize(72);
			tmpText->setContentsRect(fitRect);
			setupTextBox(tmpText);
			
			slide->addItem(tmpText);
		}
	}
	
	return slide;
}

Slide *TextImportDialog::getTemplateSlide(SlideGroup *templateGroup, QString name, int slideNum, bool autoAddTextField)
{
	QRect fitRect = AppSettings::adjustToTitlesafe(MainWindow::mw()->standardSceneRect());
	
	foreach(Slide *slide, templateGroup->slideList())
		if(slide->slideName() == name)
			return slide;
	
	return getTemplateSlide(templateGroup, slideNum, autoAddTextField);
}

static QString static_currentScriptFile="";
QScriptValue TextImportDialog_script_qDebug(QScriptContext *context, QScriptEngine */*engine*/)
{
	QStringList list;
	for(int i=0; i<context->argumentCount(); i++)
		list.append(context->argument(i).toString());
		
	qDebug() << "TextImportDialog: [Script]: "<<static_currentScriptFile<<": "<<qPrintable(list.join(" "));
	
	return QScriptValue(list.join(" "));
}

QScriptValue TextImportDialog_script_findTextItem(QScriptContext *context, QScriptEngine *engine)
{
	QObject *slideObj = context->argument(0).toQObject();
	QString itemName  = context->argument(1).toString();
	if(!slideObj)
	{
		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): Must give Slide (QObject) as first argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	Slide *slide = dynamic_cast<Slide*>(slideObj);
	if(!slide)
	{
		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): First argument is not a Slide"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	if(itemName.isEmpty())
	{
		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): No item name given in second argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	TextBoxItem *tmpText = findTextItem(slide, itemName);
	return engine->newQObject(tmpText);
}

QScriptValue TextImportDialog_script_getFileList(QScriptContext *context, QScriptEngine *engine)
{
	QString folder = context->argument(0).toString();
	QString filter = context->argument(1).toString();
	
	QDir dir(folder);
	if(!dir.exists())
	{
		qDebug() << "TextImportDialog_script_getFileList(folder,filter): Folder given does not exist: "<<folder; 
		return engine->newArray();
	}

	QStringList files;
	if(!filter.isEmpty())
		files = dir.entryList(QStringList() << filter);
	else
		files = dir.entryList();
	
	QScriptValue array = engine->newArray(files.length());
	
	for(int i=0; i<files.length(); i++)
		array.setProperty(i, files[i]);
	
	return array;
} 

QScriptValue TextImportDialog_script_changeSlideBackground(QScriptContext *context, QScriptEngine */*engine*/)
{
	QObject *slideObj = context->argument(0).toQObject();
	QString itemName  = context->argument(1).toString();
	if(!slideObj)
	{
		qDebug() << "TextImportDialog_script_changeSlideBackground(slide,itemName): Must give Slide (QObject) as first argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	Slide *slide = dynamic_cast<Slide*>(slideObj);
	if(!slide)
	{
		qDebug() << "TextImportDialog_script_changeSlideBackground(slide,itemName): First argument is not a Slide"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	if(itemName.isEmpty())
	{
		qDebug() << "TextImportDialog_script_changeSlideBackground(slide,itemName): No item name given in second argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	
	QFileInfo info(itemName);
	QString ext = info.suffix();
	QString abs = info.absoluteFilePath();
	
	AbstractVisualItem::FillType fillType = AbstractVisualItem::None;
	if(MediaBrowser::isVideo(ext))
		fillType = AbstractVisualItem::Video;
	else
	if(MediaBrowser::isImage(ext))
		fillType = AbstractVisualItem::Image;
	else
		fillType = AbstractVisualItem::Solid;
		
	if(fillType != AbstractVisualItem::None)
	{
		
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
		
		bg->setFillType(fillType);
		if(fillType == AbstractVisualItem::Video)
		{
			bg->setFillVideoFile(abs);
		}
		else
		if(fillType == AbstractVisualItem::Image)
		{
			bg->setFillImageFile(abs);
		}
		else
		if(fillType == AbstractVisualItem::Solid)
		{
			bg->setFillBrush(QColor(abs));
		}
	}
		
	
	return QScriptValue(true);
}


// QScriptValue TextImportDialog_script_addVerses(QScriptContext *context, QScriptEngine *engine)
// {
// 	scriptEngine.globalObject().setProperty("dGroup", scriptGroup);
// 	QObject *slideObj = context->argument(0).toQObject();
// 	QString itemName  = context->argument(1).toString();
// 	if(!slideObj)
// 	{
// 		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): Must give Slide (QObject) as first argument"; 
// 		return QScriptValue(QScriptValue::NullValue);
// 	}
// 	Slide *slide = dynamic_cast<Slide*>(slideObj);
// 	if(!slide)
// 	{
// 		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): First argument is not a Slide"; 
// 		return QScriptValue(QScriptValue::NullValue);
// 	}
// 	if(itemName.isEmpty())
// 	{
// 		qDebug() << "TextImportDialog_script_findTextItem(slide,itemName): No item name given in second argument"; 
// 		return QScriptValue(QScriptValue::NullValue);
// 	}
// 	TextBoxItem *tmpText = findTextItem(slide, itemName);
// 	return engine->newQObject(tmpText);
// }

QScriptValue TextImportDialog_script_findFontSize(QScriptContext *context, QScriptEngine */*engine*/)
{
	QObject *obj = context->argument(0).toQObject();
	if(!obj)
	{
		qDebug() << "TextImportDialog_script_findFontSize(textbox): Must give Slide (QObject) as first argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	TextBoxItem *text = dynamic_cast<TextBoxItem*>(obj);
	if(!text)
	{
		qDebug() << "TextImportDialog_script_findFontSize(textbox): First argument is not a TextBoxItem"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	return QScriptValue(text->findFontSize());
}


QScriptValue TextImportDialog_script_intelligentCenterTextbox(QScriptContext *context, QScriptEngine */*engine*/)
{
	QObject *obj = context->argument(0).toQObject();
	if(!obj)
	{
		qDebug() << "TextImportDialog_script_intelligentCenterTextbox(textbox): Must give TextBoxItem (QObject) as first argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	TextBoxItem *text = dynamic_cast<TextBoxItem*>(obj);
	if(!text)
	{
		qDebug() << "TextImportDialog_script_intelligentCenterTextbox(textbox): First argument is not a TextBoxItem"; 
		return QScriptValue(false);
	}
	intelligentCenterTextbox(text);
	
	return QScriptValue(true);
}

QScriptValue TextImportDialog_script_changeFontSize(QScriptContext *context, QScriptEngine */*engine*/)
{
	QObject *obj = context->argument(0).toQObject();
	if(!obj)
	{
		qDebug() << "TextImportDialog_script_changeFontSize(textbox,fontSize): Must give Slide (QObject) as first argument"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	TextBoxItem *text = dynamic_cast<TextBoxItem*>(obj);
	if(!text)
	{
		qDebug() << "TextImportDialog_script_changeFontSize(textbox,fontSize): First argument is not a Slide"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	
	
	QScriptValue fontSizeVal = context->argument(1);
	if(!fontSizeVal.isNumber())
	{
		qDebug() << "TextImportDialog_script_changeFontSize(textbox,fontSize): Second argument is not a number"; 
		return QScriptValue(QScriptValue::NullValue);
	}
	double size = (double)fontSizeVal.toNumber();
	text->changeFontSize(size);
	return QScriptValue(text->findFontSize());
}

void TextImportDialog::doImport()
{
	// Get settings from UI
	
	QString fileName;
	QString pastedTempfile;
	
	if(m_ui->tabWidget->currentIndex() == 1)
	{
		// copy/paste buffer
		QString text = m_ui->plainTextEdit->toPlainText();
	
		pastedTempfile = tr("%1/dviz-textimportdialog-pasted.txt").arg(QDir::tempPath());
		
		QFile file(pastedTempfile);
		if(!file.open(QIODevice::WriteOnly))
		{
			QMessageBox::critical(this,tr("Can't Wriite Temp File"),QString(tr("Unable to write temp file %1")).arg(pastedTempfile));
			return;
		}
	
		QTextStream stream(&file);
		stream << text;
		file.close();
		
		fileName = pastedTempfile;
		
		qDebug() << "Wrote temp file: "<<pastedTempfile;
	}
	else
	{
		fileName = m_ui->filename->text();
		AppSettings::setPreviousPath("textimport",fileName);
	}
	
	// Store the originally-given file for the group name later
	QString originalFileName = fileName;
	
	bool isPlainText = m_ui->fmtPlain->isChecked();
	QSettings().setValue("textimport/plaintext",isPlainText);
	
	QString scriptFilename = m_ui->scriptFile->text();
	AppSettings::setPreviousPath("script",scriptFilename);
	
	QString preprocFilename = m_ui->preprocFile->text();
	AppSettings::setPreviousPath("preproc",preprocFilename);
	
	
	// Setup progress dialog
	QProgressDialog progress(QString(tr("Loading %1...")).arg(fileName),"",0,0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(0); // hide cancel button
	progress.setWindowIcon(QIcon(":/data/icon-d.png"));
	progress.setWindowTitle(QString(tr("Loading %1")).arg(fileName));
	progress.show();
	m_progress = &progress;
	QApplication::processEvents();
	
	
	
	qDebug() << "TextImportDialog::doImport(): Start, file:"<<fileName<<", isPLain? "<<isPlainText<<", script:"<<scriptFilename<<", preproc:"<<preprocFilename;
	
	QString tempFilename;
	if(!preprocFilename.isEmpty())
	{
		tempFilename = tr("%1/dviz-textimportdialog-temp.txt").arg(QDir::tempPath());
		
		QProcess proc;
		proc.start(preprocFilename, QStringList() << fileName << tempFilename);
		
		bool result = proc.waitForFinished();
		if(!result)
		{
			QMessageBox::critical(this,tr("Pre-Processor Problem"),QString(tr("Preproc %1 didn't finish properly. Check the console - not sure why.")).arg(fileName));
			return;
		}
		
		fileName = tempFilename;
		qDebug() << "TextImportDialog::doImport(): Preprocessor completed, using preproc temp file: "<<tempFilename;
	}
	
	// Load text file
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this,tr("Can't Read File"),QString(tr("Unable to open %1")).arg(fileName));
		return;
	}

	QTextStream stream(&file);
	QString fileContents = stream.readAll();
	
	if(!tempFilename.isEmpty())
		file.remove(); // file is closed before it is removed
		
	// Remove the paste buffer
	//if(!pastedTempfile.isEmpty())
	//	QFile(pastedTempfile).remove();
		
	// Load script file
	QScriptEngine scriptEngine;
	if(!scriptFilename.isEmpty())
	{
		// For debug output
		static_currentScriptFile = scriptFilename;
		 
		// Install some custom functions for the script
		QScriptValue fDebug        = scriptEngine.newFunction(TextImportDialog_script_qDebug);
		QScriptValue fFindTextItem = scriptEngine.newFunction(TextImportDialog_script_findTextItem);
		QScriptValue fChangeFntSz  = scriptEngine.newFunction(TextImportDialog_script_changeFontSize);
		QScriptValue fFindFntSz    = scriptEngine.newFunction(TextImportDialog_script_findFontSize);
		QScriptValue fCntrTextBox  = scriptEngine.newFunction(TextImportDialog_script_intelligentCenterTextbox);
		QScriptValue fFileList     = scriptEngine.newFunction(TextImportDialog_script_getFileList);
		QScriptValue fChangeBg     = scriptEngine.newFunction(TextImportDialog_script_changeSlideBackground);
		
		scriptEngine.globalObject().setProperty("debug",          fDebug);
		scriptEngine.globalObject().setProperty("findTextItem",   fFindTextItem);
		scriptEngine.globalObject().setProperty("changeFontSize", fChangeFntSz);
		scriptEngine.globalObject().setProperty("findFontSize",   fFindFntSz);
		scriptEngine.globalObject().setProperty("intelligentCenterTextbox", fCntrTextBox);
		scriptEngine.globalObject().setProperty("getFileList",    fFileList);
		scriptEngine.globalObject().setProperty("changeSlideBackground", fChangeBg);
		
		scriptEngine.globalObject().setProperty("InPrimaryGroup", true);
	
		QScriptValue scriptBibleBrowser = scriptEngine.newQObject(MainWindow::mw()->bibleBrowser());
		scriptEngine.globalObject().setProperty("BibleBrowser", scriptBibleBrowser);
		
 		// Read the file
		QFile scriptFile(scriptFilename);
		if(!scriptFile.open(QIODevice::ReadOnly))
		{
			QMessageBox::critical(this,tr("Can't Read Script"),QString(tr("Unable to open script %1")).arg(fileName));
			return;
		}
		
		// Evaulate the contents of the file
		QTextStream stream(&scriptFile);
		QString contents = stream.readAll();
		scriptFile.close();
		QScriptValue scriptResult = scriptEngine.evaluate(contents, fileName);
		
		// Alert user of syntax error in script
		if (scriptEngine.hasUncaughtException()) 
		{
			int line = scriptEngine.uncaughtExceptionLineNumber();
			//qDebug() << "uncaught exception at line" << line << ":" << result.toString();
			QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
				.arg(scriptFilename)
				.arg(line)
				.arg(scriptResult.toString()));
			return;
		}
	}
	
	// Generate the slides and set the group title
	qDebug() << "TextImportDialog::doImport(): Creating template and slides for primary group";
	SlideGroup *templateGroup = getTemplate();
	SlideGroup *group = generateSlideGroup(templateGroup, fileContents, isPlainText, scriptEngine, scriptFilename);
	if(!group)
	{
		// script error or other error, cancel import
		return;
	}
	
	
	// Apply group title using the original file name, just in case we used a pre-processor
	if(group->groupTitle().isEmpty())
		group->setGroupTitle(AbstractItem::guessTitle(QFileInfo(originalFileName).baseName()));
	
	
	qDebug() << "TextImportDialog::doImport(): Checking for alternate groups";
	
	// Set a flag so scripts can respond differently for alternate groups
	if(!scriptFilename.isEmpty())
		scriptEngine.globalObject().setProperty("InPrimaryGroup", false);
	
	// Create alternate groups for outputs if present in template
	QList<Output*> allOut = AppSettings::outputs();
	foreach(Output *out, allOut)
	{
		SlideGroup *outputTemplate = templateGroup->altGroupForOutput(out);
		if(outputTemplate)
		{
			SlideGroup *existingAlt = group->altGroupForOutput(out);
			
			qDebug() << "TextImportDialog::doImport(): [prep] Creating alternate group from template for output: "<<out->name()<<", existingAlt: "<<existingAlt;
			
			SlideGroup *altGroup = generateSlideGroup(outputTemplate, fileContents, isPlainText, scriptEngine, scriptFilename, existingAlt, group);
			if(!altGroup)
			{
				// script error or other error, cancel import
				delete group;
				return;
			}
			
			altGroup->setGroupTitle(tr("Alternate %1 for %2").arg(group->groupTitle()).arg(out->name()));
			
			group->setAltGroupForOutput(out, altGroup);
		}
	}
	
	qDebug() << "TextImportDialog::doImport(): Done, adding to main window";
	
	// Add to MainWindow's document
	group->setGroupNumber(MainWindow::mw()->currentDocument()->numGroups());
	MainWindow::mw()->currentDocument()->addGroup(group);
	close();
}

SlideGroup *TextImportDialog::generateSlideGroup(SlideGroup *templateGroup, QString fileContents, bool isPlainText, QScriptEngine &scriptEngine, QString scriptFilename, SlideGroup *append, SlideGroup *primary)
{
	SlideGroup *group = append ? append : new SlideGroup();
	
	int slideNumber = 0; //group->numSlides();
	
	bool hasScript = !scriptFilename.isEmpty();
	if(isPlainText)
	{
		//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] at start, text:"<<fileContents;
		m_progress->setMaximum(0);
		m_progress->setValue(-1);
		m_progress->setLabelText("Processing text...");
		QApplication::processEvents();
		
		int MinTextSize = 48;
		int MaxTextSize = 72;

		//QString text = text.replace("\r\n","\n");
		//QStringList list = text.split("\n\n");
		
		QStringList lines;
		
		// Do a fancy split on various breaking points in the text so we can fit more
		// text per slide. If we just split on line-end (\n), we potentially could get 
		// a line that wouldn't fit on the slide, or have a lot of empty space on the
		// end of a slide. Using a variety of break points makes the text fit better.
		int pos = 0;
		int lastPos = 0;
		QRegExp rx("[-;,\n:\\.]");
		while((pos = rx.indexIn(fileContents,pos)) != -1)
		{
			lines.append(fileContents.mid(lastPos,pos-lastPos+1));
			pos += rx.matchedLength();
			lastPos = pos;
		}
		
		TextBoxItem * tmpText = 0;
		int realHeight=0;
		
		// hold a pointer to the current slide outside the loop
		// so that at the end of the loop we can add the reference to the bottom if showFullRefAtBottomLast() is enabled
		Slide * currentSlide = 0;
		
		// Main logic block - loop over all the "lines" in the verseList (really, text fragments),
		// and try to fit as many of them as we can on a slide
		QStringList tmpList;
		
		// Use a boolean flag to ask for 'tmpText' to be recreated, instead of setting tmpText to 0 at end of loop because
		// we need to use tmpText outside the loop at the end if showFullRefAtBottomLast() is true
		bool recreateTextBox = true;
		
		int currentMinTextSize = MinTextSize;
		
		for(int x=0; x<lines.size(); x++)
		{
			//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] loop start, slideNumber:"<<slideNumber<<", x:"<<x<<"/"<<lines.size()<<", line: "<<lines[x];
			if(tmpList.isEmpty() &&
			   lines[x].trimmed().isEmpty())
			   continue;
	
			tmpList.append(lines[x]);
			
			if(recreateTextBox)
			{
				//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] recreateTextBox, slideNumber:"<<slideNumber;
				QString slideNameToUse = "";
				if(hasScript)
				{
					QScriptValue scriptTmplGroup = scriptEngine.newQObject(templateGroup);
					QScriptValue scriptGroup = scriptEngine.newQObject(group);
					
					scriptEngine.globalObject().setProperty("dGroup", scriptGroup);
					scriptEngine.globalObject().setProperty("dTemplateGroup", scriptTmplGroup);
					scriptEngine.globalObject().setProperty("dText", tmpList.join(""));
					scriptEngine.globalObject().setProperty("dFormat", isPlainText);
					
					QScriptValue scriptResult = scriptEngine.evaluate(tr("getTemplateSlideName()"));
		
					// Alert user of errors
					if (scriptEngine.hasUncaughtException()) 
					{
						int line = scriptEngine.uncaughtExceptionLineNumber();
						QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
							.arg(scriptFilename)
							.arg(line)
							.arg(scriptResult.toString()));
						
						delete group;
						return 0;
					}
					
					if(scriptResult.isValid() &&
					  !scriptResult.isUndefined())
					{
						slideNameToUse = scriptResult.toString();
						qDebug() << "TextImportDialog::generateSlideGroup(): [plain] Using slideNameToUse: "<<slideNameToUse;
					}
					else
						qDebug() << "TextImportDialog::generateSlideGroup(): [plain] No slide name given";
				}
				
				if(!slideNameToUse.isEmpty())
					currentSlide = getTemplateSlide(templateGroup, slideNameToUse, slideNumber)->clone();
				else
					currentSlide = getTemplateSlide(templateGroup, slideNumber)->clone();
				tmpText = findTextItem(currentSlide, "#text");
				currentMinTextSize = (int)tmpText->findFontSize();
				
				//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] recreateTextBox, got currentSlide:"<<currentSlide<<", tmpText:"<<tmpText;
			}
			
			mergeTextItem(tmpText, tmpList.join(""));
			tmpText->changeFontSize(currentMinTextSize);
			
			QRect currentFitRect = tmpText->contentsRect().toRect();
			
			//realHeight = tmpText->fitToSize(fitSize,MinTextSize,MaxTextSize);
			realHeight = tmpText->fitToSize(currentFitRect.size(), currentMinTextSize, MaxTextSize);
			//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] x:"<<x<<", realHeight:"<<realHeight<<", currentFitRect:"<<currentFitRect;
			
			// If the 'realHeight' is <0, it means that it didnt fit on the slide.
			// Therefore, we've found the max # of text frags that fit on this slide
			// so we should create a new slide, add the text, and then start searching again.
			if(realHeight < 0 || realHeight > currentFitRect.height())
			{
				//int tmpVar = realHeight; //debugging
				
				// More than one line, so the last line is the line that made the slide overflow the screen - 
				// therefore take it off and return it to the buffer for the next slide to use.
				
				QString tmpLine;
				if(tmpList.size() > 1)
				{
					// return last line to the file buffer
					tmpLine = tmpList.takeLast();
					//lines.prepend(line);
					//x--;
					
					mergeTextItem(tmpText, tmpList.join(""));
					tmpText->changeFontSize(currentMinTextSize);
					
					realHeight = tmpText->fitToSize(currentFitRect.size(),currentMinTextSize,MaxTextSize);
				}
				
				QString emptyTest = tmpList.join("");
				emptyTest.replace("\n","");
				if(emptyTest.isEmpty())
				{
					//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] Current text is empty, not adding empty slide";
				}
				else
				{
					//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] hit limit on "<<currentFitRect.height()<<", realHeight:"<<realHeight<<", tmpVar:"<<tmpVar<<", tmpLine:"<<tmpLine;
		
					// Verse slide
					intelligentCenterTextbox(tmpText);
					addSlideWithText(group, currentSlide, tmpText);
					
					slideNumber++;
					
					if(hasScript)
					{
						QScriptValue scriptTmplGroup = scriptEngine.newQObject(templateGroup);
						QScriptValue scriptGroup = scriptEngine.newQObject(group);
						QScriptValue scriptSlide = scriptEngine.newQObject(currentSlide);
						
						scriptEngine.globalObject().setProperty("dSlide", scriptSlide);
						scriptEngine.globalObject().setProperty("dGroup", scriptGroup);
						scriptEngine.globalObject().setProperty("dTemplateGroup", scriptTmplGroup);
						scriptEngine.globalObject().setProperty("dText", tmpList.join(""));
						scriptEngine.globalObject().setProperty("dFormat", isPlainText);
						
						QScriptValue scriptResult = scriptEngine.evaluate(tr("newSlideCreated(%1)").arg(slideNumber));
			
						// Alert user of errors
						if (scriptEngine.hasUncaughtException()) 
						{
							int line = scriptEngine.uncaughtExceptionLineNumber();
							QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
								.arg(scriptFilename)
								.arg(line)
								.arg(scriptResult.toString()));
							
							delete group;
							return 0;
						}
					}
				
					recreateTextBox = true;
				}
				
				realHeight = 0;
				
				tmpList.clear();
				if(!tmpLine.isEmpty())
					tmpList.append(tmpLine);
				//qDebug() << "TextImportDialog::generateSlideGroup(): [plain] end of new slide block, tmpList:"<<tmpList;
				
			}
		}
		
		// This block handles the case where we never did hit the maximum # of text frags per slide in the search block above
		// which would mean we have a dangling blob of text that never got added - so add it here. (Such as the last slide in a long list of slides)
		if(!tmpList.isEmpty() && recreateTextBox)
		{
			//currentSlide = getTemplateSlide(templateGroup, slideNumber)->clone();
			QString slideNameToUse = "";
			if(hasScript)
			{
				QScriptValue scriptTmplGroup = scriptEngine.newQObject(templateGroup);
				QScriptValue scriptGroup = scriptEngine.newQObject(group);
				
				scriptEngine.globalObject().setProperty("dGroup", scriptGroup);
				scriptEngine.globalObject().setProperty("dTemplateGroup", scriptTmplGroup);
				scriptEngine.globalObject().setProperty("dText", tmpList.join(""));
				scriptEngine.globalObject().setProperty("dFormat", isPlainText);
				
				QScriptValue scriptResult = scriptEngine.evaluate(tr("getTemplateSlideName()"));
	
				// Alert user of errors
				if (scriptEngine.hasUncaughtException()) 
				{
					int line = scriptEngine.uncaughtExceptionLineNumber();
					QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
						.arg(scriptFilename)
						.arg(line)
						.arg(scriptResult.toString()));
					
					delete group;
					return 0;
				}
				
				if(scriptResult.isValid() &&
				  !scriptResult.isUndefined())
				{
					slideNameToUse = scriptResult.toString();
					qDebug() << "TextImportDialog::generateSlideGroup(): [plain2] Using slideNameToUse: "<<slideNameToUse;
				}
				else
					qDebug() << "TextImportDialog::generateSlideGroup(): [plain2] No slide name given";
			}
			
			if(!slideNameToUse.isEmpty())
				currentSlide = getTemplateSlide(templateGroup, slideNameToUse, slideNumber)->clone();
			else
				currentSlide = getTemplateSlide(templateGroup, slideNumber)->clone();
				
			tmpText = findTextItem(currentSlide, "#text");
			currentMinTextSize = (int)tmpText->findFontSize();
			QRect currentFitRect = tmpText->contentsRect().toRect();
				
			mergeTextItem(tmpText, tmpList.join(""));
			tmpText->changeFontSize(currentMinTextSize);
			realHeight = tmpText->fitToSize(currentFitRect.size(),currentMinTextSize,MaxTextSize);
			
			intelligentCenterTextbox(tmpText);
			addSlideWithText(group, currentSlide, tmpText);
			
			slideNumber++;
		}
	
	}
	else
	{
		//qDebug() << "TextImportDialog::generateSlideGroup(): [vars] at start, text:"<<fileContents;
		fileContents = fileContents.replace("\r\n","\n");
		QStringList blocks = fileContents.split("\n\n");
		
		int MinTextSize = 48;
		int MaxTextSize = 72;
	
		// regex to extract variable name
		QRegExp nameRx("(#[^\\s]+):\\s*");
		
		m_progress->setMaximum(blocks.size());
		m_progress->setValue(0);
		m_progress->setLabelText("Processing text...");
		QApplication::processEvents();
		
		int blockNum = 0;
		foreach(QString block, blocks)
		{
			m_progress->setValue(m_progress->value()+1);
			
			// Parse the block and extract variable:value pairs, and store in varHash
			QStringList blockLines = block.split("\n");
			QHash<QString,QString> varHash;
			
			QString currentVarName;
			QStringList currentVarContent;
			foreach(QString line, blockLines)
			{
				if(line.startsWith("//"))
					continue;
					
				int namePos = nameRx.indexIn(line);
				if(namePos == 0) // variable pattern at start of line
				{
					// Store previous variable and content in the hash
					if(!currentVarName.isEmpty())
					{
						varHash[currentVarName] = currentVarContent.join("\n"); 
						currentVarContent.clear();
					}
					
					// extract variable name, store in hash
					currentVarName = nameRx.cap(1);
					
					// Remove the variable name and any trailing spaces from the start of the line
					line = line.replace(0, nameRx.matchedLength(), "");
				}
				
				currentVarContent.append(line);
			}
			
			if(!currentVarName.isEmpty())
				varHash[currentVarName] = currentVarContent.join("\n");
				
			// Setup the script engine for the next two calls
			if(hasScript)
			{
				QScriptValue scriptTmplGroup = scriptEngine.newQObject(templateGroup);
				QScriptValue scriptGroup = scriptEngine.newQObject(group);
				
				scriptEngine.globalObject().setProperty("dGroup", scriptGroup);
				scriptEngine.globalObject().setProperty("dTemplateGroup", scriptTmplGroup);
				scriptEngine.globalObject().setProperty("dFormat", isPlainText);
				
				// Create a variant map from our variable hash to pass to the script engine
				QScriptValue hash = scriptEngine.newObject();
				foreach(QString varName, varHash.keys())
					hash.setProperty(varName, varHash.value(varName));
				scriptEngine.globalObject().setProperty("dVars", hash);
			}
				
			// The 'false' means don't auto add the '#text' field so we don't assume the fields needed
			//Slide *currentSlide = getTemplateSlide(templateGroup, slideNumber, false)->clone();
			QString slideNameToUse = "";
			if(hasScript)
			{	
				QScriptValue scriptResult = scriptEngine.evaluate(tr("getTemplateSlideName()"));
	
				// Alert user of errors
				if (scriptEngine.hasUncaughtException()) 
				{
					int line = scriptEngine.uncaughtExceptionLineNumber();
					QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
						.arg(scriptFilename)
						.arg(line)
						.arg(scriptResult.toString()));
					
					delete group;
					return 0;
				}
				
				if(scriptResult.isValid() &&
				  !scriptResult.isUndefined())
				{
					slideNameToUse = scriptResult.toString();
					qDebug() << "TextImportDialog::generateSlideGroup(): [vars] Using slideNameToUse: "<<slideNameToUse;
				}
				else
					qDebug() << "TextImportDialog::generateSlideGroup(): [vars] No slide name given";
				 
			}
			
			Slide *currentSlide;
			if(!slideNameToUse.isEmpty())
				currentSlide = getTemplateSlide(templateGroup, slideNameToUse, slideNumber, false)->clone();
			else
				currentSlide = getTemplateSlide(templateGroup, slideNumber, false)->clone();
			
			// Store the slide number as an attribute (even though Slide::slideNumber() *probably* matches, this is just to be sure - for use in the {else} block
			if(!primary)
				currentSlide->setProperty("-primary-slideNumber", slideNumber);
			else
			{
				// We're creating an alternate slide group, so look thru the givin primary group for a slide that has our slideNumber
				// stored in its properties and use that as our primary slide
				foreach(Slide *slide, primary->slideList())
				{
					int primarySlideNum = slide->property("-primary-slideNumber").toInt();
					if(primarySlideNum == slideNumber)
					{
						currentSlide->setPrimarySlideId(slide->slideId());
						//qDebug() << "TextImportDialog::generateSlideGroup(): [vars] Linked primary slide to alt slide for slideNumber:"<<slideNumber;
						break;
					}
				}
				
				if(currentSlide->primarySlideId() < 0)
					qDebug() << "TextImportDialog::generateSlideGroup(): [vars] Could not find primary slide for alt slideNumber:"<<slideNumber;
			}
			//qDebug() << "TextImportDialog::generateSlideGroup(): [vars] varHash:" <<varHash;
			
			
			// Execute script, if present, prior to applying variable values to the slide so that
			// the script can use findTextItem() to locate items on the slide
			if(hasScript)
			{
				QScriptValue scriptSlide = scriptEngine.newQObject(currentSlide);
				scriptEngine.globalObject().setProperty("dSlide", scriptSlide);
				
				QScriptValue scriptResult = scriptEngine.evaluate(tr("aboutToCreateSlide(%1)").arg(slideNumber));
	
				// Alert user of errors
				if (scriptEngine.hasUncaughtException()) 
				{
					int line = scriptEngine.uncaughtExceptionLineNumber();
					QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
						.arg(scriptFilename)
						.arg(line)
						.arg(scriptResult.toString()));
					
					delete group;
					return 0;
				}
				
				// Copy variables back from the script engine
				QScriptValue dVars = scriptEngine.globalObject().property("dVars");
				QScriptValueIterator it(dVars);
				while(it.hasNext())
				{
					it.next();
					QString varName = it.name();
					QScriptValue propValue = it.value();
					
					if(propValue.isValid())
						varHash[varName] = propValue.toString();
					else
						varHash.remove(varName);
				} 
			}
			
				
			// Now we have varHash built up for this slide, match variables to blocks in template and generate slide
			foreach(QString varName, varHash.keys())
			{
				QString varContent = varHash.value(varName);
				
				int currentMinTextSize = MinTextSize;
				
				TextBoxItem *tmpText = findTextItem(currentSlide, varName);
				if(!tmpText)
				{
					//qDebug() << "TextImportDialog::generateSlideGroup(): [vars] Error: Variable "<<varName<<" from text file in slide block "<<blockNum<<" not found in template slide";
				}
				else
				{
					// Store template font size as minimum for use below
					currentMinTextSize = (int)tmpText->findFontSize();
					
					// Merge text with existing format
					mergeTextItem(tmpText, varContent);
			
					// Resize the text to squeeze into this textbox
					QRect currentFitRect = tmpText->contentsRect().toRect();
					tmpText->fitToSize(currentFitRect.size(), currentMinTextSize, MaxTextSize);
					
					// Hackish, yes, i know....better idea..?
					if(varName == "#text")
						intelligentCenterTextbox(tmpText);
						
					// Warming typically handled in addSlideWithText(), but with multiple text boxes, we want to warm them here
					
					// Delay warming the visual cache to increase UI responsiveness when quickly adding slides
					int delay = slideNumber * 5000;
					//qDebug()<<"SlideTemplateUtilities::addSlideWithText(): slideNbr:"<<slideNbr<<": delaying "<<delay<<"ms before warmVisualCache()";
					QTimer::singleShot(delay, tmpText, SLOT(warmVisualCache()));
				}
			}
			
			// Hide any fields that are in the template but not listed on this slide
			QStringList allFields = findTextItemNames(currentSlide);
			foreach(QString fieldName, allFields)
			{
				if(!varHash.contains(fieldName))
				{
					//qDebug() << "TextImportDialog::generateSlideGroup(): [vars] Field "<<fieldName<<" not found in slide block "<<blockNum<<", hiding field on slide";
					TextBoxItem *item = findTextItem(currentSlide, fieldName);
					if(item)
						item->setOpacity(0.); 
				}
			}
			
			slideNumber ++;
			
			// Add slide to group
			addSlideWithText(group, currentSlide, 0);
			
			if(currentSlide->slideName().startsWith("#"))
				currentSlide->setSlideName(AbstractItem::guessTitle(currentSlide->slideName().replace("#","")));
			
			// The script engine has already had the dSlide, etc variables setup if it reaches this block,
			// so just execute the script once more
			if(hasScript)
			{
				QScriptValue scriptResult = scriptEngine.evaluate(tr("newSlideCreated(%1)").arg(slideNumber));
		
				// Alert user of errors
				if (scriptEngine.hasUncaughtException()) 
				{
					int line = scriptEngine.uncaughtExceptionLineNumber();
					QMessageBox::critical(this,tr("Script Exception"),QString(tr("Uncaught Exception in file %1, line %2: \n\t%3\n\nImport canceled. Please fix script and try again."))
						.arg(scriptFilename)
						.arg(line)
						.arg(scriptResult.toString()));
					
					delete group;
					return 0;
				}
			}
			
			blockNum ++;
		}
	}
	
	return group;
}

void TextImportDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

