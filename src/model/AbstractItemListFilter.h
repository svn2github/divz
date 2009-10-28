#ifndef AbstractItemListFilter_H
#define AbstractItemListFilter_H

#include "AbstractItem.h"

class OutputInstance;
class AbstractItemListFilter
{
private:
	static QList<AbstractItemListFilter *> m_availableFilters;
	
public /*static*/:
	static QList<AbstractItemListFilter *> availableFilters();
	
	static void registerFilterInstance(AbstractItemListFilter *);
	static void removeFilterInstance(AbstractItemListFilter *);
	
public:
	AbstractItemListFilter();
	virtual ~AbstractItemListFilter();
	
	virtual QString filterName() const { return "No Filter"; }
	virtual QString filterDescription() const { return "No filter applied"; }
	
	virtual bool isMandatoryFor(OutputInstance * instance = 0) { return false; }
	
	// Base implementation of filter() passes item thru mutate() if approve() returns true and adds it to a new list
	virtual AbstractItemList filter(const AbstractItemList &);
	virtual bool approve(AbstractItem*); // default impl returns true
	virtual AbstractItem * mutate(const AbstractItem *item); // default impl returns 0
};



#endif
