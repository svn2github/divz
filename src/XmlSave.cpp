/*****************************************************************************
* Copyright (C) 2008 TANGUY Arnaud <arn.tanguy@gmail.com>                    *
*                                                                            *
* This program is free software; you can redistribute it and/or modify       *
* it under the terms of the GNU General Public License as published by       *
* the Free Software Foundation; either version 2 of the License, or          *
* (at your option) any later version.                                        *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License along    *
* with this program; if not, write to the Free Software Foundation, Inc.,    *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
******************************************************************************/

#include "XmlSave.h"
/*#include "items/AbstractContent.h"
#include "items/ColorPickerItem.h"
#include "CPixmap.h"
#include "Desk.h"*/
#include "model/AbstractItem.h"
#include "model/Slide.h"

#include <QMessageBox>
#include <QFile>

XmlSave::XmlSave(const QString &filePath)
{
	// Open fotowall file
	file.setFileName(filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(0, tr("File Error"), tr("Error saving to the Fotowall file '%1'").arg(filePath));
		throw 0;
		return;
	}
	out.setDevice(&file);
	
	// This element contains all the others.
	m_rootElement = doc.createElement("slide-group");
	// All the contents will be saved in this element
	m_contentElements = doc.createElement("slide");
	
	// Add elements to the root node (slide-group).
	m_rootElement.appendChild(m_contentElements);
	
	// Add the root (and all the sub-nodes) to the document
	doc.appendChild(m_rootElement);
}

XmlSave::~XmlSave()
{
	//Add at the begining : <?xml version="1.0" ?>
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
	doc.insertBefore(noeud,doc.firstChild());
	//save in the file (4 spaces indent)
	doc.save(out, 4);
	file.close();
}

void XmlSave::saveSlide(Slide * slide)
{
	slide->toXml(m_contentElements, doc);
}
