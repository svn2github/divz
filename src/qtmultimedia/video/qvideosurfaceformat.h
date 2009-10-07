/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVIDEOSURFACEFORMAT_H
#define QVIDEOSURFACEFORMAT_H

#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qsize.h>
#include <QtGui/qimage.h>
#include <qtmultimedia/video/qvideoframe.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

//QT_MODULE(Multimedia)

class QDebug;

class QVideoSurfaceFormatPrivate;

class QVideoSurfaceFormat
{
public:
    enum Direction
    {
        TopToBottom,
        BottomToTop
    };

    enum ViewportMode
    {
        ResetViewport,
        KeepViewport
    };

    enum YuvColorSpace
    {
        YCbCr_Undefined,
        YCbCr_BT601,
        YCbCr_BT709,
        YCbCr_xvYCC601,
        YCbCr_xvYCC709,
        YCbCr_JPEG,
#ifndef qdoc
        YCbCr_CustomMatrix
#endif
    };

    typedef QPair<int, int> FrameRate;

    QVideoSurfaceFormat();
    QVideoSurfaceFormat(
            const QSize &size,
            QVideoFrame::PixelFormat pixelFormat,
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle);
    QVideoSurfaceFormat(const QVideoSurfaceFormat &format);
    ~QVideoSurfaceFormat();

    QVideoSurfaceFormat &operator =(const QVideoSurfaceFormat &format);

    bool operator ==(const QVideoSurfaceFormat &format) const;
    bool operator !=(const QVideoSurfaceFormat &format) const;

    bool isValid() const;

    QVideoFrame::PixelFormat pixelFormat() const;
    QAbstractVideoBuffer::HandleType handleType() const;

    QSize frameSize() const;
    void setFrameSize(const QSize &size, ViewportMode mode = ResetViewport);
    void setFrameSize(int width, int height, ViewportMode mode = ResetViewport);

    int frameWidth() const;
    int frameHeight() const;

    QRect viewport() const;
    void setViewport(const QRect &viewport);

    Direction scanLineDirection() const;
    void setScanLineDirection(Direction direction);

    FrameRate frameRate() const;
    void setFrameRate(const FrameRate &rate);
    void setFrameRate(int numerator, int denominator = 1);

    QSize pixelAspectRatio() const;
    void setPixelAspectRatio(const QSize &ratio);
    void setPixelAspectRatio(int width, int height);

    YuvColorSpace yuvColorSpace() const;
    void setYuvColorSpace(YuvColorSpace colorSpace);

    QSize sizeHint() const;

    QList<QByteArray> propertyNames() const;
    QVariant property(const char *name) const;
    void setProperty(const char *name, const QVariant &value);

private:
    QSharedDataPointer<QVideoSurfaceFormatPrivate> d;
};

#ifndef QT_NO_DEBUG_STREAM
//Q_MULTIMEDIA_EXPORT
QDebug operator<<(QDebug, const QVideoSurfaceFormat &);
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QVideoSurfaceFormat::FrameRate)
Q_DECLARE_METATYPE(QVideoSurfaceFormat::Direction)
Q_DECLARE_METATYPE(QVideoSurfaceFormat::YuvColorSpace)

QT_END_HEADER

#endif

