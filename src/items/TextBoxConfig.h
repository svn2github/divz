/***************************************************************************
 *                                                                         *
 *   This file is part of the Fotowall project,                            *
 *       http://code.google.com/p/fotowall                                 *
 *                                                                         *
 *   Copyright (C) 2009 by Enrico Ros <enrico.ros@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __TextBoxConfig_h__
#define __TextBoxConfig_h__

#include "GenericItemConfig.h"
class RichTextEditorDialog;
class TextBoxContent;

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
};

#endif
