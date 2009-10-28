#ifndef AbstractItemFilter_H
#define AbstractItemFilter_H

#include "AbstractItem.h"
#include <QList>

class OutputInstance;

class AbstractItemFilter;
typedef QList<AbstractItemFilter*> AbstractItemFilterList;

class AbstractItemFilter : public QObject
{
	Q_OBJECT 
	
	// make it a Q_OBJECT so derived objects can use Q_PROPERTY
	
private:
	static AbstractItemFilterList m_availableFilters;
	
public /*static*/:
	static AbstractItemFilterList availableFilters();
	
	static void registerFilterInstance(AbstractItemFilter *);
	static void removeFilterInstance(AbstractItemFilter *);
	
public:
	AbstractItemFilter();
	virtual ~AbstractItemFilter();
	
	virtual QString filterName() const { return "No Filter"; }
	virtual QString filterDescription() const { return "No filter applied"; }
	
	virtual bool isMandatoryFor(OutputInstance * /*instance */= 0) { return false; }
	
	// Base implementation of filter() passes item thru mutate() if approve() returns true and adds it to a new list
	//virtual AbstractItemList filter(const AbstractItemList &);
	virtual bool approve(AbstractItem*); // default impl returns true
	virtual AbstractItem * mutate(const AbstractItem *item); // default impl returns 0
};



#endif
