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

#ifndef __StandardFrame_h__
#define __StandardFrame_h__

#include "Frame.h"
class QPainter;

class StandardFrame : public Frame
{
    public:
        // ::Frame
        quint32 frameClass() const;
        QRect frameRect(const QRect & contentsRect) const;
        void layoutButtons(QList<ButtonItem *> buttons, const QRect & frameRect) const;
        void layoutText(QGraphicsItem * textItem, const QRect & frameRect) const;
        void paint(QPainter * painter, const QRect & frameRect, bool selected, bool opaqueContents);

};

#endif
