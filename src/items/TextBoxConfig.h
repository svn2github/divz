#ifndef __TextBoxConfig_h__
#define __TextBoxConfig_h__

#include "GenericItemConfig.h"
class RichTextEditorDialog;
class TextBoxContent;
class TextItem;
#include <QPointer>

class TextBoxConfig : public GenericItemConfig
{
	Q_OBJECT
public:
	TextBoxConfig(TextBoxContent * textContent, QGraphicsItem * parent = 0);
	~TextBoxConfig();
	
private slots:
	void itemChanged(QString fieldName, QVariant value, QVariant);
	
private:
	// ::AbstractConfig
	void slotOkClicked();
	TextBoxContent * m_textContent;
	RichTextEditorDialog * m_editor;
	bool m_saving;
	QPointer<TextItem> m_model;
};

#endif
