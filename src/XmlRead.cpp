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

#include "XmlRead.h"
// #include "frames/FrameFactory.h"
#include "model/AbstractItem.h"
#include "model/TextItem.h"
#include "model/Slide.h"
/*#include "items/PictureContent.h"
#include "items/TextContent.h"
#include "items/WebcamContent.h"
#include "CPixmap.h"
#include "Desk.h"
#include "MainWindow.h"*/
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QStringList>


XmlRead::XmlRead(const QString & filePath)
{
	// Load the file
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QMessageBox::critical(0, tr("Loading error"), tr("Unable to load file %1").arg(filePath));
		throw(0);
		return;
	}
	
	// And create the XML document into memory (with nodes...)
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(&file, false, error)) 
	{
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse file %1. The error was: %2").arg(filePath, *error));
		file.close();
		throw(0);
		return;
	}
	file.close();
	
	QDomElement root = doc.documentElement(); // The root node
// 	m_projectElement = root.firstChildElement("project"); // Get the project node
// 	m_deskElement = root.firstChildElement("desk");
	m_contentElement = root.firstChildElement("slide");
}

// void XmlRead::readProject(MainWindow *mainWindow)
// {
//     ModeInfo modeInfo;
//     QDomElement modeElement = m_projectElement.firstChildElement("mode");
//     QDomElement sizeElement = modeElement.firstChildElement("size");
//     if (!sizeElement.isNull()) {
//         float w = sizeElement.firstChildElement("w").text().toFloat();
//         float h = sizeElement.firstChildElement("h").text().toFloat();
//         modeInfo.setRealSizeInches(w, h);
// 
//     }
//     QDomElement dpiElement = sizeElement.firstChildElement("dpi");
//     if(!dpiElement.isNull()) {
//         int dpi = dpiElement.text().toInt();
//         modeInfo.setPrintDpi(dpi);
//     }
// 
//     int mode = modeElement.firstChildElement("id").text().toInt();
//     mainWindow->setModeInfo(modeInfo);
//     mainWindow->restoreMode(mode);
// }
// 
void XmlRead::readSlide(Slide *slide)
{
    // clear Desk [TODO: clear every content! or disasters happen]
//     qDeleteAll(desk->m_content);
//     desk->m_content.clear();
//     desk->m_configs.clear();
//     desk->m_backContent = 0;

	slide->fromXml(m_contentElement);
}
