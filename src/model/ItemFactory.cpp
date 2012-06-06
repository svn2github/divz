#include "ItemFactory.h"

// Include Items Here

#include <QFile>
#include <QSettings>
#include <QMutexLocker>

Q_GLOBAL_STATIC(ItemFactory, d)
/*
#define FRAME_DEF 0x0001
#define FRAME_HEART 0x0002
#define FRAME_NOFRAME 0x0003*/

quint32 ItemFactory::nextId() { return d()->m_idCounter ++; }


// STATICS
QList<quint32> ItemFactory::classes()
{
    QList<quint32> classes;
//     classes.append(FRAME_DEF);
//     classes.append(FRAME_HEART);
//     classes.append(FRAME_NOFRAME);
    return classes;
}

AbstractItem * ItemFactory::createItem(quint32 itemClass)
{
    if (itemClass == AbstractItem::Abstract)
        return 0;
//     else if (frameClass == FRAME_DEF)
//         return new StandardFrame();
//     else if (frameClass == FRAME_HEART)
//         return new HeartFrame();
//     else if (frameClass == FRAME_NOFRAME)
//         return new EmptyFrame();
//     else if (d()->m_svgMap.contains(frameClass))
//         return new PlasmaFrame(frameClass, d()->m_svgMap[ frameClass ]);
    else
        qWarning( "ItemFactory::createFrame: unknown item for class %x, falling back to default", itemClass);
    return 0;
}


// class impl
ItemFactory::ItemFactory()
{
	QSettings s;
	m_idCounter = (quint32)s.value("items/id_counter", 100).toInt();
}

ItemFactory::~ItemFactory()
{
	QSettings s;
	s.setValue("items/id_counter", (int)m_idCounter);
}
