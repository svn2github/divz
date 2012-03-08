#ifndef SlideTemplateManager_H
#define SlideTemplateManager_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHash>
#include <QTextDocument>

class DocumentListModel;
class Document;
class SlideGroup;
class AbstractSlideGroupEditor;
class TextBoxItem;
class Slide;

namespace SlideTemplateUtilities
{
	void mergeTextItem(TextBoxItem *dest, TextBoxItem *source);
	void mergeTextItem(TextBoxItem *dest, const QString &source);
	QString mergeTextItem(const QString &destTemplate, const QString &source);
	TextBoxItem *findTextItem(Slide *slide, const QString& textKey);
	void textToDocument(QTextDocument& doc, const QString& text);
	void addSlideWithText(SlideGroup *group, Slide *slide, TextBoxItem *text);
	void intelligentCenterTextbox(TextBoxItem *text);
}


class SlideTemplateManager : public QObject
{
	Q_OBJECT

protected:
	friend class AppSettings;
	static void setupTemplateManager();
	
private:
	SlideTemplateManager();
	~SlideTemplateManager();
	
public:
	static SlideTemplateManager * instance() { return m_staticInstance; }
	
	typedef enum TemplateType
	{
		Generic,
		Bible,
		Songs,
		Logo
	};

	static QString typeToString(TemplateType);

	Document * templateDocument(TemplateType);
	SlideGroup * findTemplate(TemplateType type, int groupId);

private: /*static*/
	static SlideTemplateManager * m_staticInstance;
	
private:
	QString typeToFile(TemplateType);
	Document * loadOrCreate(TemplateType);
	QHash<TemplateType, Document*> m_docHash;
};


class TemplateSelectorWidget : public QWidget
{
	Q_OBJECT
public:
	TemplateSelectorWidget(SlideTemplateManager::TemplateType type = SlideTemplateManager::Generic, const QString& labelText = "Style:", QWidget *parent=0);
	~TemplateSelectorWidget();
	
	SlideGroup * selectedGroup();
	
signals:
	void currentGroupChanged(SlideGroup*);

public slots:
	void setSelectedGroup(SlideGroup*);
	void showSample(SlideGroup*);
	
private slots:
	void selectionChanged(int);
	void showSample();
	void editTemplate();
	void newTemplate();
	void delTemplate();
	
	void editorWindowClosed();

private:
	void setupUI();
	
	QString m_labelText;
	SlideTemplateManager::TemplateType m_type;
	QComboBox * m_templateBox;
	QPushButton * m_sampleButton;
	QPushButton * m_editButton;
	QPushButton * m_newButton;
	QPushButton * m_delButton;
	DocumentListModel *m_model;
	Document *m_doc;
	SlideGroup *m_selected;
	AbstractSlideGroupEditor *m_editWin;
};

#endif
