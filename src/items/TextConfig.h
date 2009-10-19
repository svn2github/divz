#ifndef __TextConfig_h__
#define __TextConfig_h__

#include "AbstractConfig.h"
class RichTextEditorDialog;
class TextContent;

class TextConfig : public AbstractConfig
{
	Q_OBJECT
    public:
        TextConfig(TextContent * textContent, QGraphicsItem * parent = 0);
        ~TextConfig();

    private slots:
    	void applyTextSlot();
    	void resetTextSlot();
    private:
        // ::AbstractConfig
        //void slotOkClicked();
        TextContent * m_textContent;
        RichTextEditorDialog * m_editor;
};

#endif
