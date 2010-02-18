#ifndef SlideTemplateManager_H
#define SlideTemplateManager_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHash>

class DocumentListModel;
class Document;
class SlideGroup;
class AbstractSlideGroupEditor;


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
		Songs
	};

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
	TemplateSelectorWidget(SlideTemplateManager::TemplateType type = SlideTemplateManager::Generic, const QString& labelText = "Template:", QWidget *parent=0);
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
	
	void editorWindowClosed();

private:
	void setupUI();
	
	QString m_labelText;
	SlideTemplateManager::TemplateType m_type;
	QComboBox * m_templateBox;
	QPushButton * m_sampleButton;
	QPushButton * m_editButton;
	QPushButton * m_newButton;
	DocumentListModel *m_model;
	Document *m_doc;
	SlideGroup *m_selected;
	AbstractSlideGroupEditor *m_editWin;
};

#endif
