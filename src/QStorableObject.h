#ifndef QStorableObject_H
#define QStorableObject_H

#include <QObject>
#include <QByteArray>
#include <QVariant>

class QStorableObject : public QObject
{
	Q_OBJECT
public:
	virtual bool fromByteArray(QByteArray&);
	virtual QByteArray toByteArray();
	
	virtual bool ignoreProperty(const QString& /*propertyName*/, bool /*isLoading*/) { return false; }
	
	virtual bool fromVariantMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap toVariantMap();
};
	
#endif
