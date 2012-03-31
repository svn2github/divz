#include "QStorableObject.h"

#include <QDebug>
#include <QMetaProperty>
#include <QImage>

bool QStorableObject::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;

	if(map.isEmpty())
	{
		qDebug() << "Error: QStorableObject::fromByteArray(): Map is empty, unable to load object.";
		return false;
	}

	return fromVariantMap(map);
}

bool QStorableObject::fromVariantMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = map[name];

		QString propName(name);

		// Allow subclasses to ignore loading properties
		if(ignoreProperty(propName, true)) // true = loading, false would mean saving
			continue;

		if(value.isValid())
		{
			if(onlyApplyIfChanged)
			{
				if(property(name) != value)
				{
					//qDebug() << "QStorableObject::loadPropsFromMap():"<<this<<": [onlyApplyIfChanged] i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;
					setProperty(name,value);
				}
			}
			else
			{
				//qDebug() << "QStorableObject::loadPropsFromMap():"<<this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value<<" (calling set prop)";

				setProperty(name,value);
				//m_props[name] = value;
			}
		}
	}
	
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
 	foreach(QString propName, map.keys())
	{
		// Allow subclasses to ignore loading properties
		if(ignoreProperty(propName, true)) // true = loading, false would mean saving
			continue;
		
		QVariant value = map[propName];
		
		// Check for the type of the property - if it's an image
		// then we load it differently 
		bool isImageProp = false;
		int idx = metaobject->indexOfProperty(qPrintable(propName));
		if(idx > 0)
		{
			QMetaProperty meta = metaobject->property(idx);
			if(meta.type() == QVariant::Image)
				isImageProp = true;
		}
		// Property not statically defined in header, it may be a 
		// dynamic property - in that case, load stored type from file
		else
		{
			// Prefix with _q so it's ignored when saved
			QVariant::Type type = (QVariant::Type)map.value(QString("_q_dviz_propType_%1").arg(propName)).toInt();
			if(type == QVariant::Image)
				isImageProp = true;
		}
			
		// If the prop is *intended* to be an image, but stored as a byte array,
		// then load accordingly
		if(isImageProp &&
			value.type() == QVariant::ByteArray)
		{
			//qDebug() << this << ": Loading image for prop:"<<propName;
			
			QSize size = map[QString("_q_dviz_img_%1_size").arg(propName)].toSize();
			QImage::Format format = (QImage::Format)map[QString("_q_dviz_img_%1_format").arg(propName)].toInt();

			// Load image from byte array because it's quicker
			// than relying on QVariant to save/load as a PNG
			QByteArray bytes = value.toByteArray();
			QImage image((const uchar*)bytes.constData(), size.width(), size.height(), format);
			
			// Force the image to copy the data from the byte array buffer to it's internal buffer
			image = image.copy();

			// Overwrite the bytes loaded from the map with the newly-loaded image
			value = image;
		}
		
		//qDebug() << "AbstractItem::clone():"<<itemName()<<": prop:"<<name<<", value:"<<value;
		if(value.isValid())
		{
			if(onlyApplyIfChanged)
			{
				if(property(qPrintable(propName)) != value)
				{
					//qDebug() << "QStorableObject::loadPropsFromMap():"<<this<<": [onlyApplyIfChanged] i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;
					setProperty(qPrintable(propName),value);
				}
			}
			else
			{
				//setProperty(name,value);
				setProperty(qPrintable(propName),value);
			}
		}
		else
		{
			qDebug() << "QStorableObject::fromVariantMap: Unable to load property for "<<propName<<", got invalid property from map";
		}
	}
	
	return true;
}

QByteArray QStorableObject::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	stream << toVariantMap();
	return array;
}

QVariantMap QStorableObject::toVariantMap()
{
	QVariantMap map;
	
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);
		
		if(ignoreProperty(QString(name), false)) // true = loading, false=saving
			continue;

		if(value.type() == QVariant::Image)
		{
			// Store images as bytearrays because the default storage method is to compress as PNGs - which
			// takes a while on a document with 100s of slides (each possibly with multiple PNGs as props)
			QImage image = value.value<QImage>();
			QByteArray data((const char*)image.bits(), image.byteCount());
			value = data;
			
			// Must store size and format for loading image from byte array
			map[QString("_q_dviz_img_%1_size").arg(name)]   = image.size();
			map[QString("_q_dviz_img_%1_format").arg(name)] = (int)image.format();
		}
		
		//qDebug() << "AbstractItem::toByteArray(): [1] "<<this<<": prop:"<<name<<", value:"<<value;
		//item->setProperty(name,value);
		map[name] = value;
	}
	
	// Store dynamic properties as well
	QList<QByteArray> dynamicProps = dynamicPropertyNames();
	foreach(QByteArray name, dynamicProps)
	{
		if(name.startsWith("_q"))
			continue;
			
		if(ignoreProperty(QString(name), false)) // true = loading, false=saving
			continue;
		
		QVariant var = property(name.data());
		
		//qDebug() << "AbstractItem::toByteArray(): [2] "<<this<<": prop:"<<name<<", value:"<<var;
		
		// dont store userdefined types
		if(var.isValid() && (int)var.type() < 127)
		{
			if(var.type() == QVariant::Image)
			{
				QString propName(name);
				
				//qDebug() << this << ": Saving image for prop:"<<propName;
				
				QImage image = var.value<QImage>();
				
				// Must store size and format for loading image from byte array
				map[QString("_q_dviz_img_%1_size").arg(propName)]   = image.size();
				map[QString("_q_dviz_img_%1_format").arg(propName)] = (int)image.format();
				// Since this is a dynamic prop, the loading code needs to have a way
				// to look up the type of this prop before assuming its an image -
				// so store the variant type in the map as well
				map[QString("_q_dviz_propType_%1").arg(propName)]   = (int)var.type();
				
				// Store images as bytearrays because the default storage method is to compress as PNGs - which
				// takes a while on a document with 100s of slides (each possibly with multiple PNGs as props)
				QByteArray data((const char*)image.bits(), image.byteCount());
				var = data;
			}
			
			map[QString(name)] = var;
			//qDebug() << "GLDrawable::propsToMap():"<<(QObject*)this<<": dynamic prop:"<<name<<", value:"<<var;
		}
	}

	return map;
}
